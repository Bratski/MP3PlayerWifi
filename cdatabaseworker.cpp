#include "cdatabaseworker.h"

CDatabaseWorker::CDatabaseWorker(QObject *parent) : QObject{parent} {}

void CDatabaseWorker::initialize(bool *success) {
  *success = createConnection();
}

void CDatabaseWorker::readDataBasePlaylist(CPlaylistContainer *playlist,
                                           int defaultPlaylistID,
                                           bool *success) {
  QSqlQuery query;
  query.prepare("SELECT Playlist.PllID, Playlist.PllName FROM Playlist WHERE "
                "PllID = :id ");
  query.bindValue(":id", defaultPlaylistID);

  if (!query.exec()) {
    *success = false;
    return;
  }
  if (query.next()) {
    playlist->setPllID(query.value(0).toInt());
    playlist->setPllName(query.value(1).toString());
    readPlaylistTracksFromDatabase(playlist, success);
    if (!*success) {
      *success = false;
      return;
    }
  }
  *success = true;
}

void CDatabaseWorker::writePlaylistTracksToDatabase(
    CPlaylistContainer *playlist, bool *success) {

  // create a query
  QSqlQuery query;
  tracknr = 0;

  // in case the playlist has been emptied
  if (playlist->getNumberOfTracks() == 0) {
    // clear the database entries for that playlist
    query.prepare("DELETE FROM TrackPlaylist WHERE PllFK = :pllID ");
    query.bindValue(":pllID", playlist->getPllID());
    if (!query.exec()) {
      qDebug() << "error deleting tracks from the playlist";
      *success = false;
      return;
    }
    *success = false;
    return;
  }

  // clear the database entries for that playlist first, otherwise the tracks
  // will be added to the playlist, as far as they arent already in the database
  // available, isnt there some sychronize function, as alternative, it might be
  // more efficient?
  query.prepare("DELETE FROM TrackPlaylist WHERE PllFK = :pllID ");
  query.bindValue(":pllID", playlist->getPllID());
  if (!query.exec()) {
    qDebug() << "error deleting tracks from the playlist";
    *success = false;
    return;
  }

  // iterate through the pointer vector
  for (auto it = playlist->beginPtr(); it != playlist->endPtr(); ++it) {

    // qDebug() << "artist: " << (*it)->getArtist();
    // for one track:
    ++tracknr;
    emit sendProgress(tracknr);
    //  Insert or update artists
    query.prepare("INSERT INTO Artist (ArtName, ArtGenre) "
                  "VALUES (:artName, :artGenre) "
                  "ON CONFLICT(ArtName) DO UPDATE SET ArtGenre = :artGenre ");
    query.bindValue(":artName", (*it)->getArtist());
    query.bindValue(":artGenre", (*it)->getGenre());
    if (!query.exec()) {
      qDebug() << "error inserting artist";
      *success = false;
      return;
    }

    // Insert or update albums
    query.prepare(
        "INSERT INTO Album (AlbName, AlbYear, AlbArtFK) VALUES (:albName, "
        ":albYear, (SELECT ArtID FROM Artist WHERE ArtName = :artName)) ON "
        "CONFLICT(AlbName, AlbArtFK) DO UPDATE SET AlbYear = :albYear ");
    query.bindValue(":albName", (*it)->getAlbum());
    query.bindValue(":albYear", (*it)->getYear());
    query.bindValue(
        ":artName",
        (*it)->getArtist()); // Reuse the artist name to get the ArtID
    if (!query.exec()) {
      qDebug() << "error inserting album";
      *success = false;
      return;
    }

    // Insert or update track
    query.prepare(
        "INSERT INTO Track (TraName, TraNumber, TraDuration, TraBitrate, "
        "TraSamplerate, TraChannels, TraFileLocation, TraAlbFK) VALUES "
        "(:traTitle, :traNumber, :traDuration, :traBitrate, :traSamplerate, "
        ":traChannels, :traFileLocation, (SELECT AlbID FROM Album WHERE "
        "AlbName = :albName)) ON CONFLICT(TraName, TraAlbFK) DO UPDATE SET "
        "TraNumber = :traNumber, TraDuration = :traDuration, TraBitrate = "
        ":traBitrate, TraSamplerate = :traSamplerate, TraChannels = "
        ":traChannels, TraFileLocation = :traFileLocation ");
    query.bindValue(":traTitle", (*it)->getTitle());
    query.bindValue(":traNumber", (*it)->getNumber());
    query.bindValue(":traDuration", (*it)->getDuration());
    query.bindValue(":traBitrate", (*it)->getBitrate());
    query.bindValue(":traSamplerate", (*it)->getSamplerate());
    query.bindValue(":traChannels", (*it)->getChannels());
    query.bindValue(":traFileLocation", (*it)->getFileLocation());
    query.bindValue(":albName",
                    (*it)->getAlbum()); // Reuse the album name to get the AlbID
    if (!query.exec()) {
      qDebug() << "error inserting track";
      *success = false;
      return;
    }
    // associate the track with the playlist
    query.prepare(
        "INSERT INTO TrackPlaylist (TraFK, PllFK) VALUES ((SELECT TraID FROM "
        "Track WHERE TraName = :traTitle), (SELECT PllID FROM Playlist WHERE "
        "PllName = :pllName)) ON CONFLICT(TraFK, PllFK) DO NOTHING ");
    query.bindValue(
        ":traTitle",
        (*it)->getTitle()); // Reuse the track title to get the TraID
    query.bindValue(
        ":pllName",
        playlist->getPllName()); // Reuse the playlist name to get the PllID
    if (!query.exec()) {
      qDebug() << "error inserting playlist";
      *success = false;
      return;
    }
    qDebug() << "Track " << (*it)->getTitle() << " saved";
  }

  qDebug() << "Playlist successfully saved in database";
  emit progressReady();
  *success = true;
  return;
}

