#include "COled.h"
#include "dbconnect.h"
#include "mainwindow.h"

#include <QApplication>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>
#include <QtSql/QSql>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // creating Objects:
  COled oled;               // for Oled Display
  QMediaPlayer player;      // for playing audiofiles
  QAudioOutput audioOutput; // needed for setting the audio output

  // Set the players audio output:
  player.setAudioOutput(&audioOutput);

  // Create Database, stop program if not succesfull
  if (!createConnection()) {
    return EXIT_FAILURE;
  }

  MainWindow w(
      nullptr, &oled, &player,
      &audioOutput); // passing all the objects as pointers to the main window
  w.show();
  return a.exec();
}
