#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

// this should disable or enable qdebug output
#define QT_NO_DEBUG_OUTPUT
#include <QDebug>

#include "COled.h"
#include "crotaryencoderworker.h"

#include <QDialog>
#include <QMessageBox>
#include <QSettings>


namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog {
  Q_OBJECT

public:
  explicit DialogSettings(QWidget* parent = nullptr, COled* oled = nullptr,
                          QString* apikey = nullptr, bool* statusoled = nullptr,
                          CRotaryEncoderWorker* workerrtc = nullptr,
                          bool* statusrtc = nullptr);
  ~DialogSettings();

public slots:
  void autodetectOled();
  void initializeOled();
  void initializeRTC();
  void toggleOledButtons(bool checked);
  void toggleRTCButtons(bool checked);
  void saveSettings();
  void cancel();

private:
  Ui::DialogSettings* ui;
  COled* _oled;
  QString* _apiKey;
  bool* _statusOled;
  CRotaryEncoderWorker* _workerrtc;
  bool* _statusRTC;
  bool _optionsActivatedRTC;
  bool _optionsActivatedOLED;
  void showOledData();
  void showRTCData();
};

#endif // DIALOGSETTINGS_H
