#include "ctrack.h"

void CTrack::setTrackData(const QString &fileloc, const QString &id) {
  // setting the file location reference and converting from QString to C-String
  TagLib::FileRef file(fileloc.toStdString().c_str());

  if (!file.isNull() && file.tag()) {
    TagLib::Tag *tag = file.tag();
    TagLib::AudioProperties *properties = file.audioProperties();

    _TraID = id;
    _TraTitle = tag->title().toCString(true); // true to be able to read utf-8 code for special characters
    _TraAlbum = tag->album().toCString(true);
    _TraArtist = tag->artist().toCString(true);
    _TraYear = tag->year();
    _TraGenre = tag->genre().toCString(true);
    _TraBitrate = properties->bitrate();
    _TraChannels = properties->channels();
    _TraDuration = properties->lengthInSeconds();
    _TraSamplerate = properties->sampleRate();
    _TraNumber = tag->track();

    _TraFileLocation = fileloc;
  }
}
