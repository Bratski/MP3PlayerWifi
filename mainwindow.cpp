#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO read and save settings from a config file

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio) {
  ui->setupUi(this);

  // setting default parameters and initialize:
  setWindowTitle("Bratskis MP3 Player Nitro");
  _audio->setVolume(startVolume);
  ui->horizontalSliderVolume->setRange(0, 100);
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));
  _player->setSource(QUrl::fromLocalFile(playThisSong));
  ui->progressBarSong->setFormat(timeSong);
  ui->labelTotalTime->setText(timeList);
  //_sqlq = new QSqlQueryModel(); // readonly
  _sqlq = new QSqlTableModel();   // read and write
  _sqlq->setQuery(fill_SqlqWith); // fill _sqlq with the database data, as
                                  // described in the Qstring fill_SqlqWith
  // setting the _sqlq headers:
  _sqlq->setHeaderData(0, Qt::Horizontal, "Track ID");
  _sqlq->setHeaderData(1, Qt::Horizontal, "Title");
  _sqlq->setHeaderData(2, Qt::Horizontal, "Artist");
  _sqlq->setHeaderData(3, Qt::Horizontal, "Album");
  _sqlq->setHeaderData(4, Qt::Horizontal, "Year");
  _sqlq->setHeaderData(5, Qt::Horizontal, "Number");
  _sqlq->setHeaderData(6, Qt::Horizontal, "Genre");
  _sqlq->setHeaderData(7, Qt::Horizontal, "Duration");
  _sqlq->setHeaderData(8, Qt::Horizontal, "Bitrate");
  _sqlq->setHeaderData(9, Qt::Horizontal, "Samplerate");
  _sqlq->setHeaderData(10, Qt::Horizontal, "Channels");
  _sqlq->setHeaderData(11, Qt::Horizontal, "Playlist ID");
  _sqlq->setHeaderData(12, Qt::Horizontal, "Playlist Name");

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

  // reading the volume slider:
  QObject::connect(ui->horizontalSliderVolume, &QSlider::valueChanged, this,
                   &MainWindow::setVolume);

  // reading the time and display it in the progress bar:
  QObject::connect(_player, &QMediaPlayer::positionChanged, ui->progressBarSong,
                   [this](qint64 timeMS) {
                     timeSong = convertMilliSecToTimeString(timeMS);
                     ui->progressBarSong->setFormat(timeSong);
                   });

  // header menu items:
  QObject::connect(ui->actionExit, &QAction::triggered, this,
                   &MainWindow::close);
  QObject::connect(ui->actionOled_Display, &QAction::triggered, this,
                   &MainWindow::openSettingsDialog);
  QObject::connect(ui->actionFolder, &QAction::triggered, this,
                   &MainWindow::openProgressDialog);
  QObject::connect(ui->actionSearchfilter, &QAction::triggered, this,
                   &MainWindow::openSearchDialog);
  QObject::connect(ui->actionManagement, &QAction::triggered, this,
                   &MainWindow::openManagementDialog);
  QObject::connect(ui->actionOther_Playlist, &QAction::triggered, this,
                   &MainWindow::openAddPlaylistDialog);

  // pushbuttons for playing songs:
  QObject::connect(ui->pushButtonPlay, &QPushButton::clicked, _player,
                   &QMediaPlayer::play);
  QObject::connect(ui->pushButtonPause, &QPushButton::clicked, _player,
                   &QMediaPlayer::pause);
  QObject::connect(ui->pushButtonStop, &QPushButton::clicked, _player,
                   &QMediaPlayer::stop);
}

MainWindow::~MainWindow() {
  delete ui;
  delete _sqlq;
}

// header windows, the Dialog object pointed to by _dlgxxx is deleted
// automatically when its parent object (the one referred to by this) is
// destroyed.
void MainWindow::openSettingsDialog() {
  _dlgSettings = new DialogSettings(this, _oled);
  _dlgSettings->show();
}

void MainWindow::openProgressDialog() {
  _dlgProgess = new DialogProgress(this);
  _dlgProgess->show();
}

void MainWindow::openSearchDialog() {
  _dlgSearch = new DialogSearch(this);
  _dlgSearch->show();
}

void MainWindow::openManagementDialog() {
  _dlgManagement = new DialogManagement(this);
  _dlgManagement->show();
}

void MainWindow::openAddPlaylistDialog() {
  _dlgAddPlaylist = new DialogAddPlaylist(this);
  _dlgAddPlaylist->show();
}

// setting the volume level, the audio volume must be a float between 0.0 (=no
// sound) and 1.0 (=max volume):
void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
}

// converts milliseconds and returns a QString displaying the time in this
// format "0:00:00":
const QString MainWindow::convertMilliSecToTimeString(const qint64 &millisec) {
  int sec = (millisec / 1000) % 60;
  int min = (millisec / (60 * 1000)) % 60;
  int hr = (millisec / (60 * 60 * 1000));

  QString timeExHr = QString::number(min).rightJustified(2, '0') + ":" +
                     QString::number(sec).rightJustified(2, '0');
  QString timeInHr = QString::number(hr) + ":" + timeExHr;

  if (!hr)
    return timeExHr;
  else
    return timeInHr;
}
