#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// adding the header files
#include "COled.h"
#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include "crotaryencoderworker.h"
#include "ctrack.h"

// general Qt libraries
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
             QThread *dbthread = nullptr, CDatabaseWorker *workerdb = nullptr,
             QThread *rtcthread = nullptr,
             CRotaryEncoderWorker *workerrtc = nullptr);
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
  void sortByDatabase();
  void sortByGenre();
  void sortRandom();
  void undoSort();

  // player functions
  void setVolume(int level);
  void playAllSongs();
  void playNext();
  void playPrevious();
  void togglePause();
  void stopPlaying();
  void playOneSong(QTableWidgetItem *item);
  void setRepeat(bool state) { _repeat = state; }
  void setShuffle(bool state);

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
  QThread *_rtcthread;
  CDatabaseWorker *_workerdb;
  CRotaryEncoderWorker *_workerrtc;

  QNetworkAccessManager *_network;

  // attributes
  QString _playThisSong; // contains the filelocation of the music file
  QString _timeSong = "00:00";
  QString _timePrevious =
      "00:00"; // needed for reduced flickering of the OLED Display
  QString _timeList = "Total Time"; // sum of the song durations in the playlist
  QString _apiKey = "9d6171634a3f43ff46083c4534ed44db"; // default API key
  QString _imageSize =
      "large"; // Options are: "small", "medium", "large", "extralarge", "mega"

  bool _repeat = false;
  bool _shuffle = false;
  bool _playall = false;
  bool _playlistChanged = false;
  bool _imagedata = false;
  bool _cancelSaving = false;
  bool _playerStopped = false;
  bool _statusOled = false;
  bool _statusRTC = true;
  bool _filelocationValid = true;

  float _startVolume = 0.2; // setting the start volume to 20%

  int _index = 0; // song index for the playlist vector
  int _defaultPlaylistID =
      1; // at startup open the first playlist in the database by default
  int _trackID = 0; // to be able to give manually added tracks a unique id
  uint _pinSW = 23;
  uint _pinCLK = 17;
  uint _pinDT = 27;

  std::vector<QString>
      _detectedMusicFiles; // vector with all detected file paths for the
                           // add-music-folder functionality
  std::vector<int>
      _shuffleAlreadyPlayed; // vector with tracks indexes, as a reference which
                             // songs already have been played, and for the
                             // check if all the songs have been played once
  QSettings _settings{"Bratski", "MP3Player"};

  // methods
  const QString convertMilliSecToTimeString(const qint64 &millisec);
  const QString convertSecToTimeString(const int &sec);
  void
  handleMediaStatusChanged(QMediaPlayer::MediaStatus
                               status); // detect if a song has finished playing
  void refreshTableWidgetCurrentPlaylist(); // populate the table widget
  void updateTrackInfoDisplay(); // sends the info to the mainwindow info field
                                 // and oled display
  void readDataBasePlaylist();   // reads all the tracks from the database of a
                                 // particular playlist
  void closingProcedure(); // contains several procedures for a secure shutdown
  void processFolder(
      const QString &path); // recursive function to scan all the directories
                            // and subdirectories for music files
  void setItemBackgroundColour(); // to colour the background of the current
                                  // playing song
  bool playTrack();               // procedure to start playing a track
  void initializeSettings(); // create a default configuration file, if it is
                             // non existing
  void saveSettings();       // saving the configuration settings
  void loadSettings();       // loading the configuration settings
  int randomNumberGenerator(const int &min,
                            const int &max); // for the shuffle modus
};
#endif // MAINWINDOW_H
