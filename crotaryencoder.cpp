#include "crotaryencoder.h"

CRotaryencoder::CRotaryencoder() {
  _rtcthread = new QThread; // the thread for the rtc coder
  _workerrtc =
      new CRotaryEncoderWorker; // object containing rotary encoder operations
  _workerrtc->moveToThread(_rtcthread); // thread for rtc, not started yet, only
                                        // if it is been activated
  _rtcthread->start();
}

CRotaryencoder::~CRotaryencoder() {
  // stop the detecting event loop
  stop();

  // the lines and chip are being released
  QMetaObject::invokeMethod(_workerrtc, "disconnect",
                            Qt::BlockingQueuedConnection);

  // stop the thread
  _rtcthread->quit();
  _rtcthread->wait();

  // delete the objects in heap
  delete _workerrtc;
  delete _rtcthread;
}

bool CRotaryencoder::initialize() {
  stop();
  if (_rtcthread && _workerrtc) {
    bool success = false;
    QMetaObject::invokeMethod(_workerrtc, "initialize",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(bool*, &success));
    _successinitialisation = success;
    return success;
  }
  return false;
}

bool CRotaryencoder::start() {
  stop();
  if (!_successinitialisation)
    return false;
  _runRTCloop = true;
  QMetaObject::invokeMethod(_workerrtc, "run", Qt::QueuedConnection,
                            Q_ARG(bool*, &_runRTCloop), Q_ARG(int*, _level),
                            Q_ARG(bool*, _switchstate));
  return true;
}

void CRotaryencoder::stop() { _runRTCloop = false; }

bool CRotaryencoder::setChipnumber(const int& chipnumber) {
  stop();
  _chipnumber = chipnumber;
  _runRTCloop = false;
  if (_rtcthread && _workerrtc) {
    QMetaObject::invokeMethod(_workerrtc, "setChipnumber",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(int, _chipnumber));
  } else
    return false;
  return start();
}

bool CRotaryencoder::setPins(const uint& SWITCH, const uint& CLK,
                             const uint& DT) {
  stop();
  _pin1 = SWITCH;
  _pin2 = CLK;
  _pin3 = DT;
  _runRTCloop = false;
  if (_rtcthread && _workerrtc) {
    QMetaObject::invokeMethod(_workerrtc, "setPins",
                              Qt::BlockingQueuedConnection, Q_ARG(uint, _pin1),
                              Q_ARG(uint, _pin2), Q_ARG(uint, _pin3));
  } else
    return false;
  return start();
}

void CRotaryencoder::getLevelAndSwitchstatePointers(int* level,
                                                    bool* switchstate) {
  _level = level;
  _switchstate = switchstate;
}
