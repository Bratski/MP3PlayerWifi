#ifndef COLED_H
#define COLED_H

#pragma once

#include <exception>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include "lib/OledFont8x16.h"
#include "lib/OledFont8x8.h"
#include "lib/OledI2C.h"

class COled
{
public:
  COled() = default;
  COled(const std::string &bus, const std::string &adr);
  ~COled() = default;
  bool initialize();
  void updateSong(const std::string &title, const std::string &album,
                  const std::string &artist);
  void updateSong(const std::string &title, const std::string &album,
                  const std::string &artist) const;
  void updateTime(const std::string &time);
  void autodetect();
  const std::string &getBus() { return _bus; }
  const std::string &getAdress() { return _address; }
  void setBus(const std::string &bus) { _bus = bus; }
  void setAdress(const std::string &adres) { _address = adres; }
  void turnOff();

private:
  std::string _bus = "/dev/i2c-3", _address = "0x3C", _title = "title",
              _album = "album", _artist = "artist", _time = "HH:MM:SS";
  int _hexNumber;
  int _rowTitle = 0, _rowAlbum = 1, _rowArtist = 2, _rowTime = 3;
  std::unique_ptr<SSD1306::OledI2C> oled = nullptr;
  int _port = 0;
};

#endif
