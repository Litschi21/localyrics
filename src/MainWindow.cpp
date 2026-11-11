#include "MainWindow.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QStandardPaths>

#include <cmath>
#include <cstddef>

#include "AddSongDialog.h"
#include "SongBar.h"
#include "SongWidget.h"
#include "miniaudio.h"
#include "ui_MainWindow.h"


static QString libraryPath() {
  const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dir);
  return dir + "/songs.json";
}

void MainWindow::saveSongs() const {
  QJsonArray arr;
  for (const Song& s : songs) {
    arr.append(QJsonObject{{"title", s.title}, {"artist", s.artist}, {"file", s.songFile}});
  }

  QFile f(libraryPath());
  if (f.open(QIODevice::WriteOnly)) { f.write(QJsonDocument(arr).toJson()); }
}

void MainWindow::loadSongs() {
  QFile f(libraryPath());
  if (!f.open(QIODevice::ReadOnly)) { return; }

  for (const QJsonValue& v : QJsonDocument::fromJson(f.readAll()).array()) {
    const QJsonObject o = v.toObject();
    this->addSong(o["title"].toString(), o["artist"].toString(), o["file"].toString());
  }
}

void MainWindow::startSong(std::size_t id) {
  currentId = id;
  if (isPlaying) {
    ma_sound_uninit(&currentSong);
    isPlaying = false;
  }

  ma_result res = ma_sound_init_from_file(&engine, songs[id].songFile.toStdString().c_str(), 0,
                                          nullptr, nullptr, &currentSong);

  if (res == MA_SUCCESS) {
    ma_sound_start(&currentSong);
    isPlaying = true;

    songBar->setPlaying(true);
    songBar->setText(songs[id].title, songs[id].artist);

    float length{};
    ma_sound_get_length_in_seconds(&currentSong, &length);
    songBar->setLength(length);
  } else {
    QMessageBox::critical(this, "Song Error", "Failed to start song");
  }
}

void MainWindow::addSong(QString title, QString artist, QString songFile) {
  songs.emplace_back(Song(title, artist, songFile));

  auto* songWidget = new SongWidget(this);
  songWidget->setNum(QString::number(ui->songListWidget->count() + 1));
  songWidget->setTitle(title);
  songWidget->setArtist(artist);

  auto* songItem = new QListWidgetItem;
  songItem->setSizeHint(songWidget->sizeHint());

  ui->songListWidget->addItem(songItem);
  ui->songListWidget->setItemWidget(songItem, songWidget);

  saveSongs();
}

void MainWindow::deleteSong() {
  QList<QListWidgetItem*> selectedItems = ui->songListWidget->selectedItems();
  if (selectedItems.empty()) { return; }

  auto* selected = selectedItems.first();
  songs.erase(songs.begin() + ui->songListWidget->row(selected));

  for (int i{}; i < ui->songListWidget->count(); ++i) {
    QListWidgetItem* item = ui->songListWidget->item(i);
    if (i > ui->songListWidget->row(selected)) {
      auto* songWidget = qobject_cast<SongWidget*>(ui->songListWidget->itemWidget(item));
      songWidget->setNum(QString::number(i));
      if (static_cast<std::size_t>(i) < currentId) { --currentId; }
    }
  }

  delete selected;
  saveSongs();
}

void MainWindow::updateSong(int row, const QString& title, const QString& artist) {
  songs[row].title = title;
  songs[row].artist = artist;

  auto* widget =
      qobject_cast<SongWidget*>(ui->songListWidget->itemWidget(ui->songListWidget->item(row)));
  widget->setTitle(title);
  widget->setArtist(artist);

  if (isPlaying && static_cast<std::size_t>(row) == currentId) songBar->setText(title, artist);
  saveSongs();
}

void MainWindow::changeVolume(float volume) {
  ma_engine_set_volume(&engine, std::pow(volume / 100.0F, VOL_CURVE));
}

void MainWindow::nextSong() {
  if (songs.empty()) { return; }

  std::size_t next = QRandomGenerator::global()->bounded(static_cast<quint32>(songs.size()));
  startSong(next);
}

