#include "cplaylistcontainer.h"

const char *CPlaylistContainer::sortMethodsTXT[int(
    CPlaylistContainer::art_t::numberOfMethods)] = {
    "Random",   "by Artist", "by Album", "by Year",
    "by Genre", "undo Sort", "Title"};

const CTrack &CPlaylistContainer::operator[](const size_t &idx) const {
  if (idx >= _playlist_ptr_mainwindow_vector.size()) {
    throw std::out_of_range("index out of range");
  }
  return *_playlist_ptr_mainwindow_vector[idx];
}

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

bool CPlaylistContainer::fillPlaylistWithDatabaseTracks() {
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
