#ifndef RTCDRIVER_H
#define RTCDRIVER_H

#pragma once
#include <gpiod.h>
#include <iostream>
#include <unistd.h>
#include <chrono>

class rtcdriver
{
public:
    rtcdriver() = default;
    rtcdriver(const unsigned int &SWITCH, const unsigned int &CLK, const unsigned int &DT);
    ~rtcdriver();

    bool initialize();
    void stop() { _running = false; }
    void run();

    const int &getCounter() { return _rotaryCounter; }
    bool getSwitchState() { return _switchState; }

    void setPins(const unsigned int &SWITCH, const unsigned int &CLK, const unsigned int &DT);
    void setRotaryCounter(int &counter) { _rotaryCounter = counter; }

    const unsigned int &getPinSWITCH() { return _pin1; }
    const unsigned int &getPinCLK() { return _pin2; }
    const unsigned int &getPinDT() { return _pin3; }

private:
    unsigned int _pin1; // SWITCH
    unsigned int _pin2; // CLK
    unsigned int _pin3; // DT
    int _rotaryCounter = 0;
    const char *_chipname = "gpiochip0";
    gpiod_line *_line1;
    gpiod_line *_line2;
    gpiod_line *_line3;
    gpiod_chip *_chip;
    bool _running;
    bool _switchState;
};

#endif