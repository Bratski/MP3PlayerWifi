#include "dialogaddplaylist.h"
#include "ui_dialogaddplaylist.h"

DialogAddPlaylist::DialogAddPlaylist(QWidget *parent,
                                     CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogAddPlaylist), _playlist(playlist) {
  ui->setupUi(this);
  setWindowTitle("Add Playlist");
  readDatabase();
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogAddPlaylist::close);
  QObject::connect(ui->pushButtonAdd, &QPushButton::clicked, this,
                   &DialogAddPlaylist::addPlaylist);
}

DialogAddPlaylist::~DialogAddPlaylist() { delete ui; }

void DialogAddPlaylist::addPlaylist() {

  // get the playlist name from the selected table widget item
  QString name = ui->tableWidgetDatabase->currentItem()->text();
  qDebug() << "name: " << name;

  // add the playlist object with the all the tracks data from the database
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
      "WHERE Playlist.PllName = :pllName");
  query.bindValue(":pllName", name);

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
    qDebug() << "Added track:" << title << "by" << artist;
  }
  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogAddPlaylist::readDatabase() {
  // empty the list with playlists
  ui->tableWidgetDatabase->clearContents();
  ui->tableWidgetDatabase->setRowCount(0);

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
  ui->tableWidgetDatabase->setRowCount(rowCount);

  // populate the table with data from query
  QTableWidgetItem *item;
  query.seek(-1); // reset query to start position
  int row = 0;
  while (query.next()) {
    // Playlist ID
    item = new QTableWidgetItem(query.value(0).toString());
    ui->tableWidgetDatabase->setItem(row, 0, item);
    // Playlist Name
    item = new QTableWidgetItem(query.value(1).toString());
    ui->tableWidgetDatabase->setItem(row, 1, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetDatabase->resizeColumnsToContents();
  ui->tableWidgetDatabase->setAlternatingRowColors(true);
}
