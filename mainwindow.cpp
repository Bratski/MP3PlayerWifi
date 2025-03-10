#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO read and save settings from a config file

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio, CPlaylistContainer *playlist,
                       CTrack *track)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio), _playlist(playlist), _track(track) {
  ui->setupUi(this);

  // setting default parameters and initialize
  setWindowTitle("Bratskis MP3 Player Nitro");
  _audio->setVolume(startVolume);
  ui->horizontalSliderVolume->setRange(0, 100);
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));
  _player->setSource(QUrl::fromLocalFile(
      playThisSong)); // same time the artwork should be extracted from the
                      // mp3-file and being shown in the artwork label
  ui->progressBarSong->setFormat(timeSong);
  ui->labelTotalTime->setText(timeList);

  // connecting all the button, menu, sliders and checkbox actions to functions:
  // read the position in the song and set the slider and progress bar in the
  // corresponding positions
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
  // corresponding position in the song
  QObject::connect(
      ui->horizontalSliderSong, &QSlider::sliderMoved, ui->horizontalSliderSong,
      [this](int pos) { _player->setPosition(static_cast<qint64>(pos)); });

  // reading the volume slider
  QObject::connect(ui->horizontalSliderVolume, &QSlider::valueChanged, this,
                   &MainWindow::setVolume);

  // reading the time and display it in the progress bar
  QObject::connect(_player, &QMediaPlayer::positionChanged, ui->progressBarSong,
                   [this](qint64 timeMS) {
                     timeSong = convertMilliSecToTimeString(timeMS);
                     ui->progressBarSong->setFormat(timeSong);
                   });

  // header menu items
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

  // pushbuttons for playing songs
  QObject::connect(ui->pushButtonPlay, &QPushButton::clicked, _player,
                   &QMediaPlayer::play);
  QObject::connect(ui->pushButtonPause, &QPushButton::clicked, _player,
                   &QMediaPlayer::pause);
  QObject::connect(ui->pushButtonStop, &QPushButton::clicked, _player,
                   &QMediaPlayer::stop);
}

MainWindow::~MainWindow() {
  delete ui;

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
  _dlgSearch->exec();
}

void MainWindow::openManagementDialog() {
  _dlgManagement = new DialogManagement(this, _playlist);

  // prepare a connection, in case the management dialog is closed, the
  // tableWidgetCurrentPlaylist will be updated
  connect(_dlgManagement, &QDialog::finished, this,
          &MainWindow::refreshTableWidgetCurrentPlaylist);
  _dlgManagement->exec();
}

void MainWindow::openAddPlaylistDialog() {
  _dlgAddPlaylist = new DialogAddPlaylist(this);
  _dlgAddPlaylist->exec();
}

// setting the volume level, the audio volume must be a float between 0.0 (=no
// sound) and 1.0 (=max volume)
void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
}

void MainWindow::refreshTableWidgetCurrentPlaylist() {
  // count the number of Tracks being found
  int rowCount = _playlist->getNumberOfTracks();
  // qDebug() << "row count: " << rowCount;

  // empty the current playlist
  ui->tableWidgetCurrentPlaylist->clearContents();

  // set the table in mainwindow to the corresponding number of rows
  ui->tableWidgetCurrentPlaylist->setRowCount(rowCount);

  // populate the table with data from query
  QTableWidgetItem *item;

  int row = 0;
  for (auto it = _playlist->beginPtr(); it != _playlist->endPtr(); ++it) {
    item = new QTableWidgetItem((*it)->getTitle());
    ui->tableWidgetCurrentPlaylist->setItem(row, 0, item);

    item = new QTableWidgetItem((*it)->getArtist());
    ui->tableWidgetCurrentPlaylist->setItem(row, 1, item);

    item = new QTableWidgetItem((*it)->getAlbum());
    ui->tableWidgetCurrentPlaylist->setItem(row, 2, item);

    item = new QTableWidgetItem(QString::number((*it)->getYear()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 3, item);

    item = new QTableWidgetItem(QString::number((*it)->getNumber()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 4, item);

    item = new QTableWidgetItem((*it)->getGenre());
    ui->tableWidgetCurrentPlaylist->setItem(row, 5, item);

    item = new QTableWidgetItem(convertSecToTimeString((*it)->getDuration()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 6, item);

    item = new QTableWidgetItem(QString::number((*it)->getBitrate()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 7, item);

    item = new QTableWidgetItem(QString::number((*it)->getSamplerate()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 8, item);

    item = new QTableWidgetItem(QString::number((*it)->getChannels()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 9, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetCurrentPlaylist->resizeColumnsToContents();
  ui->tableWidgetCurrentPlaylist->setAlternatingRowColors(true);
  // ui->tableWidgetCurrentPlaylist->hideColumn(0);
}

// converts milliseconds and returns a QString displaying the time in this
// format "0:00:00"
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

// converts seconds and returns a QString displaying the time in this
// format "0:00:00"
const QString MainWindow::convertSecToTimeString(const qint64 &sec) {
  int seconds = sec % 60;
  int min = (sec / 60) % 60;
  int hr = (sec / (60 * 60));

  QString timeExHr = QString::number(min).rightJustified(2, '0') + ":" +
                     QString::number(seconds).rightJustified(2, '0');
  QString timeInHr = QString::number(hr) + ":" + timeExHr;

  if (!hr)
    return timeExHr;
  else
    return timeInHr;
}


