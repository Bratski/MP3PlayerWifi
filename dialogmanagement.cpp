#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent,
                                   CPlaylistContainer *playlist,
                                   CDatabaseWorker *worker,
                                   bool *playlistChanged)
    : QDialog(parent), ui(new Ui::DialogManagement), _playlist(playlist),
      _worker(worker), _playlistChanged(playlistChanged) {
  ui->setupUi(this);

  // initialize the window
  setWindowTitle("Playlist Management");
  readDatabase();
  // ui->tableWidgetPlaylists->hideColumn(0);

  // setting the button connections
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogManagement::close);
  QObject::connect(ui->pushButtonOpen, &QPushButton::clicked, this,
                   &DialogManagement::openPlaylist);
  QObject::connect(ui->pushButtonAdd, &QPushButton::clicked, this,
                   &DialogManagement::addNewPlaylist);
  QObject::connect(ui->pushButtonDelete, &QPushButton::clicked, this,
                   &DialogManagement::deletePlaylist);

  // event of an item has been edited in the table widget
  QObject::connect(ui->tableWidgetPlaylists, &QTableWidget::itemChanged, this,
                   &DialogManagement::namePlaylistEdited);
  QObject::connect(ui->tableWidgetPlaylists, &QTableWidget::itemDoubleClicked,
                   this, &DialogManagement::openPlaylist);
}

DialogManagement::~DialogManagement() { delete ui; }

void DialogManagement::openPlaylist() {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // check if one row has been selected, if yes, which one? If not return with
  // error message

  QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetPlaylists->selectedRanges();

  if (selectedRanges.size() == 0) {
    QMessageBox::warning(this, "Error",
                         "No playlist has been selected to be opened!");
    _isEditing = false;
    return;
  }

  // if yes, is it only one row or more?
  if (selectedRanges.first().rowCount() != 1) {
    QMessageBox::warning(this, "Error",
                         "Only 1 Playlist must be selected to be opened!");
    _isEditing = false;
    return;
  }

  // only ask to save the playlist to database, if the playlist has been
  // edited (_playlistChanged is set true)

  if (*_playlistChanged) {
    QMessageBox msg;
    msg.addButton("Yes", QMessageBox::YesRole);
    msg.addButton("No", QMessageBox::NoRole);
    msg.setWindowTitle("Save Playlist?");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("Do you want to save the changes to the playlist?");

    // msg.exec() returns "3" if norole, "2" if yesrole
    if (msg.exec() == 2) {
      emit saveToDBMainWindow();
    }
  }

  // get the items at the selected row
  int selectedRow = selectedRanges.first().topRow();
  QTableWidgetItem *idItem =
      ui->tableWidgetPlaylists->item(selectedRow, 0); // Column 0 (ID)
  QTableWidgetItem *nameItem =
      ui->tableWidgetPlaylists->item(selectedRow, 1); // Column 1 (Name)

  // empty the playlist
  _playlist->clear();

  // extract the PllID and PllName from the items and set the _playlist name and
  // id
  _playlist->setPllID(idItem->text().toInt());
  _playlist->setPllName(nameItem->text());

  bool success = false;
  // fill the playlist with the database tracks
  QMetaObject::invokeMethod(_worker, "readPlaylistTracksFromDatabase",
                            Qt::BlockingQueuedConnection, _playlist, &success);

  // if all went well, the playlist is a exact representation of the database,
  // and no need to save it again
  if (success)
    *_playlistChanged = false;

  // qDebug() << "success: " << success;
  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogManagement::addNewPlaylist() {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  QString name;
  name = ui->lineEditNewPlaylist->text();

  // check if there is a name in the input field,
  if (name.size() == 0) {
    QMessageBox::warning(this, "Error", "No valid name in the input field");
    _isEditing = false;
    return;
  }

  bool success = false;
  bool doubleName = false;
  // add a new playlist with the name to the database
  QMetaObject::invokeMethod(_worker, "addNewPlaylist",
                            Qt::BlockingQueuedConnection, name, &success,
                            &doubleName);

  if (success) {
    qDebug() << "added the name playlist " << name << " successfully";
  }
  if (doubleName) {
    QMessageBox::warning(this, "Error",
                         "Name already in use, choose another name");
  }

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // update the table
  readDatabase();
}

void DialogManagement::deletePlaylist() {
  // if the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // check if rows are selected in the table
  QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetPlaylists->selectedRanges();

  // delete the selected rows from the database

  // Iterate through each selected range
  for (const QTableWidgetSelectionRange &range : selectedRanges) {
    // Iterate through each row in the range
    for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
      // Access the items in the row
      QTableWidgetItem *idItem =
          ui->tableWidgetPlaylists->item(row, 0); // Column 0 (ID)
      QTableWidgetItem *nameItem =
          ui->tableWidgetPlaylists->item(row, 1); // Column 1 (Name)

      // Get the ID and Name
      int id = idItem->text().toInt();
      QString name = nameItem->text();

      // if the playlist is currently opened in the main window or is the first
      // entry in the database, it can not be deleted
      if (name == _playlist->getPllName() || id == 1) {
        QMessageBox::warning(this, "Error",
                             "The current playlist cannot be deleted!");
        _isEditing = false;
        return;
      }

      // warning are you sure
      QMessageBox msg;
      msg.addButton("Yes", QMessageBox::YesRole);
      msg.addButton("No", QMessageBox::NoRole);
      msg.setWindowTitle("Delete?");
      msg.setIcon(QMessageBox::Warning);
      msg.setText("Are you sure to delete this playlist: " + name +
                  " inclusive its contents?");

      // msg.exec() returns "3" if norole, "2" if yesrole
      if (msg.exec() == 3) {
        _isEditing = false;
        return;
      }

      bool success = false;
      // delete the playlist from the database
      QMetaObject::invokeMethod(_worker, "deletePlaylist",
                                Qt::BlockingQueuedConnection, name, &success);
    }
  }

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // refresh the table
  readDatabase();
}

