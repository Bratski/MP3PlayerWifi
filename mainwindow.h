#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "COled.h"
#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include "ctrack.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>
#include <QThread>
#include <QTimer>

// for the SQL Database management
#include <QSqlError>
#include <QtSql/QSqlQuery>

// adding the dialog windows
#include <dialogaddplaylist.h>
#include <dialogmanagement.h>
#include <dialogprogress.h>
#include <dialogsearch.h>
#include <dialogsettings.h>

// to be able to play music files
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaFormat> // to check which audio formats are supported
#include <QtMultimedia/QMediaPlayer>

// to get the artwork from an internet API
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

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
             CPlaylistContainer *playlist = nullptr, CTrack *track = nullptr,
             QThread *dbthread = nullptr, CDatabaseWorker *worker = nullptr);
  ~MainWindow();

public slots:
  // header functions
  void openSettingsDialog();
  void openSearchDialog();
  void openManagementDialog();
  void openAddPlaylistDialog();
  void addMusicFile();
  void addMusicFolder();
  void saveToDatabase();
  void deleteTrack();
  void deletePlaylist();

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
  void stopPlaying();
  void playOneSong(QTableWidgetItem *item);
  void setRepeat(bool state) { _repeat = state; }
  void setRandom(bool state);

  // network functions
  void getDataFromNetwork(QNetworkReply *reply);

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
  QThread *_dbthread;
  CDatabaseWorker *_worker;
  QNetworkAccessManager *_network;

  // attributes
  int _index = 0;
  QString _playThisSong;
  QString _timeSong = "Time Song";
  QString _timeList = "Total Time";
  float _startVolume = 0.2; // setting the start volume to 20%
  bool _repeat = false;
  bool _playall = false;
  bool _playlistChanged = false;
  bool _imagedata = false;
  bool _cancelSaving = false;
  bool _playerStopped = false;
  QString _imageSize =
      "large"; // Options are: "small", "medium", "large", "extralarge", "mega"
  int _defaultPlaylistID =
      1; // at startup open the first playlist in the database by default
  std::vector<QString>
      _detectedMusicFiles; // vector with all file paths for the add music
                           // folder functionality
  int _trackID = 0; // to be able to give manually added tracks a unique id

  // methods
  const QString convertMilliSecToTimeString(const qint64 &millisec);
  const QString convertSecToTimeString(const int &sec);
  // detect if a song has finished playing
  void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);
  // populate the table widget
  void refreshTableWidgetCurrentPlaylist();
  void updateTrackInfoDisplay();
  void readDataBasePlaylist();
  void closingProcedure();
  void processFolder(const QString &path);
};
#endif // MAINWINDOW_H
