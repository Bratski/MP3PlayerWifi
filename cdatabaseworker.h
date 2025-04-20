#ifndef CDATABASEWORKER_H
#define CDATABASEWORKER_H

// this should disable or enable qdebug output
#define QT_NO_DEBUG_OUTPUT

// adding the libraries
#include "cplaylistcontainer.h"
#include "dbconnect.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class CDatabaseWorker : public QObject {
  Q_OBJECT
public:
  explicit CDatabaseWorker(QObject* parent = nullptr);
  const QString& getDefaultPllName() { return _defaultPlaylistName; }
  const int& getDefaultPllID() { return _defaultPlaylistID; }

public slots:
  void readDataBasePlaylist(CPlaylistContainer* playlist, bool* success);
  void
  initialize(bool* success); // checks if a database exists and is correctly
                             // configured, if not it will create a default one
  void writePlaylistTracksToDatabase(
      CPlaylistContainer* playlist, // saves all the tracks from a playlist
      bool* success);
  void readPlaylistTracksFromDatabase(
      CPlaylistContainer* playlist, // reads all the tracks from a playlist
      bool* success);
  void addNewPlaylist(const QString& name, bool* success,
                      bool* doubleName); // adds a new playlist to the database
  void getPlaylistsFromDatabase(
      std::vector<QString>* list,
      bool* success); // reads all the playlists from the database
  void deletePlaylist(const QString& name,
                      bool* success); // deletes a playlist from the database
  void updatePlaylistInDatabase(
      const QString& name,
      const int& id, // after the playlist name has been edited, saves the
                     // edited name, and checks if the name is already in use
      bool* success, bool* doubleName);
  void
  cleanupDatabase(bool* success); // checks if there are orphaned tracks, albums
                                  // and artist, if so they are deleted
  void closeDatabase();           // closes the database
  void cancelSaving() {
    QMutexLocker locker(&_mutex);
    _cancelSaving = true;
  }
  void setPllIDbasedOnName(CPlaylistContainer* playlist, bool* success);

signals:                                  // for progress bar functionality
  void sendProgress(const int& progress); // sends the amount of tracks being
                                          // saved for the progress bar
  void progressReady(); // sends a signal to let the progress bar know the
                        // process is finished or properly cancelled
  void error(); // prevents the progress bar from stalling in case a database operation was unsuccessful

private:
  int _tracknr; // to be able to count the number of tracks being saved, for
                // progress bar functionality
  QMutex _mutex;
  bool _cancelSaving = false;
  // default Playlist Settings
  const QString _defaultPlaylistName = "default Playlist";
  const int _defaultPlaylistID = 1;
};

#endif // CDATABASEWORKER_H
