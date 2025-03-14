#include "cplaylistcontainer.h"

const char *CPlaylistContainer::sortMethodsTXT[int(
    CPlaylistContainer::art_t::numberOfMethods)] = {
    "Random",   "by Artist", "by Album", "by Year",
    "by Genre", "undo Sort", "Title"};

// const operator[] overload
const CTrack &CPlaylistContainer::operator[](const size_t &idx) const {
  if (idx >= _playlist_ptr_mainwindow_vector.size()) {
    throw std::out_of_range("index out of range");
  }
  return *_playlist_ptr_mainwindow_vector[idx];
}

// non-const operator[] overload
CTrack &CPlaylistContainer::operator[](const size_t &idx) {
  if (idx >= _playlist_ptr_mainwindow_vector.size()) {
    throw std::out_of_range("index out of range");
  }
  return *_playlist_ptr_mainwindow_vector[idx];
}

int CPlaylistContainer::calculatePlaylistTotalTime() {
  int totaltime = 0;
  for (const auto &track : _playlist_obj_vector) {
    totaltime += track.getDuration();
  }
  return totaltime;
}

void CPlaylistContainer::addTrack(CTrack &track) {
  _playlist_obj_vector.push_back(track);
  _playlist_ptr_mainwindow_vector.push_back(std::make_shared<CTrack>(track));
}

void CPlaylistContainer::removeTrack(int &id) {
  // method 1
  // track object is removed from the vector, if lambda track returns true
  // auto it1 = [id](CTrack &track) { return id == track.getID(); };
  // auto it2 = std::remove_if(_playlist_obj_vector.begin(),
  //                           _playlist_obj_vector.end(), it1);
  // _playlist_obj_vector.erase(it2, _playlist_obj_vector.end());

  // method 2
  // iterate through the vector and delete on corresponding ids
  for (auto it = _playlist_obj_vector.begin(); it != _playlist_obj_vector.end();
       ++it) {
    if (id == it->getID()) {
      _playlist_obj_vector.erase(it);
      break; // no duplicates are allowed, so it is ok to stop the operation,
      // in case the track has been found
    }
  }

  // because of the id problem (Tracks not coming out of the database have an id
  // of 0) Tracks will be deleted by rowNumber from the tablewidget at index
  // position in the vector
  //_playlist_obj_vector.erase(_playlist_obj_vector.begin() + (rowNumber));

  // synchronise the pointer vector with the object vector
  sortPlaylist(art_t::undoSort);
}

void CPlaylistContainer::clear() {
  _playlist_obj_vector.clear();
  _playlist_ptr_filter_vector.clear();
  _playlist_ptr_mainwindow_vector.clear();
}

void CPlaylistContainer::sortPlaylist(art_t wayofsorting) {
  // Initialize random number generator
  std::random_device rd; // Seed for the random number generator
  std::mt19937 rng(rd());

  // defining the sorting criteria
  // for the artists
  auto sortbyartist = [](const std::shared_ptr<CTrack> tr1,
                         const std::shared_ptr<CTrack> tr2) {
    return tr1->getArtist() < tr2->getArtist();
  };

  // for the albums
  auto sortbyalbum = [](const std::shared_ptr<CTrack> tr1,
                        const std::shared_ptr<CTrack> tr2) {
    return tr1->getAlbum() < tr2->getAlbum();
  };

  // for the album year
  auto sortbyyear = [](const std::shared_ptr<CTrack> tr1,
                       const std::shared_ptr<CTrack> tr2) {
    return tr1->getYear() > tr2->getYear();
  };

  // for the genres
  auto sortbygenre = [](const std::shared_ptr<CTrack> tr1,
                        const std::shared_ptr<CTrack> tr2) {
    return tr1->getGenre() < tr2->getGenre();
  };

  switch (wayofsorting) {
  case art_t::random:
    // sort the pointer-vector randomly
    std::shuffle(_playlist_ptr_mainwindow_vector.begin(),
                 _playlist_ptr_mainwindow_vector.end(), rng);
    break;

  case art_t::byArtist:
    // sort the pointer-vector by Artist name, alphabetically
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbyartist);
    break;
  case art_t::byAlbum:
    // sort the pointer-vector by Artist name, alphabetically
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbyalbum);
    break;

  case art_t::byYear:
    // sort the pointer-vector by year
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbyyear);
    break;
  case art_t::byGenre:
    // sort the pointer-vector by genre
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbygenre);
    break;
  case art_t::undoSort:
    // clear the pointer vector
    _playlist_ptr_mainwindow_vector.clear();
    // fill the pointer vector with pointers to track objects in the _playlist
    // vector
    for (auto it = _playlist_obj_vector.begin();
         it != _playlist_obj_vector.end(); ++it) {
      _playlist_ptr_mainwindow_vector.push_back(std::make_shared<CTrack>(*it));
    }
    break;
  default:
    break;
  }
}

