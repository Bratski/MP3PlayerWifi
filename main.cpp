
// this should disable or enable qdebug output
#define QT_NO_DEBUG_OUTPUT
#include <QDebug>
// for qt multimedia to turn off its qdebug messages
#include <QLoggingCategory>

#include "COled.h"
#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include "crotaryencoderworker.h"
#include "ctrack.h"
#include "mainwindow.h"

#include <QApplication>
#include <QIcon>
#include <QMetaType>
#include <QThread>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

int main(int argc, char* argv[]) {
// for pi compilation Qt 6.4.2 compatibility
#if (QT_VERSION <= QT_VERSION_CHECK(6, 4, 2))
  qRegisterMetaType<bool*>("bool*");
  qRegisterMetaType<uint*>("uint*");
  qRegisterMetaType<int*>("int*");
#endif

  // Disable all Qt Multimedia debug messages
  QLoggingCategory::setFilterRules("qt.multimedia.*=false");

  QApplication a(argc, argv);

  // creating Objects:
  COled oled; // for Oled Display

  QMediaPlayer player;      // for playing audiofiles
  QAudioOutput audioOutput; // needed for setting the audio output
  CPlaylistContainer playlist;
  CTrack track;

  QThread rtcthread;
  QThread dbthread; // the thread for database operations
  CDatabaseWorker
      workerdb; // object containing all possible database operations
  CRotaryEncoderWorker workerrtc; // for the rtc rotary encoder

  workerdb.moveToThread(
      &dbthread);   // thread and possible operations are connected
  dbthread.start(); // the database thread is started

  workerrtc.moveToThread(&rtcthread); // thread for rotary encoder functionality
  rtcthread.start();                  // thread is started

  // Set the players audio output:
  player.setAudioOutput(&audioOutput);

  // Create Database,  wait until the success-bool has turned to true
  bool success = false;
  QMetaObject::invokeMethod(&workerdb, "initialize",
                            Qt::BlockingQueuedConnection,
                            Q_ARG(bool*, &success));

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
