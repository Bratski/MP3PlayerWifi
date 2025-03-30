#include "crotaryencoderworker.h"

CRotaryEncoderWorker::CRotaryEncoderWorker(QObject* parent) : QObject{parent} {}

void CRotaryEncoderWorker::stop() {
  // to make the event detecting loop stop
  QMutexLocker locker(&mutex);
  _runRTCloop = false;
}

void CRotaryEncoderWorker::initialize(bool* success) {
  // first disconnect the lines, make sure they are free
  disconnect();

  switch (_chipnumber) {
  case 0:
    // initializing the chip
    _chip = gpiod_chip_open_by_name(_chipname0);
    qDebug() << "Chip set: " << _chipname0;
    break;
  case 1:
    // initializing the chip
    _chip = gpiod_chip_open_by_name(_chipname1);
    qDebug() << "Chip set: " << _chipname1;
    break;
  case 2:
    // initializing the chip
    _chip = gpiod_chip_open_by_name(_chipname2);
    qDebug() << "Chip set: " << _chipname2;
    break;
  case 3:
    // initializing the chip
    _chip = gpiod_chip_open_by_name(_chipname3);
    qDebug() << "Chip set: " << _chipname3;
    break;
  case 4:
    // initializing the chip
    _chip = gpiod_chip_open_by_name(_chipname4);
    qDebug() << "Chip set: " << _chipname4;
    break;
  default:
    // initializing the chip
    _chip = gpiod_chip_open_by_name(_chipname4);
    qDebug() << "Default chip set: " << _chipname4;
    break;
  }

  if (!_chip) {
    qDebug() << "Error opening chip";
    *success = false;
    return;
  }

  // initializing the pins
  if (!_line1)
    _line1 = gpiod_chip_get_line(_chip, _pin1);
  if (!_line2)
    _line2 = gpiod_chip_get_line(_chip, _pin2);
  if (!_line3)
    _line3 = gpiod_chip_get_line(_chip, _pin3);

  if (!_line3 || !_line2 || !_line1) {
    qDebug() << "Error getting lines";
    disconnect();
    *success = false;
    return;
  }
  // Request lines as inputs with pull-up for CLK and DT, Button responding on
  // rising event (low->high)
  if (gpiod_line_request_input_flags(
          _line2, "rotary-encoder", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0) {
    qDebug() << "Failed CLK line:" << strerror(errno);
    disconnect();
    *success = false;
    return;
  }
  if (gpiod_line_request_input_flags(
          _line3, "rotary-encoder", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0) {
    qDebug() << "Failed DT line:" << strerror(errno);
    disconnect();
    *success = false;
    return;
  }

  // Switch pin causing troubles!
  if (gpiod_line_request_rising_edge_events_flags(
          _line1, "button-monitor", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0) {
    qDebug() << "Failed SWITCH line:" << strerror(errno);
    disconnect();
    *success = false;
    return;
  }

  qDebug() << "GPIO succesfully initialized pinSW:" << _pin1
           << " pinCLK: " << _pin2 << " pin DT: " << _pin3;
  *success = true;
  return;
}

void CRotaryEncoderWorker::run(bool* success) {
  qDebug() << "Event detecting loop started";
  *success = true;
  _runRTCloop = true;
  int last_value = 1; // start with pullup high
  // main loop picking up the events
  int last_clk_state = gpiod_line_get_value(_line2);
  while (true) {
    // this should gently stop the detecting event loop
    {
      QMutexLocker locker(
          &mutex); // auto lock and unlock, what is within the scope
      if (!_runRTCloop)
        break;
    }

    // Read switch state, on falling edge
    int current_value = gpiod_line_get_value(_line1);
    if (last_value == 1 && current_value == 0) {
      qDebug() << "Button pressed";
      emit sendSwitchPressed();
    }

    // Reading CLK and DT states
    int current_clk = gpiod_line_get_value(_line2);

    if (current_clk != last_clk_state) {
      // CLK state changed, check DT pin to determine direction
      int current_dt = gpiod_line_get_value(_line3);

      if (current_clk == current_dt) {
        --_counter;
        if (_counter < 0) {
          _counter = 0;
        }
        emit sendVolumeChanged(_counter);
        qDebug() << "Rotation: Counter-clockwise | Count: " << _counter;
      } else {
        ++_counter;
        if (_counter > 100) {
          _counter = 100;
        }
        emit sendVolumeChanged(_counter);
        qDebug() << "Rotation: Clockwise | Count: " << _counter;
      }
      last_clk_state = current_clk;
    }
    last_value = current_value;
    // Small delay to reduce CPU usage
    usleep(1000); // 1ms
  }
  emit eventLoopStopped();
}

void CRotaryEncoderWorker::disconnect() {
  // properly closing the chip and lines, only if they are not a nullptr
  if (_line1) {
    gpiod_line_release(_line1);
    _line1 = nullptr;
    qDebug() << "Line1 successfully released";
  }
  if (_line2) {
    gpiod_line_release(_line2);
    _line2 = nullptr;
    qDebug() << "Line2 successfully released";
  }
  if (_line3) {
    gpiod_line_release(_line3);
    _line3 = nullptr;
    qDebug() << "Line3 successfully released";
  }
  if (_chip) {
    gpiod_chip_close(_chip);
    _chip = nullptr;
    qDebug() << "Chip successfully closed";
  } else if (!_line1 && !_line2 && !_line3)
    qDebug() << "No lines to be released";
}

void CRotaryEncoderWorker::setChipnumber(const int chipnumber) {
  _chipnumber = chipnumber;
  qDebug() << "Chipnumber succesfully set";
}

void CRotaryEncoderWorker::getChipnumber(int* chipnumber) {
  *chipnumber = _chipnumber;
}

void CRotaryEncoderWorker::setPins(const uint SWITCH, const uint CLK,
                                   const uint DT) {
  this->_pin1 = SWITCH;
  this->_pin2 = CLK;
  this->_pin3 = DT;
  qDebug() << "Pins succesfully set: SWITCH: " << _pin1 << " CLK: " << _pin2
           << " DT: " << _pin3;
}

void CRotaryEncoderWorker::getPins(uint* pin1, uint* pin2, uint* pin3) {
  *pin1 = _pin1;
  *pin2 = _pin2;
  *pin3 = _pin3;
  qDebug() << "Pins succesfully retrieved";
}
