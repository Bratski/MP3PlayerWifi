#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO read and save settings from a config file for _defaultID(last playlist)
// volume, OLED settings etc...

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio, CPlaylistContainer *playlist,
                       CTrack *track)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio), _playlist(playlist), _track(track) {
  ui->setupUi(this);

  // setting default parameters and initialize
  setWindowTitle("Bratskis MP3 Player Nitro");
  _audio->setVolume(_startVolume);
  ui->horizontalSliderVolume->setRange(0, 100);
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));
  ui->progressBarSong->setFormat(_timeSong);
  ui->labelTotalTime->setText(_timeList);
  ui->tableWidgetCurrentPlaylist->hideColumn(0);
  ui->tableWidgetCurrentPlaylist->hideColumn(11);
  ui->labelCurrentPlaylist->setText(_playlist->getPllName());
  readDataBasePlaylist();
  refreshTableWidgetCurrentPlaylist();

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
                     _timeSong = convertMilliSecToTimeString(timeMS);
                     ui->progressBarSong->setFormat(_timeSong);
                     // "timeMS % 10" reduces flickering in the Oled display
                     if (!(timeMS % 10))
                       _oled->updateTime(_timeSong.toStdString());
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

  // sort menu header
  QObject::connect(ui->actionby_Album, &QAction::triggered, this,
                   &MainWindow::sortByAlbum);
  QObject::connect(ui->actionby_Album_Year, &QAction::triggered, this,
                   &MainWindow::sortByYear);
  QObject::connect(ui->actionby_Artist, &QAction::triggered, this,
                   &MainWindow::sortByArtist);
  QObject::connect(ui->actionby_Genre, &QAction::triggered, this,
                   &MainWindow::sortByGenre);
  QObject::connect(ui->actionundo_Sort, &QAction::triggered, this,
                   &MainWindow::undoSort);
  QObject::connect(ui->actionFile, &QAction::triggered, this,
                   &MainWindow::addMusicFile);
  QObject::connect(ui->actionSave_Playlist, &QAction::triggered, this,
                   &MainWindow::saveToDatabase);

  // pushbuttons for playing songs
  QObject::connect(ui->pushButtonPlay, &QPushButton::clicked, this,
                   &MainWindow::playSongs);
  QObject::connect(ui->pushButtonPause, &QPushButton::clicked, this,
                   &MainWindow::togglePause);
  QObject::connect(ui->pushButtonStop, &QPushButton::clicked, _player,
                   &QMediaPlayer::stop);
  QObject::connect(ui->pushButtonNext, &QPushButton::clicked, this,
                   &MainWindow::playNext);
  QObject::connect(ui->pushButtonPrevious, &QPushButton::clicked, this,
                   &MainWindow::playPrevious);

  // QtableWidget events
  QObject::connect(ui->tableWidgetCurrentPlaylist,
                   &QTableWidget::itemDoubleClicked, this,
                   &MainWindow::playOneSong);

  // Checkbox events
  QObject::connect(ui->checkBoxRepeatAll, &QCheckBox::checkStateChanged, this,
                   &MainWindow::setRepeat);
  QObject::connect(ui->checkBoxPlayRandom, &QCheckBox::checkStateChanged, this,
                   &MainWindow::setRandom);

  // connect the media status changed signal to handle end of media
  connect(_player, &QMediaPlayer::mediaStatusChanged, this,
          &MainWindow::handleMediaStatusChanged);
}

MainWindow::~MainWindow() {
  closingProcedure();
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
  _dlgAddPlaylist = new DialogAddPlaylist(this, _playlist);

  // prepare a connection, in case the management dialog is closed, the
  // tableWidgetCurrentPlaylist will be updated
  connect(_dlgAddPlaylist, &QDialog::finished, this,
          &MainWindow::refreshTableWidgetCurrentPlaylist);
  _dlgAddPlaylist->exec();
}

void MainWindow::addMusicFile() {
  // open standard file browser
  QString filter = "Mp3 Files (*.mp3);; Flac Files (*.flac)";
  QString fileLocation = QFileDialog::getOpenFileName(
      this, "Open a file", qApp->applicationDirPath(),
      //"/home/bart/Nextcloud/CPlusPlusProjects/qtgui/MP3PlayerWorking",
      filter);

  // only if the string fileLocation contains data
  if (fileLocation.size()) {
    // add the track to the playlist
    CTrack newtrack;
    newtrack.setTrackData(fileLocation);
    _playlist->addTrack(newtrack);
    refreshTableWidgetCurrentPlaylist();
  } else
    QMessageBox::warning(this, "Error", "File could not be opened");
}

