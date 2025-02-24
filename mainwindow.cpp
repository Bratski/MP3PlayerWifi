#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, COled *oled)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled) {
  ui->setupUi(this);
  setWindowTitle("Bratskis MP3 Player Nitro");
  QObject::connect(ui->horizontalSliderSong, SIGNAL(valueChanged(int)),
                   ui->progressBarSong, SLOT(setValue(int)));
  QObject::connect(ui->actionExit, &QAction::triggered, this,
                   &MainWindow::close);
  QObject::connect(ui->actionOled_Display, &QAction::triggered, this,
                   &MainWindow::openSettingsDialog);
}

MainWindow::~MainWindow() {
  delete ui;
  // delete _oled;
  delete _dlgSettings;
}

void MainWindow::openSettingsDialog() {
  _dlgSettings = new DialogSettings(this, _oled);
  _dlgSettings->show();
}
