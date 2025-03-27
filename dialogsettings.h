#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include "COled.h"
#include "crotaryencoder.h"

#include <QDialog>
#include <QMessageBox>
#include <QSettings>

// TODO: save and read settings from a config file

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog {
  Q_OBJECT

public:
  explicit DialogSettings(QWidget* parent = nullptr, COled* oled = nullptr,
                          QString* apikey = nullptr, bool* statusoled = nullptr,
                          CRotaryencoder* rtc = nullptr,
                          bool* statusrtc = nullptr);
  ~DialogSettings();

public slots:
  void autodetectOled();
  void initializeOled();
  void initializeRTC();
  void toggleOledButtons(bool checked);
  void toggleRTCButtons(bool checked);
  void saveSettings();

private:
  Ui::DialogSettings* ui;
  COled* _oled;
  QString* _apiKey;
  bool* _statusOled;
  CRotaryencoder* _rtc;
  bool* _statusRTC;

  int _chipNUMBER = 0;
  uint _pinSW = 23;
  uint _pinCLK = 17;
  uint _pinDT = 27;

  void showOledData();
  void showRTCData();
};

#endif // DIALOGSETTINGS_H
