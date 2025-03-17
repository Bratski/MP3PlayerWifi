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

void CPlaylistContainer::removeTrack(const QString &id) {
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
      // in case the track has been found, how to be sure the track id is always
      // unique? Impossible? because you have to compare every time if the id is
      // already in use by the database. How to make a difference between
      // manually and database added tracks to the playlist vector? How to
      // design a key for that? Maybe turn the id into a string?
      // "key+incrementing number" where key determines its origin: database or
      // manually added?
    }
  }

  // because of the id problem (Tracks not coming out of the database have an id
  // of none, garbage probably) Tracks will be deleted by rowNumber from the
  // tablewidget at index position in the vector, bad idea!!, because after each
  // deletion, the rowNumber doesnt correspond to the next entry to be
  // deleted in the vector!
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
