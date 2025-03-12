#ifndef CPLAYLISTCONTAINER_H
#define CPLAYLISTCONTAINER_H

#include <QString>
#include <vector>

// for the random generator
#include <random>

// for shared pointers
#include "ctrack.h"
#include <memory>

// for the SQL Database management
#include <QtSql/QSqlQuery>

using playlist_t = std::vector<CTrack>;
using playlist_pt = std::vector<std::shared_ptr<CTrack>>;

class CPlaylistContainer {
  playlist_t _playlist_obj_vector; // the playlist with objects as read from the
                                   // database
  playlist_pt
      _playlist_ptr_mainwindow_vector; // a vector with pointers, pointing to
                                       // the tracks in the playlist, what is
                                       // being shown in the mainwindow table
  playlist_pt
      _playlist_ptr_filter_vector; // a vector with pointers, pointing to the
                                   // tracks in the playlist, needed for the
                                   // search and filter function
  int _PllID;
  QString _PllName;

public:
  enum class art_t : u_int8_t {
    random,
    byArtist,
    byAlbum,
    byYear,
    byGenre,
    undoSort,
    byTitle,
    numberOfMethods
  };
  CPlaylistContainer() = default;

  // make it possible to get a track by the vector index, operator[] overload
  const CTrack &operator[](const size_t &idx) const;
  CTrack &operator[](const size_t &idx);

  // making it possible to iterate through the different vectors
  auto begin() { return _playlist_obj_vector.begin(); }
  auto end() { return _playlist_obj_vector.end(); }

  auto beginPtr() { return _playlist_ptr_mainwindow_vector.begin(); }
  auto endPtr() { return _playlist_ptr_mainwindow_vector.end(); }

  // some general info stats about the playlist
  int getNumberOfTracks() { return _playlist_obj_vector.size(); }
  int calculatePlaylistTotalTime();

  void addTrack(CTrack &track);
  void clear();
  void sortPlaylist(art_t wayofsorting);
  void filterPlaylist(art_t wayoffiltering, const QString &text);
  bool fillPlaylistWithDatabaseTracks();

  static const char
      *sortMethodsTXT[int(CPlaylistContainer::art_t::numberOfMethods)];

  // playlist setter and getter attribute name and id
  void setPllID(const int &id) { _PllID = id; }
  void setPllName(const QString &name) { _PllName = name; }
  const int &getPllID() { return _PllID; }
  const QString &getPllName() { return _PllName; }
};

#endif // CPLAYLISTCONTAINER_H
