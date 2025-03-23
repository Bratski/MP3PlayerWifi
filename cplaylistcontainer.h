#ifndef CPLAYLISTCONTAINER_H
#define CPLAYLISTCONTAINER_H

// for the random generator
#include <random>

// for shared pointers
#include "ctrack.h"
#include <memory>

// for vector managment
#include <QString>
#include <algorithm>
#include <vector>

// for debugging
#include <QDebug>

// for progress bar functionality
#include <QObject>

using playlist_t = std::vector<CTrack>;
using playlist_pt = std::vector<std::shared_ptr<CTrack>>;

class CPlaylistContainer : public QObject { // for progress bar functionality
  Q_OBJECT                                  // for progress bar functionality

      private
      : playlist_t _playlist_obj_vector; // the playlist with track-objects as
                                         // read from the database
  playlist_pt
      _playlist_ptr_mainwindow_vector; // a vector with pointers, pointing to
                                       // the tracks in the playlist, what is
                                       // being shown in the mainwindow table
  playlist_pt
      _playlist_ptr_filter_vector; // a vector with pointers, pointing to the
                                   // tracks in the playlist, needed for the
                                   // search and filter function
  playlist_pt
      _playlist_ptr_undosort_vector; // a vector to save the previous sorted
                                     // version, in case the user wants
                                     // to undo the current sorting method

  // saves the name of the current playlist id and name
  int _PllID;
  QString _PllName;

public:
  // to make the switch functionality more readable
  enum class art_t : u_int8_t {
    random,
    byArtist,
    byAlbum,
    byYear,
    byGenre,
    byDatabase,
    byTitle,
    undoSort,
    byMainWindow,
    numberOfMethods
  };
  CPlaylistContainer() = default;

  // to get to a track by the vector index, which correspondig to the
  // tablewidget index, operator[] overload
  const CTrack &operator[](const size_t &idx) const;
  CTrack &operator[](const size_t &idx);

  // to be able iterate through the different vectors
  auto begin() { return _playlist_obj_vector.begin(); }
  auto end() { return _playlist_obj_vector.end(); }

  auto beginPtr() { return _playlist_ptr_mainwindow_vector.begin(); }
  auto endPtr() { return _playlist_ptr_mainwindow_vector.end(); }

  auto beginFilterPtr() { return _playlist_ptr_filter_vector.begin(); }
  auto endFilterPtr() { return _playlist_ptr_filter_vector.end(); }

  // some general info stats about the playlist
  // int getNumberOfTracks() { return _playlist_obj_vector.size(); }
  int getNumberOfMainwindowTracks() {
    return _playlist_ptr_mainwindow_vector.size();
  }
  int getNumberOfFilteredTracks() { return _playlist_ptr_filter_vector.size(); }
  int calculatePlaylistTotalTime();

  // playlist operations
  void addTrack(CTrack &track);
  void removeTrack(const QString &id);
  void clear();
  void sortPlaylist(art_t wayofsorting);
  void filterPlaylist(art_t wayoffiltering, const QString &text);
  void copyFilteredToMainwindow();

  // not really necessary
  static const char
      *sortMethodsTXT[int(CPlaylistContainer::art_t::numberOfMethods)];

  // playlist setter and getter attribute name and id
  void setPllID(const int &id) { _PllID = id; }
  void setPllName(const QString &name) { _PllName = name; }
  const int &getPllID() { return _PllID; }
  const QString &getPllName() { return _PllName; }
};

#endif // CPLAYLISTCONTAINER_H
