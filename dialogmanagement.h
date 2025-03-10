#ifndef DIALOGMANAGEMENT_H
#define DIALOGMANAGEMENT_H

#include "cplaylistcontainer.h"
#include <QDialog>
#include <QMessageBox>
#include <QDebug>

// for the SQL Database management
#include <QtSql/QSqlQuery>

// for the Table Widget handling
#include <QList>

namespace Ui {
class DialogManagement;
}

class DialogManagement : public QDialog {
  Q_OBJECT

public:
  explicit DialogManagement(QWidget *parent = nullptr,
                            CPlaylistContainer *playlist = nullptr);
  ~DialogManagement();

public slots:
  void openPlaylist();

private:
  Ui::DialogManagement *ui;
  CPlaylistContainer *_playlist;
  void readDatabase();
};

#endif // DIALOGMANAGEMENT_H
