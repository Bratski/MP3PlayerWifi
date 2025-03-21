#include "dialogaddplaylist.h"
#include "ui_dialogaddplaylist.h"

DialogAddPlaylist::DialogAddPlaylist(QWidget *parent,
                                     CPlaylistContainer *playlist,
                                     CDatabaseWorker *worker,
                                     bool *playlistChanged)
    : QDialog(parent), ui(new Ui::DialogAddPlaylist), _playlist(playlist),
      _worker(worker), _playlistChanged(playlistChanged) {
  ui->setupUi(this);

  // initialize the window
  setWindowTitle("Add Playlist");
  readDatabase();

  // connecting the pushbutton events to functions
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogAddPlaylist::close);
  QObject::connect(ui->pushButtonAdd, &QPushButton::clicked, this,
                   &DialogAddPlaylist::addPlaylist);
}

DialogAddPlaylist::~DialogAddPlaylist() { delete ui; }

void DialogAddPlaylist::addPlaylist() {
  // get the playlist id from the selected table widget item
  int id =
      ui->tableWidgetDatabase->item(ui->tableWidgetDatabase->currentRow(), 0)
          ->text()
          .toInt();

  // qDebug() << " id: " << id;

  // create a temporary playlist object
  CPlaylistContainer tempplaylist;

  // fill that object with the selected playlistname from the tablewidget, only
  // 1!
  bool success = false;
  QMetaObject::invokeMethod(_worker, "readDataBasePlaylist",
                            Qt::BlockingQueuedConnection, &tempplaylist, id,
                            &success);

  bool doubleTrack = false;
  // add the temporary playlist object to the global one: _playlist
  if (success)
    for (auto it1 = tempplaylist.begin(); it1 != tempplaylist.end(); ++it1) {
      // check if tracks are already available in the playlist, to avoid
      // duplicates
      doubleTrack = false;
      for (auto it2 = _playlist->begin(); it2 != _playlist->end(); ++it2) {
        if (it2->getID() == it1->getID()) {
          doubleTrack = true;
        }
      }
      if (!doubleTrack) {
        _playlist->addTrack(*it1);
        *_playlistChanged = true;
      }
    }

  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogAddPlaylist::readDatabase() {

  // no editing in this window, so this vector can be temporarely
  std::vector<QString> playlistsInDatabase;

  // empty the list with playlists
  ui->tableWidgetDatabase->clearContents();
  ui->tableWidgetDatabase->setRowCount(0);
  playlistsInDatabase.clear();
  // qDebug() << "playlistsInDatabase vector size: " <<
  // playlistsInDatabase.size();

  bool success = false;
  // find in the database
  QMetaObject::invokeMethod(_worker, "getPlaylistsFromDatabase",
                            Qt::BlockingQueuedConnection, &playlistsInDatabase,
                            &success);

  if (success) {
    // count the number of Playlists being found
    int rowCount = (playlistsInDatabase.size() / 2);

    // qDebug() << "rowCount: " << rowCount;
    // set the table in Dialog Management to the corresponding number of rows
    ui->tableWidgetDatabase->setRowCount(rowCount);

    // populate the table with data from query
    QTableWidgetItem *item;

    int row = 0;
    for (size_t i = 0; i < playlistsInDatabase.size(); ++i) {

      // Playlist ID
      if (i % 2 == 0) {
        item = new QTableWidgetItem(playlistsInDatabase[i]);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidgetDatabase->setItem(row, 0, item);
      }
      // Playlist Name
      else {
        item = new QTableWidgetItem(playlistsInDatabase[i]);
        ui->tableWidgetDatabase->setItem(row, 1, item);
        ++row;
      }
    }

    // customizing the looks
    ui->tableWidgetDatabase->resizeColumnsToContents();
    ui->tableWidgetDatabase->setAlternatingRowColors(true);
  }
}
