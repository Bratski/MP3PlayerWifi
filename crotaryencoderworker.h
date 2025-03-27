#ifndef CROTARYENCODERWORKER_H
#define CROTARYENCODERWORKER_H

#include <QDebug>
#include <QObject>
#include <gpiod.h>
#include <unistd.h>

class CRotaryEncoderWorker : public QObject {
  Q_OBJECT
public:
  explicit CRotaryEncoderWorker(QObject* parent = nullptr);

public slots:
  void initialize(bool* success);
  void run(bool* runRTCloop, int* level, bool* switchstate);
  void disconnect();
  void setChipnumber(const int chipnumber);
  void getChipnumber(int* chipnumber);
  void setPins(const uint SWITCH, const uint CLK, const uint DT);
  void getPins(uint* pin1, uint* pin2, uint* pin3);

signals:
  void sendVolumeChanged(int& level);
  void sendSwitchPressed(bool& switchstate);
  void eventloopstopped();

private:
  uint _pin1; // SWITCH
  uint _pin2; // CLK
  uint _pin3; // DT
  const char* _chipname = "gpiochip0";
  gpiod_line* _line1 = nullptr;
  gpiod_line* _line2 = nullptr;
  gpiod_line* _line3 = nullptr;
  gpiod_chip* _chip = nullptr;
  bool* _runRTCloop;
  int* _level;
  bool* _switchstate;
};

#endif // CROTARYENCODERWORKER_H
