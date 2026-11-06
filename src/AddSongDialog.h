#ifndef ADDSONGDIALOG_H
#define ADDSONGDIALOG_H

#include <QDialog>

#include "MainWindow.h"

namespace Ui {
class AddSongDialog;
}

class AddSongDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AddSongDialog(MainWindow* mainWindow, QWidget* parent = nullptr);
  ~AddSongDialog();

  void chooseFile();
  void editExisting(int row, const QString& title, const QString& artist);

 private:
  Ui::AddSongDialog* ui;
  MainWindow* mainWin;
  QString songFile;
  int editRow = -1;
};

#endif  // ADDSONGDIALOG_H
