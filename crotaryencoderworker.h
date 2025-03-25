#ifndef CROTARYENCODERWORKER_H
#define CROTARYENCODERWORKER_H

#include <QDebug>
#include <QObject>
#include <gpiod.h>
#include <unistd.h>

class CRotaryEncoderWorker : public QObject {
  Q_OBJECT
public:
  explicit CRotaryEncoderWorker(QObject *parent = nullptr);

public slots:

  void initialize(bool *success);
  void stop() { _running = false; }
  void run();

  void getCounter(int *counter) { *counter = _rotaryCounter; }
  void getSwitchState(bool *switchstate) { *switchstate = _switchState; }

  void setPins(const uint SWITCH, const uint CLK, const uint DT);
  void setRotaryCounter(int counter) { _rotaryCounter = counter; }

  void getPinSWITCH(uint *pin1) { *pin1 = _pin1; }
  void getPinCLK(uint *pin2) { *pin2 = _pin2; }
  void getPinDT(uint *pin3) { *pin3 = _pin3; }
  void getPins(uint *pin1, uint *pin2, uint *pin3) {
    *pin1 = _pin1;
    *pin2 = _pin2;
    *pin3 = _pin3;
  }

signals:
  void sendVolumeChange(const int &counter);

private:
  uint _pin1; // SWITCH
  uint _pin2; // CLK
  uint _pin3; // DT
  int _rotaryCounter = 0;
  const char *_chipname = "gpiochip0";
  gpiod_line *_line1;
  gpiod_line *_line2;
  gpiod_line *_line3;
  gpiod_chip *_chip;
  bool _running;
  bool _switchState;
};

#endif // CROTARYENCODERWORKER_H
