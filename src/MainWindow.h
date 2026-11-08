#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <vector>

#include "SongBar.h"
#include "miniaudio.h"

#define VOL_CURVE 0.7f

namespace Ui {
class MainWindow;
}

struct Song {
  QString title;
  QString artist;
  QString songFile;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  void startSong(size_t id);
  void deleteSong();
  void updateSong(int row, const QString& title, const QString& artist);
  void changeVolume(float volume);
  void nextSong();
  void pauseSong();
  void addSong(QString title, QString artist, QString songFile);
  void showSongContextMenu(const QPoint& pos);

  void saveSongs() const;
  void loadSongs();

 private:
  Ui::MainWindow* ui;
  ma_engine engine;
  SongBar* songBar = new SongBar(this);
  QTimer* posTimer = nullptr;
  size_t currentId = 0;

  bool isPlaying = false;
  ma_sound currentSong;
  std::vector<Song> songs{};
};

#endif  // MAINWINDOW_H
