#ifndef DIALOGPROGRESS_H
#define DIALOGPROGRESS_H

#include "cplaylistcontainer.h"
#include <QDialog>

namespace Ui {
class DialogProgress;
}

class DialogProgress : public QDialog {
  Q_OBJECT

public:
  explicit DialogProgress(QWidget *parent = nullptr,
                          CPlaylistContainer *playlist = nullptr);
  ~DialogProgress();

public slots:
  void receiveProgress(const int &progress);

private:
  Ui::DialogProgress *ui;
  CPlaylistContainer *_playlist;
};

#endif // DIALOGPROGRESS_H
