#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO read and save settings from a config file for _defaultID(last playlist)
// volume, OLED settings etc...

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio, CPlaylistContainer *playlist,
                       CTrack *track, QThread *dbthread,
                       CDatabaseWorker *worker)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio), _playlist(playlist), _track(track), _dbthread(dbthread),
      _worker(worker) {
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
  QObject::connect(ui->actionSearchfilter, &QAction::triggered, this,
                   &MainWindow::openSearchDialog);
  QObject::connect(ui->actionManagement, &QAction::triggered, this,
                   &MainWindow::openManagementDialog);
  QObject::connect(ui->actionOther_Playlist, &QAction::triggered, this,
                   &MainWindow::openAddPlaylistDialog);
  QObject::connect(ui->actionAddFile, &QAction::triggered, this,
                   &MainWindow::addMusicFile);
  QObject::connect(ui->actionAddFolder, &QAction::triggered, this,
                   &MainWindow::addMusicFolder);
  QObject::connect(ui->actionSave_Playlist, &QAction::triggered, this,
                   &MainWindow::saveToDatabase);
  QObject::connect(ui->actionDeleteTrack, &QAction::triggered, this,
                   &MainWindow::deleteTrack);
  QObject::connect(ui->actionDeletePlaylist, &QAction::triggered, this,
                   &MainWindow::deletePlaylist);

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

  // pushbuttons for playing songs
  QObject::connect(ui->pushButtonPlay, &QPushButton::clicked, this,
                   &MainWindow::playSongs);
  QObject::connect(ui->pushButtonPause, &QPushButton::clicked, this,
                   &MainWindow::togglePause);
  QObject::connect(ui->pushButtonStop, &QPushButton::clicked, this,
                   &MainWindow::stopPlaying);
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

void MainWindow::openSearchDialog() {
  _dlgSearch = new DialogSearch(this);
  _dlgSearch->exec();
}

void MainWindow::openManagementDialog() {
  _dlgManagement = new DialogManagement(this, _playlist, _dbthread, _worker);

  // prepare a connection, in case the management dialog is closed, the
  // tableWidgetCurrentPlaylist will be updated
  connect(_dlgManagement, &QDialog::finished, this,
          &MainWindow::refreshTableWidgetCurrentPlaylist);
  _dlgManagement->exec();
}

void MainWindow::openAddPlaylistDialog() {
  _dlgAddPlaylist =
      new DialogAddPlaylist(this, _playlist, _worker, &_playlistChanged);

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
    // to make shure the track id is unique, a character is added to the
    // incrementing number, to identify its origin: "F" from file, "D" from
    // database
    ++_trackID;
    QString id = "F" + QString::number(_trackID); // to identify its origin from
                                                  // FILE added to the database
    newtrack.setTrackData(fileLocation, id);
    _playlist->addTrack(newtrack);
    _playlistChanged = true;
    refreshTableWidgetCurrentPlaylist();
  } else
    QMessageBox::warning(this, "Error", "File could not be opened");
}

void MainWindow::addMusicFolder() {
  // open standard file browser, and get a selected directory
  QString folder = QFileDialog::getExistingDirectory(
      this, "Open folder", qApp->applicationDirPath());

  // only if the folder contains data
  if (folder.size()) {
    // process the folder, how to navigate through the directory and its
    // subdirectories?

    // to be sure the vector with music files is empty
    _detectedMusicFiles.clear();

    processFolder(folder); // a recursive function, filling up the vector
                           // _detectedMusicFiles

    // only if the vector is not empty, the tracks can be added to the current
    // playlist
    if (!_detectedMusicFiles.empty()) {
      // inserting all the detected files in the playlist
      for (const auto &filepath : _detectedMusicFiles) {
        CTrack newtrack;
        // to make shure the track id is unique, a character is added to the
        // incrementing number, to identify its origin: "F" from file, "D" from
        // database
        ++_trackID;
        QString id =
            "F" + QString::number(_trackID); // to identify its origin from
                                             // FILE added to the database
        newtrack.setTrackData(filepath, id);
        _playlist->addTrack(newtrack);
        _playlistChanged = true;
      }
      refreshTableWidgetCurrentPlaylist();
    }
  } else
    QMessageBox::warning(this, "Error", "Files could not be opened");
}

