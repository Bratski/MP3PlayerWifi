#include "cplaylistcontainer.h"

const char *CPlaylistContainer::sortMethodsTXT[int(CPlaylistContainer::sort_t::numberOfSortMethods)]
    = {"random", "byArtist", "byAlbum", "byYear", "byGenre", "undoSort"};

void CPlaylistContainer::addTrack(CTrack &track)
{
    _playlist.push_back(track);
    _playlist_trackptr.push_back(&track);
    // update the pointer vector:
    //sortPlaylist(CPlaylistContainer::sort_t::undoSort);
}

void CPlaylistContainer::sortPlaylist(sort_t wayofsorting)
{
    // Initialize random number generator
    std::random_device rd; // Seed for the random number generator
    std::mt19937 rng(rd());

    // defining the sorting criteria for the artists
    auto sortbyartist = [](const CTrack *tr1, const CTrack *tr2) {
        return tr1->getArtist() > tr2->getArtist();
    };

    // defining the sorting criteria for the albums
    auto sortbyalbum = [](const CTrack *tr1, const CTrack *tr2) {
        return tr1->getAlbum() > tr2->getAlbum();
    };

    // defining the sorting criteria for the album year
    auto sortbyyear = [](const CTrack *tr1, const CTrack *tr2) {
        return tr1->getYear() > tr2->getYear();
    };

    // defining the sorting criteria for the genres
    auto sortbygenre = [](const CTrack *tr1, const CTrack *tr2) {
        return tr1->getGenre() > tr2->getGenre();
    };



    switch (wayofsorting) {
    case sort_t::random:
        // sort the pointer-vector randomly
        std::shuffle(_playlist_trackptr.begin(), _playlist_trackptr.end(), rng);
        break;

    case sort_t::byArtist:
        // sort the pointer-vector by Artist name, alphabetically
        std::sort(_playlist_trackptr.begin(), _playlist_trackptr.end(), sortbyartist);
    break;
  case sort_t::byAlbum:
        // sort the pointer-vector by Artist name, alphabetically
        std::sort(_playlist_trackptr.begin(), _playlist_trackptr.end(), sortbyalbum);
    break;

  case sort_t::byYear:
      // sort the pointer-vector by year
      std::sort(_playlist_trackptr.begin(), _playlist_trackptr.end(), sortbyyear);
    break;
  case sort_t::byGenre:
      // sort the pointer-vector by genre
      std::sort(_playlist_trackptr.begin(), _playlist_trackptr.end(), sortbygenre);
    break;
  case sort_t::undoSort:
    // clear the pointer vector
    _playlist_trackptr.clear();
    // fill the pointer vector with pointers to the vector with track objects
    for (auto &track : _playlist) {
      _playlist_trackptr.push_back(&track);
    }
    break;
  default:
    break;
  }
}
