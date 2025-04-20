#ifndef DIALOGADDPLAYLIST_H
#define DIALOGADDPLAYLIST_H

// this should disable or enable qdebug output
#define QT_NO_DEBUG_OUTPUT
#include <QDebug>

#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include <QDialog>
#include <QMessageBox>

namespace Ui {
class DialogAddPlaylist;
}

class DialogAddPlaylist : public QDialog {
  Q_OBJECT

public:
  explicit DialogAddPlaylist(QWidget* parent = nullptr,
                             CPlaylistContainer* playlist = nullptr,
                             CDatabaseWorker* workerdb = nullptr,
                             bool* playlistChanged = nullptr);
  ~DialogAddPlaylist();

public slots:
  void addPlaylist();

private:
  Ui::DialogAddPlaylist* ui;
  CPlaylistContainer* _playlist;
  CDatabaseWorker* _workerdb;
  bool* _playlistChanged;
  void readDatabase();
};

#endif // DIALOGADDPLAYLIST_H
