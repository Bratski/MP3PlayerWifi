#include "COled.h"
#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include "crotaryencoderworker.h"
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

  QThread rtcthread; // the thread for the rtc coder
  QThread dbthread;  // the thread for database operations
  CDatabaseWorker
      workerdb; // object containing all possible database operations
  CRotaryEncoderWorker workerrtc; // object containing rotary encoder operations

  workerrtc.moveToThread(&rtcthread); // thread for rtc, not started yet, only
                                      // if it is been activated
  workerdb.moveToThread(
      &dbthread); // thread and possible operations are connected

  dbthread.start(); // the database thread is started

  // Set the players audio output:
  player.setAudioOutput(&audioOutput);

  // Create Database,  wait until the success-bool has turned to true
  bool success = false;
  QMetaObject::invokeMethod(&workerdb, "initialize",
                            Qt::BlockingQueuedConnection, &success);

  // stop program if not succesfull
  if (!success) {
    return EXIT_FAILURE;
  }

  MainWindow w(
      nullptr, &oled, &player, &audioOutput, &playlist, &track, &dbthread,
      &rtcthread, &workerdb,
      &workerrtc); // passing all the objects as pointers to the main window

  w.show();

  return a.exec();
}
