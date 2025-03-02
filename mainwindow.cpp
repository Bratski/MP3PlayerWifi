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
  QObject::connect(ui->actionFolder, &QAction::triggered, this,
                   &MainWindow::openProgressDialog);
  QObject::connect(ui->actionSearchfilter, &QAction::triggered, this,
                   &MainWindow::openSearch);
  QObject::connect(ui->actionManagement, &QAction::triggered, this,
                   &MainWindow::openManagement);
}

MainWindow::~MainWindow() {
  delete ui;
  // delete _oled;
  // delete _dlgSettings;
}

void MainWindow::openSettingsDialog() {
  _dlgSettings = new DialogSettings(this, _oled);
  _dlgSettings->show();
}

void MainWindow::openProgressDialog() {
  _dlgProgess = new DialogProgress(this);
  _dlgProgess->show();
}

void MainWindow::openSearch() {
  _dlgSearch = new DialogSearch(this);
  _dlgSearch->show();
}

void MainWindow::openManagement() {
  _dlgManagement = new DialogManagement(this);
  _dlgManagement->show();
}
