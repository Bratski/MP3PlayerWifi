#ifndef DIALOGADDPLAYLIST_H
#define DIALOGADDPLAYLIST_H

#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include <QDialog>
#include <QMessageBox>

// for the SQL Database management
#include <QtSql/QSqlQuery>

namespace Ui {
class DialogAddPlaylist;
}

class DialogAddPlaylist : public QDialog {
  Q_OBJECT

public:
  explicit DialogAddPlaylist(QWidget *parent = nullptr,
                             CPlaylistContainer *playlist = nullptr,
                             CDatabaseWorker *workerdb = nullptr,
                             bool *playlistChanged = nullptr);
  ~DialogAddPlaylist();

public slots:
  void addPlaylist();

private:
  Ui::DialogAddPlaylist *ui;
  CPlaylistContainer *_playlist;
  CDatabaseWorker *_workerdb;
  bool *_playlistChanged;
  void readDatabase();
};

#endif // DIALOGADDPLAYLIST_H
