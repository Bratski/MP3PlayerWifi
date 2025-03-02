#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include "COled.h"
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
  explicit DialogSettings(QWidget *parent = nullptr, COled *oled = nullptr);
  ~DialogSettings();

public slots:
  void autodetectOled();
  void initializeOled();
  void toggleOledButtons(bool checked);
  void toggleRTCButtons(bool checked);

private:
  Ui::DialogSettings *ui;
  COled *_oled;
  void showOledData();
  bool _statusOled = false;
  bool _statusRTC = false;
};

#endif // DIALOGSETTINGS_H
