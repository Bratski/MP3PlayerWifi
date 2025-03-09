#ifndef CTRACK_H
#define CTRACK_H

#include <QString>

// libraries for the tag-reading from a mp3-file
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>

class CTrack {
  int _TraID;
  QString _TraTitle;
  QString _TraArtist;
  QString _TraAlbum;
  int _TraYear;
  int _TraNumber;
  QString _TraGenre;
  int _TraDuration;
  int _TraBitrate;
  int _TraSamplerate;
  int _TraChannels;
  QString _TraFileLocation;

public:
  CTrack() = default;
  CTrack(const int &id) : _TraID(id) {}
  void setTrackData(const QString &fileloc);
  const QString &getArtist() const { return _TraArtist; }
  const QString &getAlbum() const { return _TraAlbum; }
  const QString &getGenre() const { return _TraGenre; }
  const QString &getTitle() const { return _TraTitle; }
  const int &getYear() const { return _TraYear; }
};

#endif // CTRACK_H
