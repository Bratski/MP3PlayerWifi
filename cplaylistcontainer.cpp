#include "cplaylistcontainer.h"

const char *CPlaylistContainer::sortMethodsTXT[int(
    CPlaylistContainer::art_t::numberOfMethods)] = {
    "Random",   "by Artist", "by Album", "by Year",
    "by Genre", "undo Sort", "Title"};

void CPlaylistContainer::addTrack(CTrack &track) {
  _playlist.push_back(track);
  _playlist_ptr_mainwindow.push_back(&track);
  // update the pointer vector:
  // sortPlaylist(CPlaylistContainer::sort_t::undoSort);
}

void CPlaylistContainer::sortPlaylist(art_t wayofsorting) {
  // Initialize random number generator
  std::random_device rd; // Seed for the random number generator
  std::mt19937 rng(rd());

  // defining the sorting criteria
  // for the artists
  auto sortbyartist = [](const CTrack *tr1, const CTrack *tr2) {
    return tr1->getArtist() > tr2->getArtist();
  };

  // for the albums
  auto sortbyalbum = [](const CTrack *tr1, const CTrack *tr2) {
    return tr1->getAlbum() > tr2->getAlbum();
  };

  // for the album year
  auto sortbyyear = [](const CTrack *tr1, const CTrack *tr2) {
    return tr1->getYear() > tr2->getYear();
  };

  // for the genres
  auto sortbygenre = [](const CTrack *tr1, const CTrack *tr2) {
    return tr1->getGenre() > tr2->getGenre();
  };

  switch (wayofsorting) {
  case art_t::random:
    // sort the pointer-vector randomly
    std::shuffle(_playlist_ptr_mainwindow.begin(),
                 _playlist_ptr_mainwindow.end(), rng);
    break;

  case art_t::byArtist:
    // sort the pointer-vector by Artist name, alphabetically
    std::sort(_playlist_ptr_mainwindow.begin(), _playlist_ptr_mainwindow.end(),
              sortbyartist);
    break;
  case art_t::byAlbum:
    // sort the pointer-vector by Artist name, alphabetically
    std::sort(_playlist_ptr_mainwindow.begin(), _playlist_ptr_mainwindow.end(),
              sortbyalbum);
    break;

  case art_t::byYear:
    // sort the pointer-vector by year
    std::sort(_playlist_ptr_mainwindow.begin(), _playlist_ptr_mainwindow.end(),
              sortbyyear);
    break;
  case art_t::byGenre:
    // sort the pointer-vector by genre
    std::sort(_playlist_ptr_mainwindow.begin(), _playlist_ptr_mainwindow.end(),
              sortbygenre);
    break;
  case art_t::undoSort:
    // clear the pointer vector
    _playlist_ptr_mainwindow.clear();
    // fill the pointer vector with pointers to track objects in the _playlist
    // vector
    for (auto &track : _playlist) {
      _playlist_ptr_mainwindow.push_back(&track);
    }
    break;
  default:
    break;
  }
}

void CPlaylistContainer::filterPlaylist(art_t wayoffiltering,
                                        const QString &text) {
  // empty the search vector with pointers
  _playlist_ptr_filter.clear();
  switch (wayoffiltering) {
  case art_t::byTitle:
    for (auto &track : _playlist) {
      // is the search text corresponding to the track title, the pointer of
      // that track is added to the ptr-filter vector
      if (track.getTitle() == text) {
        _playlist_ptr_filter.push_back(&track);
      }
    }
    break;
  case art_t::byAlbum:
    for (auto &track : _playlist) {
      // same for the track album
      if (track.getAlbum() == text) {
        _playlist_ptr_filter.push_back(&track);
      }
    }
    break;
  case art_t::byArtist:
    for (auto &track : _playlist) {
      // same for the track artist
      if (track.getArtist() == text) {
        _playlist_ptr_filter.push_back(&track);
      }
    }
    break;
  case art_t::byGenre:
    for (auto &track : _playlist) {
      // same for the track genre
      if (track.getGenre() == text) {
        _playlist_ptr_filter.push_back(&track);
      }
    }
    break;
  case art_t::byYear:
    for (auto &track : _playlist) {
      // same for the track year
      if (track.getYear() == text.toInt()) {
        _playlist_ptr_filter.push_back(&track);
      }
    }
    break;
  default:
    break;
  }
}
