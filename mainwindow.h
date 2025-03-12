#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "COled.h"
#include "cplaylistcontainer.h"
#include "ctrack.h"
#include <QCoreApplication>
#include <QFileDialog>
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
  // header functions
  void openSettingsDialog();
  void openProgressDialog();
  void openSearchDialog();
  void openManagementDialog();
  void openAddPlaylistDialog();
  void addMusicFile();
  void saveToDatabase();

  // header sort methods
  void sortByAlbum();
  void sortByYear();
  void sortByArtist();
  void sortByGenre();
  void undoSort();

  // player functions
  void setVolume(int level);
  void playSongs();
  void playNext();
  void playPrevious();
  void togglePause();
  void playOneSong(QTableWidgetItem *item);
  void setRepeat(bool state) { _repeat = state; }
  void setRandom(bool state);

private:
  // pointers
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

  // attributes
  int _index;
  QString _playThisSong;
  QString _timeSong = "Time Song";
  QString _timeList = "Total Time";
  float _startVolume = 0.2; // setting the start volume to 20%
  int _defaultPlaylistID =
      1; // first entry in the playlist table in the sql database
  bool _repeat = false;
  bool _playall = false;

  // methods
  const QString convertMilliSecToTimeString(const qint64 &millisec);
  const QString convertSecToTimeString(const int &sec);
  // detect if a song has finished playing
  void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);
  // populate the table widget
  void refreshTableWidgetCurrentPlaylist();
  void updateTrackInfoDisplay();
  void readDataBasePlaylist();
};
#endif // MAINWINDOW_H
