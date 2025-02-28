#include "COled.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {

  COled oled;

  // oled.autodetect();

  QApplication a(argc, argv);
  MainWindow w(nullptr, &oled);
  w.show();
  return a.exec();
}
