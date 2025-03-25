#include "cplaylistcontainer.h"

// not really necessary
const char *CPlaylistContainer::sortMethodsTXT[int(
    CPlaylistContainer::art_t::numberOfMethods)] = {
    "Random",      "by Artist", "by Album",  "by Year",      "by Genre",
    "by Database", "Title",     "undo Sort", "By MainWindow"};

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

// to calculate the total time of the displayed playlist
int CPlaylistContainer::calculatePlaylistTotalTime() {
  int totaltime = 0;
  for (const auto &track : _playlist_ptr_mainwindow_vector) {
    totaltime += track->getDuration();
  }
  return totaltime;
}

// to add a track to the playlist
void CPlaylistContainer::addTrack(CTrack &track) {
  _playlist_obj_vector.push_back(track);
  _playlist_ptr_mainwindow_vector.push_back(std::make_shared<CTrack>(track));
}

// to remove a track from the playlist
void CPlaylistContainer::removeTrack(const QString &id) {
  // method 1
  // track object is removed from the vector, if lambda track returns true
  // auto it1 = [id](CTrack &track) { return id == track.getID(); };
  // auto it2 = std::remove_if(_playlist_obj_vector.begin(),
  //                           _playlist_obj_vector.end(), it1);
  // _playlist_obj_vector.erase(it2, _playlist_obj_vector.end());

  // method 2
  // iterate through the vector and delete on corresponding ids

  // delete Track from the object vector, is this relevant? Yes, because undo
  // filter returns all the deleted tracks, if objects havent been deleted
  for (auto it = _playlist_obj_vector.begin(); it != _playlist_obj_vector.end();
       ++it) {
    if (id == it->getID()) {
      _playlist_obj_vector.erase(it);
      break; // no duplicates are allowed, so it is ok to abort the operation
    }
  }

  // delete Track from the pointer vector
  for (auto it = _playlist_ptr_mainwindow_vector.begin();
       it != _playlist_ptr_mainwindow_vector.end(); ++it) {
    if (id == (*it)->getID()) {
      _playlist_ptr_mainwindow_vector.erase(it);
      break; // no duplicates are allowed, so it is ok to abort the operation
    }
  }
}

// empties all the vectors
void CPlaylistContainer::clear() {
  _playlist_obj_vector.clear();
  _playlist_ptr_filter_vector.clear();
  _playlist_ptr_mainwindow_vector.clear();
  _playlist_ptr_undosort_vector.clear();
}

