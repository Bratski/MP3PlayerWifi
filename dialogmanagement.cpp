#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent,
                                   CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogManagement), _playlist(playlist) {
  ui->setupUi(this);

  // initialize the window
  setWindowTitle("Playlist Management");
  readDatabase();

  // setting the button connections
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogManagement::close);
  QObject::connect(ui->pushButtonOpen, &QPushButton::clicked, this,
                   &DialogManagement::openPlaylist);
}

DialogManagement::~DialogManagement() { delete ui; }

void DialogManagement::openPlaylist() {
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
  // create a query, and find in the database
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

  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogManagement::readDatabase() {
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
    for (int col = 0; col < ui->tableWidgetPlaylists->columnCount(); ++col) {
      item = new QTableWidgetItem(query.value(col).toString());
      ui->tableWidgetPlaylists->setItem(row, col, item);
    }
    ++row;
  }

  // customizing the looks
  ui->tableWidgetPlaylists->resizeColumnsToContents();
  ui->tableWidgetPlaylists->setAlternatingRowColors(true);
  // ui->tableWidgetCurrentPlaylist->hideColumn(0);
}
