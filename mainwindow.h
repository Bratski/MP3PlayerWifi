#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "COled.h"
#include "cplaylistcontainer.h"
#include "ctrack.h"
#include <QCoreApplication>
#include <QMainWindow>
#include <QSettings>

// for the SQL Database management
#include <QtSql/QSqlQuery>

// adding the dialog windows
#include <dialogaddplaylist.h>
#include <dialogmanagement.h>
#include <dialogprogress.h>
#include <dialogsearch.h>
#include <dialogsettings.h>

// to be able to play music files
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

// to make it possible to extract artwork from the mp3-file
#include <taglib/id3v2frame.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, COled *oled = nullptr,
             QMediaPlayer *player = nullptr, QAudioOutput *audio = nullptr,
             CPlaylistContainer *playlist = nullptr, CTrack *track = nullptr);
  ~MainWindow();

public slots:
  // header
  void openSettingsDialog();
  void openProgressDialog();
  void openSearchDialog();
  void openManagementDialog();
  void openAddPlaylistDialog();

  // player
  void setVolume(int level);
  void playSong();
  void playNext();
  void playPrevious();

  // table widget
  void refreshTableWidgetCurrentPlaylist();

private:
  Ui::MainWindow *ui;
  COled *_oled;
  QMediaPlayer *_player;
  QAudioOutput *_audio;
  DialogSettings *_dlgSettings;
  DialogProgress *_dlgProgess;
  DialogSearch *_dlgSearch;
  DialogManagement *_dlgManagement;
  DialogAddPlaylist *_dlgAddPlaylist;
  CPlaylistContainer *_playlist;
  CTrack *_track;


  int index;
  QString playThisSong; // = "/home/bart/Nextcloud/Music/Calexico/Aerocalexico/01 "
                        // "All The Pretty Horses.mp3"; // for debugging

  QString timeSong = "Time Song";
  QString timeList = "Total Time";
  float startVolume = 0.2; // setting the start volume to 20%
  const QString convertMilliSecToTimeString(const qint64 &millisec);
  const QString convertSecToTimeString(const int &sec);
};
#endif // MAINWINDOW_H
