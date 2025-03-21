#include<iostream>
#include<string>

#include"COled.h"

using namespace std;

int main()
{
    string bus = "/dev/i2c-8";
    string address = "0x3C";
    string title, album = "album", artist = "artist", time;

    // cout << "Oled bus: ";
    // getline(cin, bus);
    // cout << "Oled Address: ";
    // getline(cin, address);
   

    
    COled oled;

    oled.autodetect();
    
    //oled.run();

    cout << "Oled title: ";
    getline(cin, title);
    cout << "Oled time: ";
    getline(cin, time);

    oled.updateSong(title, album, artist);
    oled.updateTime(time);


    cout << "Oled time: ";
    getline(cin, time);

    oled.updateTime(time);


    exit(0);
}