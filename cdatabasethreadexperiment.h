#ifndef CDATABASETHREAD_H
#define CDATABASETHREAD_H

#include "cplaylistcontainer.h"
#include "dbconnect.h"
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>

class CDatabaseThread : public QThread {
  Q_OBJECT
public:
  // explicit CDatabaseThread(QObject *parent = nullptr) : QThread(parent) {}

  CDatabaseThread() = default;
  void run();

public slots:
  bool initialize();
  bool readDataBasePlaylist(CPlaylistContainer *playlist,
                            int &defaultPlaylistID);
  bool writePlaylistTracksToDatabase(CPlaylistContainer *playlist);
  bool readPlaylistTracksFromDatabase(CPlaylistContainer *playlist);
  bool addNewPlaylist(const QString &name);
  const std::vector<QString> &getPlaylistsFromDatabase();
  bool deletePlaylist(const QString &name);
  bool updatePlaylistInDatabase(const QString &name, const int &id);
  bool cleanupDatabase();
  void closeDatabase();

signals: // for progress bar functionality
  void sendProgress(const int &progress);
  void ProgressReady();

private:
  int tracknr;
  std::vector<QString> _playlistsInDatabase;
};

#endif // CDATABASETHREAD_H
