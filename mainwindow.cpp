#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio) {
  ui->setupUi(this);

  // set default settings and initialze
  setWindowTitle("Bratskis MP3 Player Nitro");
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));

  // connecting all the button, menu, sliders and checkbox actions to functions:
  // sliders:
  QObject::connect(ui->horizontalSliderSong, SIGNAL(valueChanged(int)),
                   ui->progressBarSong, SLOT(setValue(int)));
  QObject::connect(ui->horizontalSliderVolume, &QSlider::valueChanged, this,
                   &MainWindow::setVolume);
  // header menu items:
  QObject::connect(ui->actionExit, &QAction::triggered, this,
                   &MainWindow::close);
  QObject::connect(ui->actionOled_Display, &QAction::triggered, this,
                   &MainWindow::openSettingsDialog);
  QObject::connect(ui->actionFolder, &QAction::triggered, this,
                   &MainWindow::openProgressDialog);
  QObject::connect(ui->actionSearchfilter, &QAction::triggered, this,
                   &MainWindow::openSearch);

  // pushbuttons:
  QObject::connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this,
                   SLOT(playSong()));
  QObject::connect(ui->pushButtonStop, SIGNAL(clicked(bool)), this,
                   SLOT(stopSong()));
}

MainWindow::~MainWindow() { delete ui; }

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

void MainWindow::playSong(QString &filelocation) {
  _player->setSource(QUrl::fromLocalFile(filelocation));
  _player->play();
}

void MainWindow::stopSong() { _player->stop(); }

void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
}
