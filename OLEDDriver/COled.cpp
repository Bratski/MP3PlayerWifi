#include "COled.h"

COled::COled(const std::string &bus, const std::string &adr) : _bus(bus), _address(adr)
{
    initialize();
}

bool COled::initialize()
{
    try
    {
        _hexNumber = std::stoi(_address, nullptr, 16);
        oled = std::make_unique<SSD1306::OledI2C>(_bus, _hexNumber);
        if (!oled)
        {
            throw std::runtime_error("Failed to reinitialize OLED display");
        }
        else
        {
            updateTime(_time);
            return true;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error (re)initializing OLED: " << e.what() << '\n';
        oled = nullptr;
        return false;
    }
}

void COled::updateSong(const std::string &title, const std::string &album, const std::string &artist)
{
    if (oled)
    {
        _title = title;
        _album = album;
        _artist = artist;

        oled->clear();

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowTitle * 16},
                                _title,
                                SSD1306::PixelStyle::Set,
                                *oled);

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowAlbum * 16},
                                _album,
                                SSD1306::PixelStyle::Set,
                                *oled);

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowArtist * 16},
                                _artist,
                                SSD1306::PixelStyle::Set,
                                *oled);

        oled->displayUpdate();
    }
}

void COled::updateSong(const std::string &title, const std::string &album, const std::string &artist) const
{
    if (oled)
    {
        // _title = title;
        // _album = album;
        // _artist = artist;

        oled->clear();

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowTitle * 16},
                                title,
                                SSD1306::PixelStyle::Set,
                                *oled);

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowAlbum * 16},
                                album,
                                SSD1306::PixelStyle::Set,
                                *oled);

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowArtist * 16},
                                artist,
                                SSD1306::PixelStyle::Set,
                                *oled);

        oled->displayUpdate();
    }
}

void COled::updateTime(const std::string &time)
{
    if (oled)
    {
        _time = time;

        updateSong(_title, _album, _artist);

        SSD1306::drawString8x16(SSD1306::OledPoint{0, _rowTime * 16},
                                _time,
                                SSD1306::PixelStyle::Set,
                                *oled);

        oled->displayUpdate();
    }
}

void COled::autodetect()
{
    //_port = system("i2cdetect -l | grep -A 0 'I2C' | cut -c5");

    FILE *fp;
    char buffer[128];
    fp = popen("i2cdetect -l | grep -A 0 'CH341' | cut -c5", "r");
    while (fgets(buffer, sizeof(buffer), fp) != nullptr)
        if (isdigit(buffer[0]))
            _port = std::stoi(buffer);
    _bus = "/dev/i2c-" + std::to_string(_port);
    // std::cout << "_bus = *" << _bus << "*" << std::endl;
    // std::cout << "_address = *" << _address << "*" << std::endl;
    // system("sudo chmod a+rw /dev/i2c-"+_port);
    // COled(_bus, _address);
}

void COled::turnOff()
{
    if (oled)
    {
        oled->clear();
        oled->displayOff();
    }
}