void MainWindow::saveToDatabase() {
  if (!_playlist->savePlaylistToDatabase()) {
    QMessageBox::warning(this, "Error",
                         "The playlist could not be saved to the database");
  }
}

void MainWindow::sortByAlbum() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byAlbum);
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByYear() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byYear);
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByArtist() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byArtist);
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByGenre() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byGenre);
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::undoSort() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::undoSort);
  refreshTableWidgetCurrentPlaylist();
}

// setting the volume level, the audio volume must be a float between 0.0 (=no
// sound) and 1.0 (=max volume)
void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
}

void MainWindow::playSongs() {
  // check if one row has been selected, if yes, which one? If not return with
  // error message
  QList<QTableWidgetItem *> selectedItems =
      ui->tableWidgetCurrentPlaylist->selectedItems();

  // if no items in the table are selected, and the playlist is not empty, start
  // with the first track
  if (selectedItems.empty() && _playlist->getNumberOfTracks() >= 1)
    _index = 0;

  // if any items are selected, pick the first row of the selected items
  if (!selectedItems.empty()) {
    _index = selectedItems[0]->row();
  }

  // pass the file location of that entry to the player source
  if (_playlist->getNumberOfTracks() >= 1)
    _playThisSong = (*_playlist)[_index].getFileLocation();

  _player->setSource(QUrl::fromLocalFile(_playThisSong));

  // start playing the song
  _player->play();

  // display the song title, samplerate and artwork in the main window info
  // output
  updateTrackInfoDisplay();
  _playall = true;
}

void MainWindow::playNext() {
  if (_playlist->getNumberOfTracks() == 0 ||
      _index == _playlist->getNumberOfTracks() - 1) {
    if (_repeat)
      _index = -1;
    else {
      _player->stop();
      return;
    }
  }
  ++_index;
  _playThisSong = (*_playlist)[_index].getFileLocation();
  _player->setSource(QUrl::fromLocalFile(_playThisSong));
  _player->play();
  updateTrackInfoDisplay();
}

void MainWindow::playPrevious() {
  if (_playlist->getNumberOfTracks() == 0 || _index == 0) {
    _player->stop();
    return;
  }
  --_index;
  _playThisSong = (*_playlist)[_index].getFileLocation();
  _player->setSource(QUrl::fromLocalFile(_playThisSong));
  _player->play();
  updateTrackInfoDisplay();
}

// making the pause button to toggle between pause and playing
void MainWindow::togglePause() {
  qint64 pos = _player->position();
  if (pos > 0)
    _player->isPlaying() ? _player->pause() : _player->play();
}

void MainWindow::playOneSong(QTableWidgetItem *item) {
  // take the row number of the selected item
  _index = item->row();

  // pass the file location of that entry to the player source
  if (_playlist->getNumberOfTracks() >= 1)
    _playThisSong = (*_playlist)[_index].getFileLocation();

  _player->setSource(QUrl::fromLocalFile(_playThisSong));

  // start playing the song
  _player->play();

  // display the song title, samplerate and artwork in the main window info
  // output
  updateTrackInfoDisplay();
  _playall = false;
}

