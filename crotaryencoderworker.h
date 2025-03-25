#ifndef CROTARYENCODERWORKER_H
#define CROTARYENCODERWORKER_H

#include "crtcdriver.h"
#include <QObject>

class CRotaryEncoderWorker : public QObject {
  Q_OBJECT
public:
  explicit CRotaryEncoderWorker(QObject *parent = nullptr);

public slots:

  void initialize(uint sw, uint clk, uint dt, bool *success);
  void run() {
    _rtc.run();
    _running = true;
  }
  void stop() {
    _rtc.stop();
    _running = false;
  }
  void getRTC();

signals:
  void sendVolumeChange(const int &counter);

private:
  CRtcdriver _rtc;
  bool _running;
  int _counterbefore;
  int _counterafter;
};

#endif // CROTARYENCODERWORKER_H