void CPlaylistContainer::filterPlaylist(art_t wayoffiltering,
                                        const QString &text) {
  // empty the search vector with pointers
  _playlist_ptr_filter_vector.clear();
  switch (wayoffiltering) {
  case art_t::byTitle:
    for (auto &track : _playlist_obj_vector) {
      // is the search text corresponding to the track title, the pointer of
      // that track is added to the ptr-filter vector
      if (track.getTitle() == text) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byAlbum:
    for (auto &track : _playlist_obj_vector) {
      // same for the track album
      if (track.getAlbum() == text) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byArtist:
    for (auto &track : _playlist_obj_vector) {
      // same for the track artist
      if (track.getArtist() == text) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byGenre:
    for (auto &track : _playlist_obj_vector) {
      // same for the track genre
      if (track.getGenre() == text) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byYear:
    for (auto &track : _playlist_obj_vector) {
      // same for the track year
      if (track.getYear() == text.toInt()) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  default:
    break;
  }
}

bool CPlaylistContainer::readPlaylistFromDatabase() {
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
  query.bindValue(":playlistID", _PllID);

  // if the query returned with an error
  if (!query.exec()) {
    return false;
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

    this->addTrack(newtrack);

    // Debug output for each track
    // qDebug() << "Added track:" << title << "by" << artist;
  }
  return true;
}

// TODO if a lot of entries are to be saved, it demands a lot of CPU power, some
// sort of progress bar is needed
bool CPlaylistContainer::writePlaylistToDatabase() {

  // create a query
  QSqlQuery query;

  // in case the playlist has been emptied
  if (_playlist_ptr_mainwindow_vector.empty()) {
    // clear the database entries for that playlist
    query.prepare("DELETE FROM TrackPlaylist WHERE PllFK = :pllID ");
    query.bindValue(":pllID", _PllID);
    if (!query.exec()) {
      qDebug() << "error deleting tracks from the playlist";
      return false;
    }
    return true;
  }

  // clear the database entries for that playlist first, otherwise the tracks
  // will be added to the playlist, as far as they arent already in the database
  // available, isnt there some sychronize function, as alternative, it might be
  // more efficient?
  query.prepare("DELETE FROM TrackPlaylist WHERE PllFK = :pllID ");
  query.bindValue(":pllID", _PllID);
  if (!query.exec()) {
    qDebug() << "error deleting tracks from the playlist";
    return false;
  }

  // iterate through the pointer vector
  for (auto it = _playlist_ptr_mainwindow_vector.begin();
       it != _playlist_ptr_mainwindow_vector.end(); ++it) {

    // qDebug() << "artist: " << (*it)->getArtist();
    // for one track:
    // Insert or update artists
    query.prepare("INSERT INTO Artist (ArtName, ArtGenre) "
                  "VALUES (:artName, :artGenre) "
                  "ON CONFLICT(ArtName) DO UPDATE SET ArtGenre = :artGenre ");
    query.bindValue(":artName", (*it)->getArtist());
    query.bindValue(":artGenre", (*it)->getGenre());
    if (!query.exec()) {
      qDebug() << "error artist";
      return false;
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
      qDebug() << "error album";
      return false;
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
      qDebug() << "error track";
      return false;
    }
    // associate the track with the playlist
    query.prepare(
        "INSERT INTO TrackPlaylist (TraFK, PllFK) VALUES ((SELECT TraID FROM "
        "Track WHERE TraName = :traTitle), (SELECT PllID FROM Playlist WHERE "
        "PllName = :pllName)) ON CONFLICT(TraFK, PllFK) DO NOTHING ");
    query.bindValue(
        ":traTitle",
        (*it)->getTitle()); // Reuse the track title to get the TraID
    query.bindValue(":pllName",
                    _PllName); // Reuse the playlist name to get the PllID
    if (!query.exec()) {
      qDebug() << "error playlist";
      return false;
    }
  }

  return true;
}
