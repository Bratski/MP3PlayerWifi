#ifndef CDATABASEWORKER_H
#define CDATABASEWORKER_H

#include "cplaylistcontainer.h"
#include "dbconnect.h"

#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class CDatabaseWorker : public QObject {
  Q_OBJECT
public:
  explicit CDatabaseWorker(QObject *parent = nullptr);
public slots:
  void initialize(bool *success);
  void readDataBasePlaylist(CPlaylistContainer *playlist, int defaultPlaylistID,
                            bool *success);
  void writePlaylistTracksToDatabase(CPlaylistContainer *playlist,
                                     bool *success);
  void readPlaylistTracksFromDatabase(CPlaylistContainer *playlist,
                                      bool *success);
  void addNewPlaylist(const QString &name, bool *success);
  void getPlaylistsFromDatabase(std::vector<QString> *list, bool *success);
  void deletePlaylist(const QString &name, bool *success);
  void updatePlaylistInDatabase(const QString &name, const int &id,
                                bool *success);
  void cleanupDatabase(bool *success);
  void closeDatabase();

signals: // for progress bar functionality
  void sendProgress(const int &progress);
  void progressReady();

private:
  int tracknr;
};

#endif // CDATABASEWORKER_H