void MainWindow::saveToDatabase() {
  // display the prorgress bar
  _dlgProgess = new DialogProgress(this, _playlist);
  _dlgProgess->show();

  // Connect signals from the database thread to the progress dialog
  connect(_worker, &CDatabaseWorker::sendProgress, _dlgProgess,
          &DialogProgress::receiveProgress);
  connect(_worker, &CDatabaseWorker::progressReady, _dlgProgess,
          &DialogProgress::close);

  // Create an event loop to block until the database operation is done,
  // necessary to display the progressbar properly
  QEventLoop loop;
  connect(_worker, &CDatabaseWorker::progressReady, &loop, &QEventLoop::quit);

  // start the database thread operation
  bool success = false;
  QMetaObject::invokeMethod(_worker, "writePlaylistTracksToDatabase",
                            Qt::QueuedConnection, _playlist, &success);

  // Block until the database operation is complete, necessary to display the
  // progressbar properly
  loop.exec();

  // set the bool playlist, in case the files have been successfully saved in
  // the database
  if (success)
    _playlistChanged = false;
}

void MainWindow::deleteTrack() {
  qDebug() << "number of tracks in the list: "
           << _playlist->getNumberOfTracks();

  // create a list of the selected range
  const QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetCurrentPlaylist->selectedRanges();

  // if no rows are selected leave the deleteTrack function
  if (!selectedRanges.size())
    return;

  // iterate through the range
  for (const auto &range : selectedRanges) {
    // iterate through the rows in one range
    for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
      // get the item with the song id from that row
      QTableWidgetItem *idItem =
          ui->tableWidgetCurrentPlaylist->item(row, 0); // Column 0 (ID)

      // Get the song ID
      QString id = idItem->text();

      qDebug() << "id: " << id;

      // delete that song from the playlist vector
      _playlist->removeTrack(id);
      _playlistChanged = true;
    }
  }

  // refresh the table
  qDebug() << "number of tracks in the list: "
           << _playlist->getNumberOfTracks();
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::deletePlaylist() {
  _playlist->clear();
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByAlbum() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byAlbum);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByYear() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byYear);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByArtist() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byArtist);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
}

void MainWindow::sortByGenre() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byGenre);
  _playlistChanged = true;
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

  // if the playlist is empty do nothing
  if (_playlist->getNumberOfTracks() == 0)
    return;

  // check if one row has been selected, if yes, which one?

  QList<QTableWidgetItem *> selectedItems =
      ui->tableWidgetCurrentPlaylist->selectedItems();

  // if no items in the table are selected, and the playlist is not empty,
  // start with the first track
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
  // check if playlist is empty or the player is playing
  if (_playlist->getNumberOfTracks() == 0 || !_player->isPlaying())
    return;

  // check if the index is pointing to the last song
  if (_index >= _playlist->getNumberOfTracks() - 1) {
    // if repeat all is enabled, start at the beginning, put the index to 0,
    // which occurs at the ++_index;
    if (_repeat)
      _index = -1;
    // if repeat all is disabled, stop playing and stop going to play the next
    // song, reset the index to 0
    else {
      _player->stop();
      _index = 0;
      return;
    }
  }
  // set the index to the next song
  ++_index;

  // get the filelocation at that index
  _playThisSong = (*_playlist)[_index].getFileLocation();
  // set the player to play that file
  _player->setSource(QUrl::fromLocalFile(_playThisSong));
  // starts playing the song
  _player->play();
  // updates the song information output to user
  updateTrackInfoDisplay();
}

