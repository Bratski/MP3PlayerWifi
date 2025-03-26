#include "dialogprogress.h"
#include "ui_dialogprogress.h"

DialogProgress::DialogProgress(QWidget* parent, CPlaylistContainer* playlist,
                               QThread* dbthread, bool* cancelsaving)
    : QDialog(parent), ui(new Ui::DialogProgress), _playlist(playlist),
      _dbthread(dbthread), _cancelSaving(cancelsaving), _allowclose(false) {
  ui->setupUi(this);

  setWindowTitle("Saving Music Files to Database...");
  // initializing the range of the progressbar (0 to 100%)
  ui->progressBar->setRange(0, 100);

  // connect the button events to a function
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogProgress::cancelSaving);
}

DialogProgress::~DialogProgress() { delete ui; }

void DialogProgress::receiveProgress(const int& progress) {
  // calculation from song number to percentage
  int numberOfTracks = _playlist->getNumberOfMainwindowTracks();
  int percentage = 100;
  // caution! number of tracks are not allowed to be 0! No division by 0 is
  // possible!
  if (numberOfTracks != 0)
    percentage = ((progress * 100) / numberOfTracks);
  // setting the progress bar to the new position
  ui->progressBar->setValue(percentage);
}

// to circumvent the closeEvent override
void DialogProgress::allowClose() {
  _allowclose = true;
  this->close();
}

void DialogProgress::cancelSaving() {
  *_cancelSaving = true; // for proper abortion of the saving process in the
                         // database worker-thread
}
