#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "COled.h"
#include <QMainWindow>
#include <QSettings>
#include <dialogmanagement.h>
#include <dialogprogress.h>
#include <dialogsearch.h>
#include <dialogsettings.h>

#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, COled *oled = nullptr,
             QMediaPlayer *player = nullptr, QAudioOutput *audio = nullptr);
  ~MainWindow();

public slots:
  // header
  void openSettingsDialog();
  void openProgressDialog();
  void openSearch();
  void openManagement();

  // player
  void setVolume(int level);

private:
  Ui::MainWindow *ui;
  COled *_oled;
  QMediaPlayer *_player;
  QAudioOutput *_audio;
  DialogSettings *_dlgSettings;
  DialogProgress *_dlgProgess;
  DialogSearch *_dlgSearch;
  DialogManagement *_dlgManagement;
  QString playThisSong =
      "/home/bart/Music/Bart/calexico/mix/calexico - Dutch TV-show.mp3";
  QString timeSong = "Time Song";
  QString timeList = "Total Time";
  float startVolume = 0.2;
  const QString convertMilliSec(const qint64 &millisec);
};
#endif // MAINWINDOW_H
