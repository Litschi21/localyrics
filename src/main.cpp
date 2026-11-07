#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
  QCoreApplication::setApplicationName("localyrics");

  QApplication app(argc, argv);
  app.setStyleSheet(R"(
    QLabel {
      font-family: JetBrains Mono;
      color: white;
    }

    QListWidget {
      border: none;
      outline: none;
    }

    QListWidget::item:hover {
      background: transparent;
    }

    QListWidget::item {
      border: none;
      border-radius: 5px;
      padding-left: 5px;
      padding-right: 5px;
      margin: 5px;
    }

    QListWidget::item::selected {
      background-color: #5A5A5A;
    }

    QScrollBar {
      height: 0px;
    }
  )");

  MainWindow window;
  window.show();

  return app.exec();
}
