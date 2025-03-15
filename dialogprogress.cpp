#include "dialogprogress.h"
#include "ui_dialogprogress.h"

DialogProgress::DialogProgress(QWidget *parent, CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogProgress), _playlist(playlist) {
  ui->setupUi(this);
  setWindowTitle("Saving Music Files to Database...");

  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogProgress::close);
}

DialogProgress::~DialogProgress() { delete ui; }

void DialogProgress::receiveProgress(const int &progress) {
  ui->labelProgress->setText(QString::number(progress));
}
