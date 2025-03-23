#ifndef DIALOGMANAGEMENT_H
#define DIALOGMANAGEMENT_H

#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include "dialogprogress.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QThread>

// for the SQL Database management
#include <QtSql/QSqlQuery>

// for the Table Widget handling
#include <QList>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
class DialogManagement;
}

class DialogManagement : public QDialog {
  Q_OBJECT

public:
  explicit DialogManagement(QWidget *parent = nullptr,
                            CPlaylistContainer *playlist = nullptr,
                            QThread *dbthread = nullptr,
                            CDatabaseWorker *worker = nullptr,
                            bool *playlistChanged = nullptr);
  ~DialogManagement();

public slots:
  void openPlaylist();
  void addNewPlaylist();
  void deletePlaylist();
  void namePlaylistEdited(QTableWidgetItem *item);

private:
  Ui::DialogManagement *ui;
  CPlaylistContainer *_playlist;
  QThread *_dbthread;
  CDatabaseWorker *_worker;
  DialogProgress *_dlgProgess;
  bool *_playlistChanged;  // to trigger the save to db question at shutdown
  bool _isEditing = false; // to prevent multiple triggeringg
  bool _cancelSaving = false;
  void readDatabase();
  void saveToDatabase();
};

#endif // DIALOGMANAGEMENT_H
