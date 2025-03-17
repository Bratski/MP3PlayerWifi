#include "COled.h"
#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include "ctrack.h"
#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // creating Objects:
  COled oled;               // for Oled Display
  QMediaPlayer player;      // for playing audiofiles
  QAudioOutput audioOutput; // needed for setting the audio output
  CPlaylistContainer playlist;
  CTrack track;
  QThread dbthread;       // the thread for database operations
  CDatabaseWorker worker; // object containing all possible database operations

  worker.moveToThread(
      &dbthread); // thread and possible operations are connected

  dbthread.start(); // the database thread is started

  // Set the players audio output:
  player.setAudioOutput(&audioOutput);

  // Create Database,  wait until the success-bool has turned to true
  bool success = false;
  QMetaObject::invokeMethod(&worker, "initialize", Qt::BlockingQueuedConnection,
                            &success);

  // stop program if not succesfull
  if (!success) {
    return EXIT_FAILURE;
  }

  MainWindow w(
      nullptr, &oled, &player, &audioOutput, &playlist, &track, &dbthread,
      &worker); // passing all the objects as pointers to the main window

  w.show();

  return a.exec();
}
