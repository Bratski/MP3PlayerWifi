#ifndef DIALOGADDPLAYLIST_H
#define DIALOGADDPLAYLIST_H

#include "cplaylistcontainer.h"
#include <QDialog>

namespace Ui {
class DialogAddPlaylist;
}

class DialogAddPlaylist : public QDialog {
  Q_OBJECT

public:
  explicit DialogAddPlaylist(QWidget *parent = nullptr,
                             CPlaylistContainer *playlist = nullptr);
  ~DialogAddPlaylist();

private:
  Ui::DialogAddPlaylist *ui;
  CPlaylistContainer *_playlist;
};

#endif // DIALOGADDPLAYLIST_H
