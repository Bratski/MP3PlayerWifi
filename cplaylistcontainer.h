#ifndef CPLAYLISTCONTAINER_H
#define CPLAYLISTCONTAINER_H

#include <vector>
#include <QString>

// for the random generator
#include <random>

#include "ctrack.h"

using playlist_t = std::vector<CTrack>;
using playlist_pt = std::vector<CTrack *>;

class CPlaylistContainer
{
    playlist_t _playlist; // the playlist with objects as read from the database
    playlist_pt _playlist_ptr_mainwindow; // a vector with pointers, pointing to the tracks in the playlist, what is being shown in the mainwindow table
    playlist_pt _playlist_ptr_filter; // a vector with pointers, pointing to the tracks in the playlist, needed for the search and filter function
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
    CPlaylistContainer(const int &pllid, const QString &pllname): _PllID(pllid), _PllName(pllname) {}

    auto begin(){return _playlist_ptr_mainwindow.begin();}
    auto end(){return _playlist_ptr_mainwindow.end();}

    void addTrack(CTrack &track);

    void sortPlaylist(art_t wayofsorting);
    void filterPlaylist(art_t wayoffiltering, const QString &text);

    static const char *sortMethodsTXT[int(CPlaylistContainer::art_t::numberOfMethods)];


};

#endif // CPLAYLISTCONTAINER_H