// different ways to sort the pointer vector
void CPlaylistContainer::sortPlaylist(art_t wayofsorting) {
  // Initialize random number generator
  std::random_device rd; // Seed for the random number generator
  std::mt19937 rng(rd());

  // defining the sorting criteria
  // for the artists in lambda definitions
  auto sortbyartist = [](const std::shared_ptr<CTrack> tr1,
                         const std::shared_ptr<CTrack> tr2) {
    return tr1->getArtist().toLower() < tr2->getArtist().toLower();
  };

  // for the albums
  auto sortbyalbum = [](const std::shared_ptr<CTrack> tr1,
                        const std::shared_ptr<CTrack> tr2) {
    return tr1->getAlbum().toLower() < tr2->getAlbum().toLower();
  };

  // for the album year
  auto sortbyyear = [](const std::shared_ptr<CTrack> tr1,
                       const std::shared_ptr<CTrack> tr2) {
    return tr1->getYear() > tr2->getYear();
  };

  // for the genres
  auto sortbygenre = [](const std::shared_ptr<CTrack> tr1,
                        const std::shared_ptr<CTrack> tr2) {
    return tr1->getGenre().toLower() > tr2->getGenre().toLower();
  };

  // switching to the way of sorting
  switch (wayofsorting) {
  case art_t::random:
    // copy the main window ptr vector to the ptr undo sort vector
    _playlist_ptr_undosort_vector = _playlist_ptr_mainwindow_vector;

    // sort the main window pointer-vector randomly
    std::shuffle(_playlist_ptr_mainwindow_vector.begin(),
                 _playlist_ptr_mainwindow_vector.end(), rng);
    break;

  case art_t::byArtist:
    // copy the main window ptr vector to the ptr undo sort vector
    _playlist_ptr_undosort_vector = _playlist_ptr_mainwindow_vector;
    // sort the pointer-vector by Artist name, alphabetically
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbyartist);
    break;
  case art_t::byAlbum:
    // copy the main window ptr vector to the ptr undo sort vector
    _playlist_ptr_undosort_vector = _playlist_ptr_mainwindow_vector;
    // sort the pointer-vector by Artist name, alphabetically
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbyalbum);
    break;

  case art_t::byYear:
    // copy the main window ptr vector to the ptr undo sort vector
    _playlist_ptr_undosort_vector = _playlist_ptr_mainwindow_vector;
    // sort the pointer-vector by year
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbyyear);
    break;
  case art_t::byGenre:
    // copy the main window ptr vector to the ptr undo sort vector
    _playlist_ptr_undosort_vector = _playlist_ptr_mainwindow_vector;
    // sort the pointer-vector by genre
    std::sort(_playlist_ptr_mainwindow_vector.begin(),
              _playlist_ptr_mainwindow_vector.end(), sortbygenre);
    break;
  case art_t::byDatabase:
    // copy the main window ptr vector to the ptr undo sort vector
    _playlist_ptr_undosort_vector = _playlist_ptr_mainwindow_vector;
    // clear the pointer vector
    _playlist_ptr_mainwindow_vector.clear();
    // fill the pointer vector with pointers to track objects in the _playlist
    // vector
    for (auto it = _playlist_obj_vector.begin();
         it != _playlist_obj_vector.end(); ++it) {
      _playlist_ptr_mainwindow_vector.push_back(std::make_shared<CTrack>(*it));
    }
    break;
  case art_t::undoSort:
    // swap the pointer vector with the last sorted version, only if the undo
    // sort ptr vector is not empty
    if (!_playlist_ptr_undosort_vector.empty())
      _playlist_ptr_undosort_vector.swap(_playlist_ptr_mainwindow_vector);
    break;
  default:
    break;
  }
}

void CPlaylistContainer::filterPlaylist(art_t wayoffiltering,
                                        const QString &text) {
  // empty the search vector with pointers
  _playlist_ptr_filter_vector.clear();

  // switching to the way of filtering
  switch (wayoffiltering) {
  case art_t::byTitle:
    for (auto &track : _playlist_obj_vector) {
      // is the search text corresponding to the track title, the pointer of
      // that track is added to the ptr-filter vector // TODO partially
      // corresponding suggestions?
      if (track.getTitle().toLower() == text.toLower()) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byAlbum:
    for (auto &track : _playlist_obj_vector) {
      // same for the track album
      if (track.getAlbum().toLower() == text.toLower()) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byArtist:
    for (auto &track : _playlist_obj_vector) {
      // same for the track artist
      if (track.getArtist().toLower() == text.toLower()) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byGenre:
    for (auto &track : _playlist_obj_vector) {
      // same for the track genre
      if (track.getGenre().toLower() == text.toLower()) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byYear:
    for (auto &track : _playlist_obj_vector) {
      // same for the track year, if text has no numbers it is 0
      if (track.getYear() == text.toInt()) {
        _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(track));
      }
    }
    break;
  case art_t::byMainWindow:
    for (auto &track : _playlist_ptr_mainwindow_vector) {
      _playlist_ptr_filter_vector.push_back(std::make_shared<CTrack>(*track));
    }
    break;
  default:
    break;
  }
}

void CPlaylistContainer::copyFilteredToMainwindow() {
  // shortest syntax, just swapping the ptr vectors
  _playlist_ptr_filter_vector.swap(_playlist_ptr_mainwindow_vector);
  // clearing the filter ptr vector for a next search
  _playlist_ptr_filter_vector.clear();
}
