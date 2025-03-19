#ifndef DIALOGPROGRESS_H
#define DIALOGPROGRESS_H

#include "cplaylistcontainer.h"
#include <QCloseEvent>
#include <QDialog>
#include <QThread>

namespace Ui {
class DialogProgress;
}

class DialogProgress : public QDialog {
  Q_OBJECT

public:
  explicit DialogProgress(QWidget *parent = nullptr,
                          CPlaylistContainer *playlist = nullptr,
                          QThread *dbthread = nullptr);
  ~DialogProgress();

public slots:
  void receiveProgress(const int &progress);
  // needed to prevent the progressbar from closing until the saving process has
  // been completed, if the saving process has completed allowClose is run
  void allowClose();


protected:
  // overriding the close function, not to close as long as the saving process is running
  void closeEvent(QCloseEvent *event) override {
    if (!_allowclose && _dbthread->isRunning())
      event->ignore();
    else
      event->accept();
  }

private:
  Ui::DialogProgress *ui;
  CPlaylistContainer *_playlist;
  QThread *_dbthread;
  bool _allowclose;
};

#endif // DIALOGPROGRESS_H
