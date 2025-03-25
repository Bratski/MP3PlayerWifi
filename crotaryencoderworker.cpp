#include "crotaryencoderworker.h"

CRotaryEncoderWorker::CRotaryEncoderWorker(QObject *parent) : QObject{parent} {}

void CRotaryEncoderWorker::initialize(uint sw, uint clk, uint dt,
                                      bool *success) {

  _rtc.setPins(sw, clk, dt);
  *success = _rtc.initialize();
}

void CRotaryEncoderWorker::getRTC() {
  while (_running) {
    _counterbefore = _rtc.getCounter();
    usleep(1000);
    _counterafter = _rtc.getCounter();
    if (_counterafter != _counterbefore)
      emit sendVolumeChange(_counterafter);
  }
}
