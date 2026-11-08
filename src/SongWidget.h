#ifndef SONGWIDGET_H
#define SONGWIDGET_H

#include <QWidget>

#define NUM_GAP_MULT 1.7

namespace Ui {
class SongWidget;
}

class SongWidget : public QWidget {
  Q_OBJECT

 public:
  explicit SongWidget(QWidget* parent = nullptr);
  ~SongWidget();

  void setNum(QString num);
  void setTitle(QString title);
  void setArtist(QString artist);

 private:
  Ui::SongWidget* ui;
};

#endif  // SONGWIDGET_H
