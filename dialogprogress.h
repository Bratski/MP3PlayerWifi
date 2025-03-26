#ifndef DIALOGPROGRESS_H
#define DIALOGPROGRESS_H

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
                          bool* cancelsaving = nullptr);
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
  bool* _cancelSaving; // for proper aborting
  bool _allowclose;    // to be able to block all the functions while saving is
                       // running
};

#endif // DIALOGPROGRESS_H
