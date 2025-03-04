#include "dialogaddplaylist.h"
#include "ui_dialogaddplaylist.h"

DialogAddPlaylist::DialogAddPlaylist(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogAddPlaylist) {
  ui->setupUi(this);
  setWindowTitle("Add Playlist");
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogAddPlaylist::close);
}

DialogAddPlaylist::~DialogAddPlaylist() { delete ui; }
