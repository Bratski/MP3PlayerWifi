#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent,
                                   CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogManagement), _playlist(playlist) {
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
}

DialogManagement::~DialogManagement() { delete ui; }

void DialogManagement::openPlaylist() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  // check if one row has been selected, if yes, which one? If not return with
  // error message

  QList<QTableWidgetItem *> selectedItems =
      ui->tableWidgetPlaylists->selectedItems();

  if (selectedItems.size() != 2) {
    QMessageBox msg;
    msg.addButton("OK", QMessageBox::YesRole);
    msg.setWindowTitle("Error");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("Only 1 playlist at the time can be selected to be opened!");
    msg.exec();
    return;
  }

  // empty the playlist
  _playlist->clear();

  // extract the PllID and PllName and set the _playlist
  _playlist->setPllID(selectedItems[0]->text().toInt());
  _playlist->setPllName(selectedItems[1]->text());

  // fill the playlist object with the all the tracks data from the database
  // found at that particular playlist
  QSqlQuery query;
  query.prepare(
      "SELECT Track.TraID, Track.TraName, Artist.ArtName, Album.AlbName, "
      "Album.AlbYear, Track.TraNumber, Artist.ArtGenre, Track.TraDuration, "
      "Track.TraBitrate, Track.TraSamplerate, Track.TraChannels, "
      "Track.TraFileLocation "
      "FROM Track "
      "JOIN Album ON Track.TraAlbFK = Album.AlbID "
      "JOIN Artist ON Album.AlbArtFK = Artist.ArtID "
      "JOIN TrackPlaylist ON Track.TraID = TrackPlaylist.TraFK "
      "JOIN Playlist ON TrackPlaylist.PllFK = Playlist.PllID "
      "WHERE Playlist.PllID = :playlistID");
  query.bindValue(":playlistID", _playlist->getPllID());

  // if the query returned with an error
  if (!query.exec()) {
    QMessageBox msg;
    msg.addButton("OK", QMessageBox::YesRole);
    msg.setWindowTitle("Error");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("something wrong with the db-query");
    msg.exec();
    return;
  }

  // populate the _playlist with tracks from query
  int id, year, number, duration, bitrate, samplerate, channels;
  QString title, artist, album, genre, filelocation;
  while (query.next()) {

    id = query.value(0).toInt();
    title = query.value(1).toString();
    artist = query.value(2).toString();
    album = query.value(3).toString();
    year = query.value(4).toInt();
    number = query.value(5).toInt();
    genre = query.value(6).toString();
    duration = query.value(7).toInt();
    bitrate = query.value(8).toInt();
    samplerate = query.value(9).toInt();
    channels = query.value(10).toInt();
    filelocation = query.value(11).toString();

    CTrack newtrack(id, title, artist, album, year, number, genre, duration,
                    bitrate, samplerate, channels, filelocation);

    _playlist->addTrack(newtrack);

    // Debug output for each track
    // qDebug() << "Added track:" << title << "by" << artist;
  }

  // prevents interfering with the namePlaylistEdited function
  blockSignals(false);
  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogManagement::addNewPlaylist() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  QString name;
  name = ui->lineEditNewPlaylist->text();

  // check if there is a name in the input field,
  if (name.size() == 0) {
    QMessageBox msg;
    msg.addButton("OK", QMessageBox::YesRole);
    msg.setWindowTitle("Error");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("No valid name in the input field");
    msg.exec();
    return;
  }

  // create a query, and check if name already exists in the database
  QSqlQuery query;

  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  if (!query.exec())
    return;

  if (query.first()) {
    QMessageBox msg;
    msg.addButton("OK", QMessageBox::YesRole);
    msg.setWindowTitle("Error");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("The playlist already exists, please change the name");
    msg.exec();
    return;
  }

  // create the new playlist in the database
  query.prepare("INSERT INTO Playlist (PllName) VALUES (:PllName) ");
  query.bindValue(":PllName", name);

  if (!query.exec())
    return;

  // update the table
  readDatabase();
  blockSignals(false);
}

void DialogManagement::deletePlaylist() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  // check if rows are selected in the table
  QList<QTableWidgetItem *> selectedItems =
      ui->tableWidgetPlaylists->selectedItems();

  // return if no entries are selected
  if (selectedItems.size() == 0)
    return;

  // warning are you sure
  QMessageBox msg;
  msg.addButton("Yes", QMessageBox::YesRole);
  msg.addButton("No", QMessageBox::NoRole);
  msg.setWindowTitle("Delete?");
  msg.setIcon(QMessageBox::Warning);
  msg.setText("Are you sure to delete this playlist, inclusive its contents?");

  // msg.exec() returns "3" if norole, "2" if yesrole
  if (msg.exec() == 3) {
    return;
  }

  // delete the selected rows from the database
  QSqlQuery query;
  QString name;

  for (auto it = selectedItems.begin(); it != selectedItems.end(); ++it) {
    if ((*it)->column() == 1) {
      name = (*it)->text();
      query.prepare("DELETE FROM Playlist WHERE PllName = :PllName ");
      query.bindValue(":PllName", name);

      if (!query.exec())
        return;
    }
  }

  // refresh the table
  readDatabase();
  // prevents interfering with the namePlaylistEdited function
  blockSignals(false);
}

void DialogManagement::namePlaylistEdited(QTableWidgetItem *item) {
  // If the flag is set, ignore the signal to avoid recursion
  if (isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  isEditing = true;
  // get the edited name
  QString name = item->text();

  // get the database id, where the new name must be stored
  int row = item->row();
  int id = ui->tableWidgetPlaylists->item(row, 0)->text().toInt();

  // check if the edited name already exists in the database
  QSqlQuery query;
  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  if (!query.exec())
    return;

  if (query.first()) {
    QMessageBox msg;
    msg.addButton("OK", QMessageBox::YesRole);
    msg.setWindowTitle("Error");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("The playlist already exists, please change the name");
    msg.exec();
    readDatabase();
    return;
  }

  // update the database at position id
  query.prepare("UPDATE Playlist SET PllName = :PllName WHERE PllID = :PllID ");
  query.bindValue(":PllID", id);
  query.bindValue(":PllName", name);

  if (!query.exec())
    return;

  // update the table
  readDatabase();
  // prevents interfering with the namePlaylistEdited function
  isEditing = false;
}

void DialogManagement::readDatabase() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  // empty the list with playlists
  ui->tableWidgetPlaylists->clearContents();
  ui->tableWidgetPlaylists->setRowCount(0);

  // create a query, and find in the database
  QSqlQuery query;
  query.prepare("SELECT Playlist.PllID, Playlist.PllName FROM Playlist ");

  if (!query.exec())
    return;

  // count the number of Playlists being found
  int rowCount = 0;
  while (query.next())
    ++rowCount;

  // set the table in Dialog Management to the corresponding number of rows
  ui->tableWidgetPlaylists->setRowCount(rowCount);

  // populate the table with data from query
  QTableWidgetItem *item;
  query.seek(-1); // reset query to start position
  int row = 0;
  while (query.next()) {
    item = new QTableWidgetItem(query.value(0).toString());
    ui->tableWidgetPlaylists->setItem(row, 0, item);

    item = new QTableWidgetItem(query.value(1).toString());
    ui->tableWidgetPlaylists->setItem(row, 1, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetPlaylists->resizeColumnsToContents();
  ui->tableWidgetPlaylists->setAlternatingRowColors(true);

  // prevents interfering with the namePlaylistEdited function
  blockSignals(false);
}
