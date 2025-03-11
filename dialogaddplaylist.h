#ifndef DIALOGADDPLAYLIST_H
#define DIALOGADDPLAYLIST_H

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
                             CPlaylistContainer *playlist = nullptr);
  ~DialogAddPlaylist();

public slots:
  void addPlaylist();

private:
  Ui::DialogAddPlaylist *ui;
  CPlaylistContainer *_playlist;
  void readDatabase();
};

#endif // DIALOGADDPLAYLIST_H