// sort the playlist randomly, for shuffle mode
void MainWindow::setRandom(bool state) {
  if (state) {
    _playlist->sortPlaylist(CPlaylistContainer::art_t::random);
    refreshTableWidgetCurrentPlaylist();
  } else {
    _playlist->sortPlaylist(CPlaylistContainer::art_t::undoSort);
    refreshTableWidgetCurrentPlaylist();
  }
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
    item = new QTableWidgetItem(QString::number((*it)->getID()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 0, item);

    item = new QTableWidgetItem((*it)->getTitle());
    ui->tableWidgetCurrentPlaylist->setItem(row, 1, item);

    item = new QTableWidgetItem((*it)->getArtist());
    ui->tableWidgetCurrentPlaylist->setItem(row, 2, item);

    item = new QTableWidgetItem((*it)->getAlbum());
    ui->tableWidgetCurrentPlaylist->setItem(row, 3, item);

    item = new QTableWidgetItem(QString::number((*it)->getYear()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 4, item);

    item = new QTableWidgetItem(QString::number((*it)->getNumber()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 5, item);

    item = new QTableWidgetItem((*it)->getGenre());
    ui->tableWidgetCurrentPlaylist->setItem(row, 6, item);

    item = new QTableWidgetItem(convertSecToTimeString((*it)->getDuration()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 7, item);

    item = new QTableWidgetItem(QString::number((*it)->getBitrate()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 8, item);

    item = new QTableWidgetItem(QString::number((*it)->getSamplerate()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 9, item);

    item = new QTableWidgetItem(QString::number((*it)->getChannels()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 10, item);

    item = new QTableWidgetItem(((*it)->getFileLocation()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 11, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetCurrentPlaylist->resizeColumnsToContents();
  ui->tableWidgetCurrentPlaylist->setAlternatingRowColors(true);

  // set the playlist name in the main window info output
  ui->labelCurrentPlaylist->setText(_playlist->getPllName());
  ui->labelTotalTime->setText(
      convertSecToTimeString(_playlist->calculatePlaylistTotalTime()));
}

void MainWindow::updateTrackInfoDisplay() {
  // to make sure the index is not pointing to a non existing object
  if (_index >= _playlist->getNumberOfTracks() ||
      _playlist->getNumberOfTracks() == 0) {
    return;
  }
  QString title = (*_playlist)[_index].getTitle();
  QString album = (*_playlist)[_index].getAlbum();
  QString artist = (*_playlist)[_index].getArtist();

  ui->labelCurrentSong->setText(title);
  ui->labelcurrentArtist->setText(artist);
  _oled->updateSong(title.toStdString(), artist.toStdString(),
                    album.toStdString());
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
const QString MainWindow::convertSecToTimeString(const int &sec) {
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

// if a song has detected as ended, the next one is started if bool _playall is
// set true
void MainWindow::handleMediaStatusChanged(QMediaPlayer::MediaStatus status) {
  if (status == QMediaPlayer::EndOfMedia) {
    _player->stop();
    if (_playall)
      playNext();
  }
}

// open first (default) playlist in the database-table "playlist" on start up:
void MainWindow::readDataBasePlaylist() {
  QSqlQuery query;
  query.prepare("SELECT Playlist.PllID, Playlist.PllName FROM Playlist WHERE "
                "PllID = :id ");
  query.bindValue(":id", _defaultPlaylistID);

  if (!query.exec())
    return;
  if (query.next()) {
    _playlist->setPllID(query.value(0).toInt());
    _playlist->setPllName(query.value(1).toString());
    _playlist->fillPlaylistWithDatabaseTracks();
  }
}

void MainWindow::closingProcedure() {
  // on exit, asking if the current playlist should be saved
  QMessageBox msg;
  msg.addButton("Yes", QMessageBox::YesRole);
  msg.addButton("No", QMessageBox::NoRole);
  msg.setWindowTitle("Save Playlist?");
  msg.setIcon(QMessageBox::Warning);
  msg.setText("Do you want to save the current playlist?");

  // msg.exec() returns "3" if norole, "2" if yesrole
  if (msg.exec() == 3) {
    return;
  }
  if (_playlist->savePlaylistToDatabase())
    qDebug() << "playlist saved";

  // Cleaning tables
  QSqlQuery query;

  // Clean the Track table
  if (!query.exec("DELETE FROM Track WHERE TraID NOT IN (SELECT TraFK FROM "
                  "TrackPlaylist) ")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to clean Track table: " +
                              query.lastError().text());
  }

  // Clean the Album table
  if (!query.exec("DELETE FROM Album WHERE AlbID NOT IN (SELECT TraAlbFK FROM "
                  "Track) ")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to clean Album table: " +
                              query.lastError().text());
  }

  // Clean the Artist table
  if (!query.exec("DELETE FROM Artist WHERE ArtID NOT IN (SELECT AlbArtFK FROM "
                  "Album) ")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to clean Artist table: " +
                              query.lastError().text());
  }

  // closing the database
  QSqlDatabase db =
      QSqlDatabase::database(); // Get the default database connection
  if (db.isOpen()) {
    db.close(); // Close the database connection
    qDebug() << "Database connection closed";
  }
}
