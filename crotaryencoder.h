#ifndef CROTARYENCODER_H
#define CROTARYENCODER_H

#include <QDebug>
#include <QString>
#include <QThread>

#include "crotaryencoderworker.h"

class CRotaryencoder {
public:
  CRotaryencoder();
  ~CRotaryencoder();

  // initialize the rtc
  bool initialize();

  // start and stop the detection loop
  bool start();
  void stop();

  // getters and setters
  bool setChipnumber(const int& chipnumber);
  const int& getChipnumber() { return _chipnumber; }
  bool setPins(const uint& SWITCH, const uint& CLK, const uint& DT);
  const uint& getPinSWITCH() { return _pin1; }
  const uint& getPinCLK() { return _pin2; }
  const uint& getPinDT() { return _pin3; }
  CRotaryEncoderWorker* getRTCWorkerPointer() {
    return _workerrtc;
  } // needed for the emit function volumechanged
  // set the pointers to volume level and switchstate
  void getLevelAndSwitchstatePointers(int* level, bool* switchstate);

private:
  // attributes
  uint _pin1 = 23; // SWITCH
  uint _pin2 = 17; // CLK
  uint _pin3 = 27; // DT
  int _chipnumber = 0;
  bool _runRTCloop = false;
  bool _successinitialisation = false;
  bool _stopped = false;

  // pointers
  int* _level = nullptr;
  bool* _switchstate = nullptr;
  QThread* _rtcthread = nullptr;
  CRotaryEncoderWorker* _workerrtc = nullptr;
};

#endif // CROTARYENCODER_H
