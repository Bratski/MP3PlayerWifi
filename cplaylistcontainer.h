#ifndef CPLAYLISTCONTAINER_H
#define CPLAYLISTCONTAINER_H

#include <vector>
#include <QString>
#include "ctrack.h"

using playlist_t = std::vector<CTrack>;

class CplaylistContainer
{
    playlist_t _playlist;
    int _PllID;
    QString _PllName;

public:
    CplaylistContainer() = default;
    CplaylistContainer(const int &pllid, const QString &pllname): _PllID(pllid), _PllName(pllname) {}

    auto begin(){return _playlist.begin();}
    auto end(){return _playlist.end();}

    void addTrack(CTrack &track);


};

#endif // CPLAYLISTCONTAINER_H
