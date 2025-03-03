#include "COled.h"
#include "mainwindow.h"

#include <QApplication>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // creating Objects:
  COled oled;               // for Oled Display
  QMediaPlayer player;      // for playing audiofiles
  QAudioOutput audioOutput; // needed for setting the audio output

  // Set the players audio output:
  player.setAudioOutput(&audioOutput);

  MainWindow w(
      nullptr, &oled, &player,
      &audioOutput); // passing all the objects as pointers to the main window
  w.show();
  return a.exec();
}
