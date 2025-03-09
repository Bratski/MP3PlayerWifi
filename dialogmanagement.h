#ifndef DIALOGMANAGEMENT_H
#define DIALOGMANAGEMENT_H

#include "cplaylistcontainer.h"
#include <QDialog>

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

private:
  Ui::DialogManagement *ui;
  CPlaylistContainer *_playlist;
};

#endif // DIALOGMANAGEMENT_H
