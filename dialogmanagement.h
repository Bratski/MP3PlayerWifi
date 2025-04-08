#ifndef DIALOGMANAGEMENT_H
#define DIALOGMANAGEMENT_H

#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QThread>

// for import and export XML functionalities
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// for the SQL Database management
#include <QtSql/QSqlQuery>

// for the Table Widget handling
#include <QList>
#include <QTableWidget>
#include <QTableWidgetItem>

// to deal with single and double click mouse events
#include <QTimer>

namespace Ui {
class DialogManagement;
}

class DialogManagement : public QDialog {
  Q_OBJECT

public:
  explicit DialogManagement(QWidget* parent = nullptr,
                            CPlaylistContainer* playlist = nullptr,
                            CDatabaseWorker* workerdb = nullptr,
                            bool* playlistChanged = nullptr);
  ~DialogManagement();

public slots:
  void openPlaylist();
  void addNewPlaylist();
  void deletePlaylist();
  void namePlaylistEdited(QTableWidgetItem* item);
  void dealWithMouseOneTimeClick(QTableWidgetItem* item);
  void importXML();
  void exportXML();

signals:
  void saveToDBMainWindow(CPlaylistContainer* playlist);

private:
  Ui::DialogManagement* ui;
  CPlaylistContainer* _playlist;
  CDatabaseWorker* _workerdb;

  QAction _action;

  int _lastrow;
  int _activeplaylistid;
  bool* _playlistChanged;  // to trigger the save to db question at shutdown
  bool _isEditing = false; // to prevent multiple triggeringg
  void readDatabase();
  bool _firstckick = true;
  // std::vector<CPlaylistContainer*> _importedplaylists;
  // to colour the background of the current playlist
  void setItemBackgroundColour();
};

#endif // DIALOGMANAGEMENT_H