void MainWindow::pauseSong() {
  songBar->setPlaying(false);
  emit songBar->playToggled(false);
}

void MainWindow::showSongContextMenu(const QPoint& pos) {
  QMenu songContextMenu(this);
  const bool hasSelection = !ui->songListWidget->selectedItems().empty();

  songContextMenu.addAction("Add Song", this, [this] {
    auto* dialog = new AddSongDialog(this, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
  });

  auto* editSongAction = new QAction(this);
  editSongAction->setText("Edit Song");
  editSongAction->setEnabled(hasSelection);
  connect(editSongAction, &QAction::triggered, this, [this] {
    const int row = ui->songListWidget->row(ui->songListWidget->selectedItems().first());

    auto* dialog = new AddSongDialog(this, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->editExisting(row, songs[row].title, songs[row].artist);
    dialog->show();
  });
  songContextMenu.addAction(editSongAction);

  auto* deleteSongAction = new QAction(this);
  deleteSongAction->setText("Delete Song");
  connect(deleteSongAction, &QAction::triggered, this, [this] { deleteSong(); });
  deleteSongAction->setEnabled(!ui->songListWidget->selectedItems().empty());

  songContextMenu.addAction("Import Multiple Files", this, [this] {
    const QStringList files =
        QFileDialog::getOpenFileNames(this, "Import Songs", "", "MP3, FLAC, or WAV Files (*.mp3 *.flac *.wav)");
    for (const QString& f : files) addSong(QFileInfo(f).completeBaseName(), "Unknown artist", f);
  });

  songContextMenu.addAction(deleteSongAction);
  songContextMenu.exec(mapToGlobal(pos));
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/localyrics.svg"));

  ui->homeLayout->addWidget(songBar);

  connect(songBar, &SongBar::playToggled, this, [this](bool playing) {
    if (!isPlaying) { return; }

    if (playing) {
      ma_sound_start(&currentSong);
    } else {
      ma_sound_stop(&currentSong);
    }
  });

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QListWidget::customContextMenuRequested, this, &MainWindow::showSongContextMenu);

  connect(ui->songListWidget, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem* item) { MainWindow::startSong(ui->songListWidget->row(item)); });

  connect(songBar, &SongBar::volumeChanged, this,
          [this](int volume) { changeVolume(static_cast<float>(volume)); });

  connect(songBar, &SongBar::muteToggled, this,
          [this](bool muted) { changeVolume(muted ? 0.f : songBar->volume()); });

  posTimer = new QTimer(this);
  connect(posTimer, &QTimer::timeout, this, [this] {
    if (!isPlaying) { return; }

    float curr{};
    float length{};
    ma_sound_get_cursor_in_seconds(&currentSong, &curr);
    ma_sound_get_length_in_seconds(&currentSong, &length);
    songBar->setProgress(curr, length);
    songBar->setTimeProgress(curr);

    if (!ma_sound_at_end(&currentSong)) { return; }
    if (songBar->repeatOne() || songs.size() <= 1) {
      startSong(currentId);
    } else {
      nextSong();
    }
  });
  posTimer->start(250);

  connect(songBar, &SongBar::sliderMoved, this, [this](float seconds) {
    if (!isPlaying) { return; }

    ma_uint32 sampleRate{};
    ma_sound_get_data_format(&currentSong, nullptr, nullptr, &sampleRate, nullptr, 0);
    ma_sound_seek_to_pcm_frame(&currentSong, static_cast<ma_uint64>(seconds * sampleRate));
  });

  connect(songBar, &SongBar::nextClicked, this, [this] { nextSong(); });

  if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) {
    QMessageBox::critical(this, "Audio Engine Error", "Failed to initialized Audio Engine");
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
    return;
  }

  loadSongs();
  nextSong();
  pauseSong();
}

MainWindow::~MainWindow() {
  if (isPlaying) { ma_sound_uninit(&currentSong); }

  ma_engine_uninit(&engine);
  delete ui;
}
