#include "SongBar.h"

#include <QSignalBlocker>

#include "ui_SongBar.h"

void SongBar::setPlaying(bool playing) {
  QSignalBlocker blocker(ui->playButton);
  ui->playButton->setChecked(playing);
}

void SongBar::setText(QString title, QString artist) {
  ui->songTitle->setText(title);
  ui->songArtist->setText(artist);
}

void SongBar::setProgress(float curr, float length) {
  if (ui->songProgressSlider->isSliderDown()) return;
  ui->songProgressSlider->setMaximum(static_cast<int>(length * 1000));
  ui->songProgressSlider->setValue(static_cast<int>(curr * 1000));
}

QString formatTime(int sec) {
  int min = std::round(sec / 60);
  QString lengthText = QString::number(min) + ':' + QString("%1").arg(sec % 60, 2, 10, QChar('0'));
  return lengthText;
}

void SongBar::setTimeProgress(int sec) {
  QString lengthText = formatTime(sec);
  ui->currSongTimeLabel->setText(lengthText);
}

void SongBar::setLength(int sec) {
  QString lengthText = formatTime(sec);
  ui->songLengthLabel->setText(lengthText);
}

float SongBar::volume() const { return ui->volumeBar->value(); }

bool SongBar::repeatOne() const { return ui->repeatButton->isChecked(); }

SongBar::SongBar(QWidget* parent) : QWidget(parent), ui(new Ui::SongBar) {
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground, true);

  ui->songTitle->setText("Title");
  ui->songArtist->setText("Artist");

  connect(ui->playButton, &QPushButton::toggled, this, &SongBar::playToggled);
  connect(ui->volumeBar, &QSlider::valueChanged, this, &SongBar::volumeChanged);
  connect(ui->muteButton, &QPushButton::toggled, this, &SongBar::muteToggled);
  connect(ui->songProgressSlider, &QSlider::sliderReleased, this,
          [this]() { emit sliderMoved(ui->songProgressSlider->value() / 1'000.f); });
  connect(ui->nextButton, &QPushButton::clicked, this, &SongBar::nextClicked);
}

SongBar::~SongBar() { delete (ui); }
