#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO read and save settings from a config file

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio) {
  ui->setupUi(this);

  // setting default parameters and initialize
  setWindowTitle("Bratskis MP3 Player Nitro");
  _audio->setVolume(startVolume);
  ui->horizontalSliderVolume->setRange(0, 100);
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));
  _player->setSource(QUrl::fromLocalFile(playThisSong));
  ui->progressBarSong->setFormat(timeSong);
  ui->labelTotalTime->setText(timeList);

  // filling the Table in MainWindow with database entries
  fillTableWithDatabase(defaultPlaylistName);

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
  // delete _sqlq;
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
// sound) and 1.0 (=max volume)
void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
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

void MainWindow::fillTableWithDatabase(const QString &playlistName) {
  // empty the current playlist
  ui->tableWidgetCurrentPlaylist->clearContents();
  ui->tableWidgetCurrentPlaylist->setRowCount(0);

  // find the corresponding ID in the playlist
  int playlistID = getPlaylistID(playlistName);

  // leave function if no valid ID could be found
  if (playlistID == -1) {
    qDebug() << "no valid ID could be found";
    return;
  }

  // create a query, and find in the database
  QSqlQuery query;
  query.prepare(
      "SELECT Track.TraID, Track.TraName, Track.TraNumber, Track.TraDuration, "
      "Track.TraBitrate, "
      "Track.TraSamplerate, Track.TraChannels, Track.TraFileLocation, "
      "Album.AlbName, Album.AlbYear, Artist.ArtName "
      "FROM Track "
      "JOIN Album ON Track.TraAlbFK = Album.AlbID "
      "JOIN Artist ON Album.AlbArtFK = Artist.ArtID "
      "JOIN TrackPlaylist ON Track.TraID = TrackPlaylist.TraFK "
      "JOIN Playlist ON TrackPlaylist.PllFK = Playlist.PllID "
      "WHERE Playlist.PllID = :playlistID");
  query.bindValue(":playlistID", playlistID);

  if (!query.exec())
    return;

  // count the number of Tracks being found
  int rowCount = 0;
  while (query.next())
    ++rowCount;

  // question: is it useful to put the contents of the one playlist out of the
  // database directly into the tableWidget, or is it better to put its contents
  // into a playlist object, create pointer versions of this list, in case
  // special ways of sorting are needed, and display the playlist (pointer)
  // object into the tableWidget? In other words the database is filtered
  // through several objects: object which contains the data as sorted and
  // stored in the database, other objects are filled with pointers to this
  // object, if different sortings are needed, before it is displayed or edited?
  // The database is just then updated, when a Playlist is being saved
  // explicitly, or the program is terminated, in the destructor? Or ask if the
  // current list should be saved on exit?

  // set the table in mainwindow to the corresponding number of rows
  ui->tableWidgetCurrentPlaylist->setRowCount(rowCount);

  // populate the table with data from query
  query.seek(-1); // reset query to start position
  int row = 0;
  while (query.next()) {
    for (int col = 0; col < ui->tableWidgetCurrentPlaylist->columnCount();
         ++col) {
      QTableWidgetItem *item =
          new QTableWidgetItem(query.value(col + 1).toString());
      ui->tableWidgetCurrentPlaylist->setItem(row, col, item);
    }
    ++row;
  }

  // customizing the looks
  ui->tableWidgetCurrentPlaylist->resizeColumnsToContents();
  ui->tableWidgetCurrentPlaylist->setAlternatingRowColors(true);
  // ui->tableWidgetCurrentPlaylist->hideColumn(0);
}

int MainWindow::getPlaylistID(const QString &playlistName) {
  QSqlQuery query;
  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :name");
  query.bindValue(":name", playlistName);

  if (!query.exec())
    return -1;
  if (query.next())
    return query.value(0).toInt();
  else
    return -1;
}
