#include "dialogprogress.h"
#include "ui_dialogprogress.h"

DialogProgress::DialogProgress(QWidget *parent, CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogProgress), _playlist(playlist) {
  ui->setupUi(this);
  setWindowTitle("Saving Music Files to Database...");
  // initializing the range of the progressbar (0 to 100%)
  ui->progressBar->setRange(0, 100);
}

DialogProgress::~DialogProgress() { delete ui; }

void DialogProgress::receiveProgress(const int &progress) {
  // quick calculation from song number to percentage
  int numberOfTracks = _playlist->getNumberOfMainwindowTracks();
  int percentage = 100;
  // caution number of tracks are not allowed to be 0! No division by 0 is
  // possible!
  if (numberOfTracks != 0)
    percentage = ((progress * 100) / numberOfTracks);
  // setting the progress bar to the new position
  ui->progressBar->setValue(percentage);
}
