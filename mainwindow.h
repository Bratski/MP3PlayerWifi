#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "COled.h"
#include <QCoreApplication>
#include <QMainWindow>
#include <QSettings>
//#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <dialogaddplaylist.h>
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
  void openSearchDialog();
  void openManagementDialog();
  void openAddPlaylistDialog();

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
  DialogAddPlaylist *_dlgAddPlaylist;
  // QSqlQueryModel *_sqlq; // not able to edit a database, it's read only
  // QSqlTableModel *_sqlq;

  // QString fill_SqlqWith =
  //     "SELECT Track.TraID, Track.TraName, Track.TraNumber, Track.TraDuration,
  //     " "Track.TraBitrate, " "Track.TraSamplerate, Track.TraChannels,
  //     Track.TraFileLocation, " "Album.AlbName, Album.AlbYear, Artist.ArtName
  //     " "FROM Track " "JOIN Album ON Track.TraAlbFK = Album.AlbID " "JOIN
  //     Artist ON Album.AlbArtFK = Artist.ArtID " "JOIN TrackPlaylist ON
  //     Track.TraID = TrackPlaylist.TraFK " "JOIN Playlist ON
  //     TrackPlaylist.PllFK = Playlist.PllID " "WHERE Playlist.PllName =
  //     :playlistName";
  QString playThisSong =
      "/home/bart/Music/Fridge/Happiness (Anniversary Edition)/07 - Drums Bass "
      "Sonics & Edits - Remastered.flac";
  QString defaultPlaylistName = "Americana";
  // QString playThisSong = "/home/bart/Music/Bart/calexico/mix/calexico - Dutch
  // "
  //                        "TV-show.mp3"; // just for testing
  QString timeSong = "Time Song";
  QString timeList = "Total Time";
  float startVolume = 0.2; // setting the start volume to 20%
  const QString convertMilliSecToTimeString(const qint64 &millisec);
  const QString convertSecToTimeString(const qint64 &sec);
  void fillTableWithDatabase(const QString &playlistName);
  int getPlaylistID(const QString &playlistName);
};
#endif // MAINWINDOW_H
