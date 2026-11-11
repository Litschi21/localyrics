#ifndef SONGBAR_H
#define SONGBAR_H

#include <QWidget>

namespace Ui {
class SongBar;
}

class SongBar : public QWidget {
  Q_OBJECT

 signals:
  void playToggled(bool playing);
  void volumeChanged(int volume);
  void muteToggled(bool muted);
  void sliderMoved(float seconds);
  void nextClicked();

 public:
  explicit SongBar(QWidget* parent = nullptr);
  ~SongBar();

  void setPlaying(bool playing);
  void setText(QString title, QString artist);
  void setProgress(float curr, float length);
  void setTimeProgress(int sec);
  void setLength(int sec);
  [[nodiscard]] float volume() const;
  [[nodiscard]] bool repeatOne() const;

 private:
  Ui::SongBar* ui;
};

#endif  // SONGBAR_H
