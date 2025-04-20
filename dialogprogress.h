#ifndef DIALOGPROGRESS_H
#define DIALOGPROGRESS_H

// this should disable or enable qdebug output
#define QT_NO_DEBUG_OUTPUT
#include <QDebug>

#include "cdatabaseworker.h"
#include "cplaylistcontainer.h"
#include <QCloseEvent>
#include <QDialog>
#include <QMessageBox>
#include <QThread>

namespace Ui {
class DialogProgress;
}

class DialogProgress : public QDialog {
  Q_OBJECT

public:
  explicit DialogProgress(QWidget* parent = nullptr,
                          CPlaylistContainer* playlist = nullptr,
                          QThread* dbthread = nullptr,
                          CDatabaseWorker* workerdb = nullptr);
  ~DialogProgress();

public slots:
  void receiveProgress(const int& progress);
  // needed to prevent the progressbar from closing until the saving process has
  // been completed, if the saving process has completed allowClose is run
  void allowClose();
  void cancelSaving();

protected:
  // overriding the close function, not to close as long as the saving process
  // is running
  void closeEvent(QCloseEvent* event) override {
    if (!_allowclose && _dbthread->isRunning())
      event->ignore();
    else
      event->accept();
  }

private:
  Ui::DialogProgress* ui;
  CPlaylistContainer* _playlist;
  QThread* _dbthread;
  CDatabaseWorker* _workerdb;
  bool _cancelSaving; // for proper aborting
  bool _allowclose;   // to be able to block all the functions while saving is
                      // running
};

#endif // DIALOGPROGRESS_H