void CDatabaseWorker::readPlaylistTracksFromDatabase(
    CPlaylistContainer *playlist, bool *success) {
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
  query.bindValue(":playlistID", playlist->getPllID());

  // if the query returned with an error
  if (!query.exec()) {
    *success = false;
    return;
  }

  // populate the _playlist with tracks from query
  int year, number, duration, bitrate, samplerate, channels;
  QString id, title, artist, album, genre, filelocation;
  while (query.next()) {
    // to make shure the track id is unique, a character is added to the
    // incrementing number, to identify its origin: "F" from file, "D" from
    // database
    id =
        "D" +
        query.value(0)
            .toString(); // to identify its origin, a "D" from DATABASE is added
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

    playlist->addTrack(newtrack);

    // Debug output for each track
    // qDebug() << "Added track:" << title << "by" << artist;
  }
  *success = true;
  return;
}

void CDatabaseWorker::addNewPlaylist(const QString &name, bool *success,
                                     bool *doubleName) {
  // create a query, and check if name already exists in the database
  QSqlQuery query;

  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    *success = false;
    return;
  }

  if (query.first()) {
    qDebug() << "The playlist already exists, please change the name";
    *success = false;
    *doubleName = true;
    return;
  }

  // create the new playlist in the database
  query.prepare("INSERT INTO Playlist (PllName) VALUES (:PllName) ");
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    qDebug() << "Error creating new Playlist in the Database";
    *success = false;
    return;
  }
  *success = true;
  return;
}

void CDatabaseWorker::getPlaylistsFromDatabase(std::vector<QString> *list,
                                               bool *success) {

  // create a query, and find in the database
  QSqlQuery query;
  query.prepare("SELECT Playlist.PllID, Playlist.PllName FROM Playlist ");

  if (!query.exec()) {
    qDebug() << "Error selecting the Playlist name in the Database";
    *success = false;
    return;
  }

  // populate the vector with data from query
  query.seek(-1); // reset query to start position
  while (query.next()) {
    // Playlist ID
    list->push_back(query.value(0).toString());
    // Playlist Name
    list->push_back(query.value(1).toString());
  }

  *success = true;
  return;
}

void CDatabaseWorker::deletePlaylist(const QString &name, bool *success) {
  QSqlQuery query;
  // creating the deletion query
  query.prepare("DELETE FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  // deleting the database playlist entry
  if (!query.exec()) {
    qDebug() << "Error removing Playlists!";
    *success = false;
    return;
  }
  *success = true;
  return;
}

void CDatabaseWorker::updatePlaylistInDatabase(const QString &name,
                                               const int &id, bool *success,
                                               bool *doubleName) {

  // check if the edited name already exists in the database
  QSqlQuery query;
  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    qDebug() << "Error checking the Playlist name in the Database";
    *success = false;
    return;
  }
  if (query.first()) {
    qDebug() << "Error The playlist already exists, please change the name";
    *doubleName = true;
    *success = false;
    return;
  }

  // update the database at position id
  query.prepare("UPDATE Playlist SET PllName = :PllName WHERE PllID = :PllID ");
  query.bindValue(":PllID", id);
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    qDebug() << "Error updating name in the Playlist name in the Database";
    *success = false;
    return;
  }
  *success = true;
  return;
}

void CDatabaseWorker::cleanupDatabase(bool *success) {
  // Cleaning tables, remove orphaned tracks, albums and artists
  QSqlQuery query;

  // Clean the Track table
  if (!query.exec("DELETE FROM Track WHERE TraID NOT IN (SELECT TraFK FROM "
                  "TrackPlaylist) ")) {
    qDebug() << "Database Error, Failed to clean Track table: " +
                    query.lastError().text();
    *success = false;
    return;
  }

  // Clean the Album table
  if (!query.exec("DELETE FROM Album WHERE AlbID NOT IN (SELECT TraAlbFK FROM "
                  "Track) ")) {
    qDebug() << "Database Error, Failed to clean Album table: " +
                    query.lastError().text();
    *success = false;
    return;
  }

  // Clean the Artist table
  if (!query.exec("DELETE FROM Artist WHERE ArtID NOT IN (SELECT AlbArtFK FROM "
                  "Album) ")) {
    qDebug() << "Database Error, Failed to clean Artist table: " +
                    query.lastError().text();
    *success = false;
    return;
  }

  *success = true;
  return;
}

void CDatabaseWorker::closeDatabase() {
  // closing the database
  QSqlDatabase db =
      QSqlDatabase::database(); // Get the default database connection
  if (db.isOpen()) {
    db.close(); // Close the database connection
    qDebug() << "Database connection closed";
  }
}
