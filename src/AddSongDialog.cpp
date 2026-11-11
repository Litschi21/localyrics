#include "AddSongDialog.h"

#include <QFileDialog>

#include "MainWindow.h"
#include "ui_AddSongDialog.h"

void AddSongDialog::chooseFile() {
  const QString file =
      QFileDialog::getOpenFileName(this, "Choose File", "", "MP3, FLAC, or WAV Files (*.mp3 *.flac *.wav)");

  auto fileInfo = QFileInfo(file);
  if (!file.isEmpty()) {
    if (ui->useFilenameAsTitle->checkState()) {
      ui->titleEdit->setText(fileInfo.completeBaseName());
    }

    ui->addButton->setEnabled(true);
    ui->fileButton->setText(fileInfo.fileName());
    songFile = file;
  }
}

void AddSongDialog::editExisting(int row, const QString& title, const QString& artist) {
  editRow = row;
  setWindowTitle("Edit Song");
  ui->titleEdit->setText(title);
  ui->artistEdit->setText(artist);
  ui->addButton->setText("Save");
  ui->addButton->setEnabled(true);
  ui->fileButton->hide();
  ui->useFilenameAsTitle->hide();
}

AddSongDialog::AddSongDialog(MainWindow* mainWindow, QWidget* parent)
    : QDialog(parent), ui(new Ui::AddSongDialog), mainWin(mainWindow) {
  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/localyrics.svg"));

  connect(ui->fileButton, &QPushButton::clicked, this, [this]() { chooseFile(); });

  connect(ui->addButton, &QPushButton::clicked, this, [this]() {
    if (editRow >= 0)
      mainWin->updateSong(editRow, ui->titleEdit->text(), ui->artistEdit->text());
    else
      mainWin->addSong(ui->titleEdit->text(), ui->artistEdit->text(), songFile);

    this->close();
  });
}

AddSongDialog::~AddSongDialog() { delete ui; }
