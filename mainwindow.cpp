#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio) {
  ui->setupUi(this);

  // set default settings and initialze
  setWindowTitle("Bratskis MP3 Player Nitro");
  _audio->setVolume(0.5); // set start volume to 50%
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));
  _player->setSource(QUrl::fromLocalFile(playThisSong));

  // connecting all the button, menu, sliders and checkbox actions to functions:
  // read the position in the song and set the slider and progress bar in the
  // corresponding positions:
  QObject::connect(_player, &QMediaPlayer::positionChanged,
                   ui->horizontalSliderSong, [this](qint64 pos) {
                     ui->horizontalSliderSong->setRange(
                         0, static_cast<int>(_player->duration()));
                     ui->progressBarSong->setRange(
                         0, static_cast<int>(_player->duration()));
                     ui->horizontalSliderSong->setValue(static_cast<int>(pos));
                     ui->progressBarSong->setValue(static_cast<int>(pos));
                   });
  // the other way around, read the slider position and move to the
  // corresponding position in the song:
  QObject::connect(
      ui->horizontalSliderSong, &QSlider::sliderMoved, ui->horizontalSliderSong,
      [this](int pos) { _player->setPosition(static_cast<qint64>(pos)); });
  QObject::connect(ui->horizontalSliderVolume, &QSlider::valueChanged, this,
                   &MainWindow::setVolume);
  QObject::connect(_player, &QMediaPlayer::positionChanged, ui->progressBarSong,
                   [this](qint64 timeMS) {
                     QString time = convertMilliSec(timeMS);
                     ui->progressBarSong->setFormat(time);
                   });
  // header menu items:
  QObject::connect(ui->actionExit, &QAction::triggered, this,
                   &MainWindow::close);
  QObject::connect(ui->actionOled_Display, &QAction::triggered, this,
                   &MainWindow::openSettingsDialog);
  QObject::connect(ui->actionFolder, &QAction::triggered, this,
                   &MainWindow::openProgressDialog);
  QObject::connect(ui->actionSearchfilter, &QAction::triggered, this,
                   &MainWindow::openSearch);
  // pushbuttons for playing songs:
  QObject::connect(ui->pushButtonPlay, &QPushButton::clicked, _player,
                   &QMediaPlayer::play);
  QObject::connect(ui->pushButtonPause, &QPushButton::clicked, _player,
                   &QMediaPlayer::pause);
  QObject::connect(ui->pushButtonStop, &QPushButton::clicked, _player,
                   &QMediaPlayer::stop);
}

MainWindow::~MainWindow() { delete ui; }

// header windows
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

// setting the volume level:
void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
}

QString &MainWindow::convertMilliSec(qint64 &millisec) {
  int sec = (millisec / 1000) % 60;
  int min = (millisec / (60 * 1000)) % 60;
  int hr = (millisec / (60 * 60 * 1000));

  time = QString::number(hr) + ":" +
         QString::number(min).rightJustified(2, '0') + ":" +
         QString::number(sec).rightJustified(2, '0');
  return time;
}
