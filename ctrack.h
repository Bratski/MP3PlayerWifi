#ifndef CTRACK_H
#define CTRACK_H

#include <QString>

// libraries for the tag-reading from a mp3-file
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>

class CTrack {
private:
  QString _TraID;
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
  // Ctor for empty object, not necessary if default values are given
  // CTrack() = default;

  // Ctor for the sql table input
  CTrack(const QString& id = 0, const QString& title = "Unknown",
         const QString& artist = "Unknown", const QString& album = "Unknown",
         const int& year = 0, const int& number = 0,
         const QString& genre = "Unknown", const int& duration = 0,
         const int& bitrate = 0, const int& samplerate = 0,
         const int& channels = 0, const QString& filelocation = "Unknown")
      : _TraID(id), _TraTitle(title), _TraArtist(artist), _TraAlbum(album),
        _TraYear(year), _TraNumber(number), _TraGenre(genre),
        _TraDuration(duration), _TraBitrate(bitrate),
        _TraSamplerate(samplerate), _TraChannels(channels),
        _TraFileLocation(filelocation) {}

  // Method to set data from mp3-file-tag
  void setTrackData(const QString& fileloc, const QString& id);

  // All the get-methods for the attributes
  const QString& getArtist() const { return _TraArtist; }
  const QString& getAlbum() const { return _TraAlbum; }
  const QString& getGenre() const { return _TraGenre; }
  const QString& getTitle() const { return _TraTitle; }
  const QString& getFileLocation() const { return _TraFileLocation; }
  const QString& getID() const { return _TraID; }
  const int& getNumber() const { return _TraNumber; }
  const int& getDuration() const { return _TraDuration; }
  const int& getBitrate() const { return _TraBitrate; }
  const int& getSamplerate() const { return _TraSamplerate; }
  const int& getChannels() const { return _TraChannels; }
  const int& getYear() const { return _TraYear; }
};

#endif // CTRACK_H