void DialogManagement::namePlaylistEdited(QTableWidgetItem *item) {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // get the edited name
  QString name = item->text();

  // get the database id, where the new name must be stored
  int row = item->row();
  int id = ui->tableWidgetPlaylists->item(row, 0)->text().toInt();

  bool success = false;
  bool doubleName = false;
  // update the database
  QMetaObject::invokeMethod(_worker, "updatePlaylistInDatabase",
                            Qt::QueuedConnection, name, id, &success,
                            &doubleName);

  if (doubleName) {
    QMessageBox::warning(this, "Error",
                         "Name already in use, choose another name");
  }
  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // update the table
  readDatabase();
}

void DialogManagement::readDatabase() {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // temporare QString vector
  std::vector<QString> playlistsInDatabase;

  // empty the list with playlists
  ui->tableWidgetPlaylists->clearContents();
  ui->tableWidgetPlaylists->setRowCount(0);
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
    ui->tableWidgetPlaylists->setRowCount(rowCount);

    // populate the table with data from query
    QTableWidgetItem *item;

    int row = 0;
    for (size_t i = 0; i < playlistsInDatabase.size(); ++i) {

      // Playlist ID
      if (i % 2 == 0) {
        item = new QTableWidgetItem(playlistsInDatabase[i]);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidgetPlaylists->setItem(row, 0, item);
      }
      // Playlist Name
      else {
        item = new QTableWidgetItem(playlistsInDatabase[i]);
        ui->tableWidgetPlaylists->setItem(row, 1, item);
        ++row;
      }
    }

    // customizing the looks
    ui->tableWidgetPlaylists->resizeColumnsToContents();
    ui->tableWidgetPlaylists->setAlternatingRowColors(true);
  }
  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;
}
