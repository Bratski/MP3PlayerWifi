#include "crotaryencoder.h"

CRotaryencoder::CRotaryencoder() {
  _workerrtc.moveToThread(&_rtcthread); // thread for rtc, not started yet, only
                                        // if it is been activated
  _rtcthread.start();
}

CRotaryencoder::~CRotaryencoder() {
  // stop the detecting event loop
  _workerrtc.stop();

  // the lines and chip are being released
  _workerrtc.disconnect();

  // stop the thread
  _rtcthread.quit();
  _rtcthread.wait();
}

bool CRotaryencoder::initialize() {
  stop();
  // if (_rtcthread && _workerrtc) {
  bool success = false;
  // _workerrtc->initialize(&success);
  // QMetaObject::invokeMethod(_workerrtc, "initialize",
  //                           Qt::BlockingQueuedConnection,
  //                           Q_ARG(bool*, &success));
  _workerrtc.initialize(&success);
  _successinitialisation = success;
  return success;
}

bool CRotaryencoder::start() {
  stop();
  if (!_successinitialisation)
    return false;
  bool success = false;
  _workerrtc.run(&success);
  // QMetaObject::invokeMethod(_workerrtc, "run", Qt::QueuedConnection,
  //                           Q_ARG(bool*, &success), Q_ARG(int*, _level),
  //                           Q_ARG(bool*, _switchstate));
  return success;
}

void CRotaryencoder::stop() { _workerrtc.stop(); }

bool CRotaryencoder::setChipnumber(const int& chipnumber) {
  stop();
  _chipnumber = chipnumber;
  // if (_rtcthread && _workerrtc) {
  _workerrtc.setChipnumber(_chipnumber);
  // QMetaObject::invokeMethod(_workerrtc, "setChipnumber",
  //                           Qt::BlockingQueuedConnection,
  //                           Q_ARG(int, _chipnumber));
  // } else
  //   return false;
  return true;
}

bool CRotaryencoder::setPins(const uint& SWITCH, const uint& CLK,
                             const uint& DT) {
  stop();
  _pin1 = SWITCH;
  _pin2 = CLK;
  _pin3 = DT;
  // if (_rtcthread && _workerrtc) {
  //   QMetaObject::invokeMethod(_workerrtc, "setPins",
  //                             Qt::BlockingQueuedConnection, Q_ARG(uint,
  //                             _pin1), Q_ARG(uint, _pin2), Q_ARG(uint,
  //                             _pin3));
  // } else
  //   return false;
  _workerrtc.setPins(_pin1, _pin2, _pin3);
  return true;
}

void CRotaryencoder::getLevelAndSwitchstatePointers(int* level,
                                                    bool* switchstate) {
  _level = level;
  _switchstate = switchstate;
}
