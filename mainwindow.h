#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "COled.h"
#include <QMainWindow>
#include <QSettings>
#include <dialogsettings.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, COled *oled = nullptr);
  ~MainWindow();

public slots:
  void openSettingsDialog();

private:
  Ui::MainWindow *ui;
  COled *_oled;
  DialogSettings *_dlgSettings;
};
#endif // MAINWINDOW_H
