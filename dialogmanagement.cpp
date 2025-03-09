#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent,
                                   CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogManagement), _playlist(playlist) {
  ui->setupUi(this);
  setWindowTitle("Playlist Management");
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogManagement::close);
}

DialogManagement::~DialogManagement() { delete ui; }
