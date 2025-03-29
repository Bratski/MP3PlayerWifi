#ifndef CROTARYENCODERWORKER_H
#define CROTARYENCODERWORKER_H

#include <QDebug>
#include <QMutex>
#include <QObject>
// #include <QWaitCondition>
#include <gpiod.h>
#include <unistd.h>

class CRotaryEncoderWorker : public QObject {
  Q_OBJECT
public:
  explicit CRotaryEncoderWorker(QObject* parent = nullptr);
  void stop();

public slots:
  void initialize(bool* success);
  void run(bool* success);

  void disconnect();
  void setChipnumber(const int chipnumber);
  void getChipnumber(int* chipnumber);
  void setPins(const uint SWITCH, const uint CLK, const uint DT);
  void getPins(uint* pin1, uint* pin2, uint* pin3);
  void setCounter(int counter) {
    QMutexLocker locker(&m_mutex);
    _counter = counter;
  } // not working, try with Mutex, still not working??

  const int& getChipnumber() { return _chipnumber; }
  const uint& getPinSW() { return _pin1; }
  const uint& getPinCLK() { return _pin2; }
  const uint& getPinDT() { return _pin3; }

signals:
  void sendVolumeChanged(const int counter);
  void sendSwitchPressed(bool& switchstate);
  void eventLoopStopped();

private:
  uint _pin1 = 23; // SWITCH
  uint _pin2 = 17; // CLK
  uint _pin3 = 27; // DT
  const char* _chipname0 = "gpiochip0";
  const char* _chipname1 = "gpiochip1";
  const char* _chipname2 = "gpiochip2";
  const char* _chipname3 = "gpiochip3";
  const char* _chipname4 = "gpiochip4";

  int _chipnumber = 4;
  bool _switchstate = false;

  gpiod_line* _line1 = nullptr;
  gpiod_line* _line2 = nullptr;
  gpiod_line* _line3 = nullptr;
  gpiod_chip* _chip = nullptr;

  // QWaitCondition m_waitCondition; // not necessary, or even contraproductive
  // (extra overhead) because usleep is used, and it is a continous loop
  QMutex m_mutex; //
  int _counter = 0;
  bool _runRTCloop = true;
};

#endif // CROTARYENCODERWORKER_H
