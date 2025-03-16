#include "COled.h"
#include "cplaylistcontainer.h"
#include "ctrack.h"
#include "dbconnect.h"
#include "mainwindow.h"
//#include "threadsavedatabase.h"

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
  CPlaylistContainer playlist;
  CTrack track;
  // CThreadsavedatabase savedbthread(&playlist);

  // Set the players audio output:
  player.setAudioOutput(&audioOutput);

  // Create Database, stop program if not succesfull
  if (!createConnection()) {
    return EXIT_FAILURE;
  }

  MainWindow w(
      nullptr, &oled, &player, &audioOutput, &playlist,
      &track); // passing all the objects as pointers to the main window

  w.show();

  return a.exec();
}
