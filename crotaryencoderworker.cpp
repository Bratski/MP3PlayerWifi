#include "crotaryencoderworker.h"

CRotaryEncoderWorker::CRotaryEncoderWorker(QObject *parent) : QObject{parent} {}

void CRotaryEncoderWorker::initialize(bool *success) {

  // initializing the chip
  _chip = gpiod_chip_open_by_name(_chipname);

  if (!_chip) {
    qDebug() << "Error opening chip";
    *success = false;
    return;
  }

  // initializing the pins
  _line1 = gpiod_chip_get_line(_chip, _pin1);
  _line2 = gpiod_chip_get_line(_chip, _pin2);
  _line3 = gpiod_chip_get_line(_chip, _pin3);

  if (!_line3 || !_line2 || !_line1) {
    qDebug() << "Error getting line";
    gpiod_chip_close(_chip);
    *success = false;
    return;
  }
  // Request lines as inputs with pull-up for CLK and DT, Button responding on
  // rising event (low->high)
  if (gpiod_line_request_input_flags(
          _line2, "rotary-encoder", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0 ||
      gpiod_line_request_input_flags(
          _line3, "rotary-encoder", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0 ||
      gpiod_line_request_rising_edge_events(_line1, "button-monitor") < 0) {
    qDebug() << "Error requesting GPIO lines";
    gpiod_line_release(_line1);
    gpiod_line_release(_line2);
    gpiod_line_release(_line3);
    gpiod_chip_close(_chip);
    *success = false;
    return;
  }

  *success = true;
  return;
}

void CRotaryEncoderWorker::run() {
  _running = true;
  int last_clk_state = gpiod_line_get_value(_line2);
  while (_running) {

    // Read switch state
    if (gpiod_line_event_wait(_line1, nullptr) > 0) {
      gpiod_line_event event;
      if (gpiod_line_event_read(_line1, &event) == 0) {
        _switchState = (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE);
        qDebug() << "Button state: " << _switchState;
      }
    }

    // Reading CLK and DT states
    int current_clk = gpiod_line_get_value(_line2);

    if (current_clk != last_clk_state) {
      // CLK state changed, check DT pin to determine direction
      int current_dt = gpiod_line_get_value(_line3);

      if (current_clk == current_dt) {
        --_rotaryCounter;
        if (_rotaryCounter < 0) {
          _rotaryCounter = 0;
        }
        emit sendVolumeChange(_rotaryCounter);
        qDebug() << "Rotation: Counter-clockwise | Count: " << _rotaryCounter;
      } else {
        ++_rotaryCounter;
        if (_rotaryCounter > 100) {
          _rotaryCounter = 100;
        }
        emit sendVolumeChange(_rotaryCounter);
        qDebug() << "Rotation: Clockwise | Count: " << _rotaryCounter;
      }

      last_clk_state = current_clk;
    }

    // Small delay to reduce CPU usage
    usleep(1000); // 1ms
  }

  // if stopping, properly closing the chip and lines
  if (!_running) {
    gpiod_line_release(_line1);
    gpiod_line_release(_line2);
    gpiod_line_release(_line3);
    gpiod_chip_close(_chip);
  }
}

void CRotaryEncoderWorker::setPins(const uint SWITCH, const uint CLK,
                                   const uint DT) {
  this->_pin1 = SWITCH;
  this->_pin2 = CLK;
  this->_pin3 = DT;
}
