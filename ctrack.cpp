#include "ctrack.h"

void CTrack::setTrackData(const QString &fileloc) {
  // setting the file location reference and converting from QString to C-String
  TagLib::FileRef file(fileloc.toStdString().c_str());

  if (!file.isNull() && file.tag()) {
    TagLib::Tag *tag = file.tag();
    TagLib::AudioProperties *properties = file.audioProperties();

    _TraTitle = tag->title().toCString();
    _TraAlbum = tag->album().toCString();
    _TraArtist = tag->artist().toCString();
    _TraGenre = tag->genre().toCString();
    _TraBitrate = properties->bitrate();
    _TraChannels = properties->channels();
    _TraDuration = properties->lengthInSeconds();
    _TraSamplerate = properties->sampleRate();
    _TraNumber = tag->track();

    _TraFileLocation = fileloc;
  }
}
