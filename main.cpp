#include "COled.h"
#include "mainwindow.h"

#include <QApplication>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

int main(int argc, char *argv[]) {

  COled oled;
  QMediaPlayer player;
  QAudioOutput audioOutput;
  player.setAudioOutput(&audioOutput);

  // player.setSource(QUrl::fromLocalFile(
  //     "/home/bart/Music/Bart/Broadcast/broadcast - austin powers.mp3"));
  // audioOutput.setVolume(50);
  // player.play();

  // oled.autodetect();

  QApplication a(argc, argv);
  MainWindow w(nullptr, &oled, &player, &audioOutput);
  w.show();
  return a.exec();
}
