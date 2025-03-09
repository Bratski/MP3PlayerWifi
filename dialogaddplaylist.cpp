#include "dialogaddplaylist.h"
#include "ui_dialogaddplaylist.h"

DialogAddPlaylist::DialogAddPlaylist(QWidget *parent,
                                     CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogAddPlaylist), _playlist(playlist) {
  ui->setupUi(this);
  setWindowTitle("Add Playlist");
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogAddPlaylist::close);
}

DialogAddPlaylist::~DialogAddPlaylist() { delete ui; }
