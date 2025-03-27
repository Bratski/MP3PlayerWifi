#include "crotaryencoderworker.h"

CRotaryEncoderWorker::CRotaryEncoderWorker(QObject* parent) : QObject{parent} {}

void CRotaryEncoderWorker::initialize(bool* success) {

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
    qDebug() << "Error getting lines";
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
  qDebug() << "GPIO succesfully initialized";
  *success = true;
  return;
}

void CRotaryEncoderWorker::run(bool* runRTCloop, int* level,
                               bool* switchstate) {
  qDebug() << "Event detecting loop started";
  // main loop picking up the events
  int last_clk_state = gpiod_line_get_value(_line2);
  while (*runRTCloop) {
    // Read switch state
    if (gpiod_line_event_wait(_line1, nullptr) > 0) {
      gpiod_line_event event;
      if (gpiod_line_event_read(_line1, &event) == 0) {
        *switchstate = (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE);
        emit sendSwitchPressed(*_switchstate);
        qDebug() << "Button state: " << *switchstate;
      }
    }

    // Reading CLK and DT states
    int current_clk = gpiod_line_get_value(_line2);

    if (current_clk != last_clk_state) {
      // CLK state changed, check DT pin to determine direction
      int current_dt = gpiod_line_get_value(_line3);

      if (current_clk == current_dt) {
        --*level;
        if (*level < 0) {
          *level = 0;
        }
        emit sendVolumeChanged(*level);
        qDebug() << "Rotation: Counter-clockwise | Count: " << *level;
      } else {
        ++*level;
        if (*level > 100) {
          *level = 100;
        }
        emit sendVolumeChanged(*level);
        qDebug() << "Rotation: Clockwise | Count: " << *level;
      }
    }
    last_clk_state = current_clk;

    // Small delay to reduce CPU usage
    usleep(1000); // 1ms
  }
  emit eventloopstopped();
}

void CRotaryEncoderWorker::disconnect() {
  // properly closing the chip and lines, only if they are not a nullptr
  if (_line1) {
    gpiod_line_release(_line1);
    qDebug() << "Line1 successfully released";
  }
  if (_line2) {
    gpiod_line_release(_line2);
    qDebug() << "Line2 successfully released";
  }
  if (_line3) {
    gpiod_line_release(_line3);
    qDebug() << "Line3 successfully released";
  }
  if (_chip) {
    gpiod_chip_close(_chip);
    qDebug() << "Chip successfully closed";
  } else if (!_line1 && !_line2 && !_line3)
    qDebug() << "No chips or lines to be closed or released";
}

void CRotaryEncoderWorker::setChipnumber(const int chipnumber) {
  QString chipname = "gpiochip" + QString::number(chipnumber);
  _chipname = chipname.toStdString().c_str();
  qDebug() << "Chip succesfully set";
}

void CRotaryEncoderWorker::getChipnumber(int* chipnumber) {
  *chipnumber = int(std::string(_chipname).back());
}

void CRotaryEncoderWorker::setPins(const uint SWITCH, const uint CLK,
                                   const uint DT) {
  this->_pin1 = SWITCH;
  this->_pin2 = CLK;
  this->_pin3 = DT;
  qDebug() << "Pins succesfully set";
}

void CRotaryEncoderWorker::getPins(uint* pin1, uint* pin2, uint* pin3) {
  *pin1 = _pin1;
  *pin2 = _pin2;
  *pin3 = _pin3;
  qDebug() << "Pins succesfully retrieved";
}