void MainWindow::playPrevious() {
  // check if the playlist is empty or the player is not playing or the index
  // is already set to the first song in the list
  if (_playlist->getNumberOfTracks() == 0 || !_player->isPlaying() ||
      _index <= 0) {
    return;
  }

  // as none of them is the case, than the player can switch to the previous
  // song
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

void MainWindow::stopPlaying() {
  // stop the player
  _player->stop();

  // clear the infos of the current song in the display
  ui->labelCurrentSong->clear();
  ui->labelArtWork->clear();
  ui->labelcurrentArtist->clear();
  _oled->turnOff();

  // set the index back to 0
  _index = 0;
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

  // populate the table with data from the track vector (playlist)
  QTableWidgetItem *item;

  int row = 0;
  for (auto it = _playlist->beginPtr(); it != _playlist->endPtr(); ++it) {
    item = new QTableWidgetItem((*it)->getID());
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

// if a song has detected as ended, the next one is started if bool _playall
// is set true
void MainWindow::handleMediaStatusChanged(QMediaPlayer::MediaStatus status) {
  if (status == QMediaPlayer::EndOfMedia) {
    _player->stop();
    if (_playall)
      playNext();
  }
}

// open first (default) playlist in the database-table "playlist" on start up:
void MainWindow::readDataBasePlaylist() {
  bool success = false;
  QMetaObject::invokeMethod(_worker, "readDataBasePlaylist",
                            Qt::BlockingQueuedConnection, _playlist,
                            _defaultPlaylistID, &success);
  if (!success)
    qDebug() << "Something went wrong reading the database";
}

void MainWindow::closingProcedure() {
  // on exit, asking if the current playlist should be saved
  QMessageBox msg;
  msg.addButton("Yes", QMessageBox::YesRole);
  msg.addButton("No", QMessageBox::NoRole);
  msg.setWindowTitle("Save Playlist?");
  msg.setIcon(QMessageBox::Warning);
  msg.setText("Do you want to save the changes to the playlist?");

  // only ask to save the playlist to database, if the playlist has been
  // edited (_playlistChanged is set true)

  if (_playlistChanged) {
    // msg.exec() returns "3" if norole, "2" if yesrole
    if (msg.exec() == 2) {
      saveToDatabase();
    }
  }

  // stop playing
  stopPlaying();

  bool success = false;
  // cleaning up the database
  QMetaObject::invokeMethod(_worker, "cleanupDatabase",
                            Qt::BlockingQueuedConnection, &success);
  if (success)
    qDebug() << "Database clean up was successfull";

  // close database
  QMetaObject::invokeMethod(_worker, "closeDatabase",
                            Qt::BlockingQueuedConnection);

  // stop the thread for the database
  _dbthread->quit();
  _dbthread->wait();
}

// a recursive function to go through all the subdirectories and collect all
// the music files
void MainWindow::processFolder(const QString &path) {
  QDir dir(path);
  if (!dir.exists()) {
    qDebug() << "Directory does not exist:" << path;
    return;
  }

  // Get all files and directories in the current directory
  const QFileInfoList entries =
      dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

  for (const QFileInfo &entry : entries) {
    if (entry.isDir()) {
      // Recursively process subdirectories
      processFolder(entry.absoluteFilePath());
    } else if (entry.isFile()) {
      QString filefound = entry.absoluteFilePath();
      QString fileExtension4 = filefound.right(
          4); // get the last 4 characters of the filefound string
      QString fileExtension5 = filefound.right(
          5); // get the last 5 characters of the filefound string
      // Process files, only add the file path to the vector
      // _detectedMusicFiles, if it has a .mp3 or .flac extension
      if (fileExtension4 == ".mp3" || fileExtension5 == ".flac") {
        qDebug() << "File:" << entry.absoluteFilePath();
        _detectedMusicFiles.push_back(filefound);
      }
    }
  }
}
