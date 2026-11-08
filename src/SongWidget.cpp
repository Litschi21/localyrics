#include "SongWidget.h"

#include "ui_SongWidget.h"

void SongWidget::setNum(QString num) {
  ui->songNum->setText(num);

  int width = ui->songNum->fontMetrics().horizontalAdvance(num);
  ui->songNum->setFixedWidth(width * NUM_GAP_MULT);
}

void SongWidget::setTitle(QString title) { ui->titleLabel->setText(title); }

void SongWidget::setArtist(QString artist) { ui->artistLabel->setText(artist); }

SongWidget::SongWidget(QWidget* parent) : QWidget(parent), ui(new Ui::SongWidget) {
  ui->setupUi(this);
}

SongWidget::~SongWidget() { delete (ui); }
