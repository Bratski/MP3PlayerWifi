#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO read and save settings from a config file for _defaultID(last playlist)
// volume, OLED settings, API key etc...

// TODO undosort after a filter operation

// TODO search and filter possible with partial text orders

MainWindow::MainWindow(QWidget *parent, COled *oled, QMediaPlayer *player,
                       QAudioOutput *audio, CPlaylistContainer *playlist,
                       CTrack *track, QThread *dbthread,
                       CDatabaseWorker *worker)
    : QMainWindow(parent), ui(new Ui::MainWindow), _oled(oled), _player(player),
      _audio(audio), _playlist(playlist), _track(track), _dbthread(dbthread),
      _worker(worker) {
  ui->setupUi(this);

  // setting default parameters and initialize
  _network = new QNetworkAccessManager();
  setWindowTitle("Bratskis MP3 Player Nitro");
  initializeSettings();
  loadSettings();
  if (_statusOled)
    _statusOled = _oled->initialize(); // in case the initialisation failed,
                                       // further settings must be disabled
  _audio->setVolume(_startVolume);
  ui->horizontalSliderVolume->setRange(0, 100);
  ui->horizontalSliderVolume->setSliderPosition(
      static_cast<int>(_audio->volume() * 100));
  ui->progressBarSong->setFormat(_timeSong);
  ui->labelTotalTime->setText(_timeList);
  ui->tableWidgetCurrentPlaylist->hideColumn(0);
  ui->tableWidgetCurrentPlaylist->hideColumn(11);
  ui->tableWidgetCurrentPlaylist->setColumnWidth(1, 300);
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
                     // Only change time if the time has changed, to reduce
                     // flickering in the Oled display
                     if (_timeSong != _timePrevious)
                       _oled->updateTime(_timeSong.toStdString());
                     _timePrevious = _timeSong;
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
  QObject::connect(ui->actionby_Database, &QAction::triggered, this,
                   &MainWindow::sortByDatabase);
  QObject::connect(ui->actionby_Genre, &QAction::triggered, this,
                   &MainWindow::sortByGenre);
  QObject::connect(ui->actionRandom, &QAction::triggered, this,
                   &MainWindow::sortRandom);
  QObject::connect(ui->actionundo_Sort, &QAction::triggered, this,
                   &MainWindow::undoSort);

  // pushbuttons for playing songs
  QObject::connect(ui->pushButtonPlay, &QPushButton::clicked, this,
                   &MainWindow::playAllSongs);
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
  QObject::connect(ui->checkBoxPlayShuffle, &QCheckBox::checkStateChanged, this,
                   &MainWindow::setShuffle);

  // connect the media status changed signal to handle end of media
  connect(_player, &QMediaPlayer::mediaStatusChanged, this,
          &MainWindow::handleMediaStatusChanged);

  // connect response after a network request is completed
  connect(_network, &QNetworkAccessManager::finished, this,
          &MainWindow::getDataFromNetwork);

  // check which audio files are supported
  // QMediaFormat mediaFormat;
  // const QList<QMediaFormat::AudioCodec> supportedAudioCodecs =
  //     mediaFormat.supportedAudioCodecs(QMediaFormat::Decode);
  // for (auto &codec : supportedAudioCodecs) {
  //   qDebug() << codec;
  // }
}

MainWindow::~MainWindow() {
  // a couple of procedures are in this function put together
  closingProcedure();
  delete ui;
  delete _network;
}

// header windows, the Dialog object pointed to by _dlgxxx is deleted
// automatically when its parent object (the one referred to by this) is
// destroyed.
void MainWindow::openSettingsDialog() {
  _dlgSettings = new DialogSettings(this, _oled, &_apiKey, &_statusOled);
  _dlgSettings->show();
}

void MainWindow::openSearchDialog() {
  _dlgSearch = new DialogSearch(this, _playlist, &_playlistChanged);

  // prepare a connection, in case the management dialog is closed, the
  // tableWidgetCurrentPlaylist will be updated
  connect(_dlgSearch, &QDialog::finished, this,
          &MainWindow::refreshTableWidgetCurrentPlaylist);
  _dlgSearch->exec();
}

void MainWindow::openManagementDialog() {
  _dlgManagement =
      new DialogManagement(this, _playlist, _worker, &_playlistChanged);

  // prepare a connection, in case the management dialog is closed, the
  // tableWidgetCurrentPlaylist will be updated
  connect(_dlgManagement, &DialogManagement::saveToDBMainWindow, this,
          &MainWindow::saveToDatabase);
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
  QString filter = "Mp3 Files (*.mp3);; Flac Files (*.flac) ;; WMA Files "
                   "(*.wma) ;; Wave Files (*.wav) ;; AAC Files (*.aac) ;; AC3 "
                   "Files (*.ac3) ;; EAC3 (*.eac3) ;; ALAC Files (*.alac)";
  QString fileLocation = QFileDialog::getOpenFileName(
      this, "Open a file", qApp->applicationDirPath(),
      //"/home/bart/Nextcloud/CPlusPlusProjects/qtgui/MP3PlayerWorking",
      filter);

  // only if the string fileLocation contains data
  if (fileLocation.size()) {
    // add the track to the playlist
    CTrack newtrack;
    // to make sure the track id is unique, a character is added to the
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

        // Instantiate a new track object
        CTrack newtrack;
        // to make sure the track id is unique, a character is added to the
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
  // display the progress bar
  _dlgProgess = new DialogProgress(this, _playlist, _dbthread, &_cancelSaving);
  _dlgProgess->open();
  _cancelSaving = false;

  // Connect signals from the database thread to the progress dialog
  connect(_worker, &CDatabaseWorker::sendProgress, _dlgProgess,
          &DialogProgress::receiveProgress);
  connect(_worker, &CDatabaseWorker::progressReady, _dlgProgess,
          &DialogProgress::allowClose);

  // Create an event loop to block until the database operation is done,
  // necessary to display the progressbar properly
  QEventLoop loop;
  connect(_worker, &CDatabaseWorker::progressReady, &loop, &QEventLoop::quit);

  // start the database thread operation
  bool success = false;
  QMetaObject::invokeMethod(_worker, "writePlaylistTracksToDatabase",
                            Qt::QueuedConnection, _playlist, &success,
                            &_cancelSaving);

  // Block until the database operation is complete, necessary to display the
  // progressbar properly
  loop.exec();

  if (_cancelSaving)
    QMessageBox::warning(this, "Warning",
                         "Saving playlist to database has NOT completed!!");

  // set the bool playlist, in case the files have been successfully saved in
  // the database
  if (success)
    _playlistChanged = false;
}

void MainWindow::deleteTrack() {
  // qDebug() << "number of tracks in the list: "
  //         << _playlist->getNumberOfMainwindowTracks();

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

      // qDebug() << "id: " << id;

      // delete that song from the playlist vector
      _playlist->removeTrack(id);
      _playlistChanged = true;
    }
  }

  // refresh the table
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::deletePlaylist() {
  stopPlaying();
  _playlist->clear();
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::sortByAlbum() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byAlbum);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::sortByYear() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byYear);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::sortByArtist() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byArtist);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::sortByDatabase() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byDatabase);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::sortByGenre() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::byGenre);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::sortRandom() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::random);
  _playlistChanged = true;
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

void MainWindow::undoSort() {
  _playlist->sortPlaylist(CPlaylistContainer::art_t::undoSort);
  refreshTableWidgetCurrentPlaylist();
  setItemBackgroundColour();
}

// setting the volume level, the audio volume must be a float between 0.0 (=no
// sound) and 1.0 (=max volume)
void MainWindow::setVolume(int level) {
  float audioLevel = static_cast<float>(level) / 100.0f;
  _audio->setVolume(audioLevel);
}

void MainWindow::playAllSongs() {

  // if the playlist is empty do nothing
  if (_playlist->getNumberOfMainwindowTracks() == 0)
    return;

  // check if one row has been selected, if yes, which one?
  QList<QTableWidgetItem *> selectedItems =
      ui->tableWidgetCurrentPlaylist->selectedItems();

  // if no items in the table are selected, and the playlist is not empty,
  // start with the first track
  if (selectedItems.empty() && _playlist->getNumberOfMainwindowTracks() >= 1)
    _index = 0;

  // if any items are selected, pick the first row of the selected items
  if (!selectedItems.empty()) {
    _index = selectedItems[0]->row();
  }
  // bool to continue to the next track, see the handleMediaStatusChanged()
  // function

  // clear the shuffle list, in case shuffle is activated
  if (_shuffle)
    _shuffleAlreadyPlayed.clear();

  // to continue playing the list, see handleMediaStatusChanged() function
  _playall = true;

  // play the track
  playTrack();
}

void MainWindow::playNext() {
  // check if playlist is empty
  if (_playlist->getNumberOfMainwindowTracks() == 0)
    return;

  // SHUFFLE MODUS
  if (_shuffle) {
    // qDebug() << "size of shuffle already played: "
    //          << int(_shuffleAlreadyPlayed.size());
    // qDebug() << "size of playlist main window: "
    //          << _playlist->getNumberOfMainwindowTracks();

    // if all the songs have been played one time, exit the loop
    if (int(_shuffleAlreadyPlayed.size()) >=
        _playlist->getNumberOfMainwindowTracks()) {
      if (_repeat) // by repeat, clear the shuffle number list, will
                   // automatically result in relooping through a shuffle list
        _shuffleAlreadyPlayed.clear();
      else {
        return; // stop going to the next song
      }
    }

    // getting a random index number, which hasnt been used before
    bool numberInList = true;
    int randomnumber = 0;
    do {
      // generate a random number
      randomnumber = randomNumberGenerator(
          0, _playlist->getNumberOfMainwindowTracks() - 1);
      // qDebug() << "random number: " << randomnumber;
      // if the  shuffle list is empty, save that number
      if (_shuffleAlreadyPlayed.empty()) {
        _shuffleAlreadyPlayed.push_back(randomnumber);
        break; // leave the while loop
      } else {
        // check if the number has already been used
        for (const int &idx : _shuffleAlreadyPlayed) {
          // qDebug() << "idx: " << idx;
          if (idx == randomnumber) {
            numberInList = true;
            break; // leave the range based for loop
          } else
            numberInList = false;
        }
        // qDebug() << "bool number in list: " << numberInList;

        // is the number not in the list yet, and is the list not full yet, add
        // the number
        if (!numberInList && int(_shuffleAlreadyPlayed.size()) <=
                                 _playlist->getNumberOfMainwindowTracks())
          _shuffleAlreadyPlayed.push_back(randomnumber);
      }
    } while (numberInList);

    // set the index to the random generate number

    _index = randomnumber;

    // qDebug() << "index random: " << _index;

  } else {
    // NORMAL MODUS
    // check if the index is pointing to the last song
    if (_index >= _playlist->getNumberOfMainwindowTracks() - 1) {
      // if repeat all is enabled, start at the beginning, put the index to 0,
      // which occurs at the ++_index;
      if (_repeat)
        _index = -1;
      // if repeat all is disabled, stop playing and stop going to play the next
      // song, reset the index to 0
      else {
        _player->stop();
        return;
      }
    }
    // set the index to the next song
    ++_index;
  }

  // qDebug() << "index: " << _index;
  // start playing the track
  playTrack();

  // qDebug() << "shuffle counter: " << _shuffleCounter;
  // qDebug() << "index: " << _index;
  // qDebug() << "number of tracks: " <<
  // _playlist->getNumberOfMainwindowTracks();
}

void MainWindow::playPrevious() {
  // check if the playlist is empty or the index
  // is already set to the first song in the list
  if (_playlist->getNumberOfMainwindowTracks() == 0 || _index <= 0) {
    return;
  }

  // TODO
  // if (_shuffle)
  //   _index = _shuffleAlreadyPlayed[];
  // as none of them is the case, than the player can switch to the previous
  // song
  // else
  --_index;
  // start playing the track
  playTrack();
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

  // set the infos of the current song in the display, running
  _playerStopped = true;

  // empty the output data info field and oled
  updateTrackInfoDisplay();

  // resetting the bool
  _playerStopped = false;

  // // resetting the shuffle list
  // _shuffleAlreadyPlayed.clear();
}

void MainWindow::playOneSong(QTableWidgetItem *item) {
  // to make sure the player stops after playing one song see the
  // handleMediaStatusChanged() function
  _playall = false;

  // take the row number of the selected item
  _index = item->row();

  // qDebug() << "index: " << _index;

  playTrack();
}

// set the playmode to shuffle instead of play next
void MainWindow::setShuffle(bool state) {
  // shuffle mode
  if (state) {
    _shuffleAlreadyPlayed.clear();
    _shuffle = true;
  } else
    _shuffle = false;
}

// how to deal with the response to a network reply
void MainWindow::getDataFromNetwork(QNetworkReply *reply) {

  // in case the image is coming
  if (_imagedata) {
    QByteArray bytes = reply->readAll(); // converting the reply to bytes
    QPixmap pixmap;                      // creating pixmap object
    pixmap.loadFromData(bytes);          // filling the pixmap up with the bytes
    ui->labelArtWork->setPixmap(pixmap); // show the image in the main window
  } else {
    // in case the JSON data is coming
    QString text = reply->readAll(); // read the text string from the reply
    // qDebug() << "API JSON returning: " << text;

    // key value prinzip
    QJsonDocument doc = QJsonDocument::fromJson(
        text.toUtf8());             // convert the textstring to a JSON document
    QJsonObject obj = doc.object(); // convert it to a JSON object
    QJsonObject main = obj["album"].toObject(); // get the album values
    QJsonArray submainarray =
        main["image"].toArray(); // from the image value, turn the key into an
                                 // JSON array

    // qDebug() << "obj: " << obj;
    // qDebug() << "main: " << main;

    // QString album = main["name"].toString();
    // QString artist = main["artist"].toString();
    QString urlimage = "";

    // look for the right size in the array with different image urls to
    // different sizes, if available
    for (const auto &value : submainarray) {
      QJsonObject imageObject = value.toObject();
      if (_imageSize ==
          imageObject["size"].toString()) // in case the demanded size is
                                          // found, get the corresponding url
        urlimage = imageObject["#text"].toString();
    }

    // qDebug() << "album: " << album;
    // qDebug() << "artist: " << artist;
    // qDebug() << "urlimage: " << urlimage;

    // only send a network request if an url has been found
    if (!urlimage.isEmpty()) {
      QNetworkRequest request(urlimage);
      _network->get(request);
      _imagedata = true;
    } else { // if not, clear the artwork
      ui->labelArtWork->clear();
    }
  }
}

void MainWindow::refreshTableWidgetCurrentPlaylist() {
  // to restart playing the song at the first index, in case the playlist has
  // been edited, or the order has changed
  if (_playlistChanged)
    _index = 0;
  // qDebug() << "_index: " << _index;

  // count the number of Tracks being found
  int rowCount = _playlist->getNumberOfMainwindowTracks();

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
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetCurrentPlaylist->setItem(row, 4, item);

    item = new QTableWidgetItem(QString::number((*it)->getNumber()));
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetCurrentPlaylist->setItem(row, 5, item);

    item = new QTableWidgetItem((*it)->getGenre());
    ui->tableWidgetCurrentPlaylist->setItem(row, 6, item);

    item = new QTableWidgetItem(convertSecToTimeString((*it)->getDuration()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetCurrentPlaylist->setItem(row, 7, item);

    item = new QTableWidgetItem(QString::number((*it)->getBitrate()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetCurrentPlaylist->setItem(row, 8, item);

    item = new QTableWidgetItem(QString::number((*it)->getSamplerate()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetCurrentPlaylist->setItem(row, 9, item);

    item = new QTableWidgetItem(QString::number((*it)->getChannels()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetCurrentPlaylist->setItem(row, 10, item);

    item = new QTableWidgetItem(((*it)->getFileLocation()));
    ui->tableWidgetCurrentPlaylist->setItem(row, 11, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetCurrentPlaylist->resizeColumnsToContents();
  ui->tableWidgetCurrentPlaylist->setColumnWidth(1, 300);
  ui->tableWidgetCurrentPlaylist->setAlternatingRowColors(true);

  // set the playlist name in the main window info output
  ui->labelCurrentPlaylist->setText(_playlist->getPllName());
  ui->labelNumberOfTracksAdded->setText(
      QString::number(ui->tableWidgetCurrentPlaylist->rowCount()));
  ui->labelTotalTime->setText(
      convertSecToTimeString(_playlist->calculatePlaylistTotalTime()));

  // colour the row which track is currently playing
  setItemBackgroundColour();
}

void MainWindow::updateTrackInfoDisplay() {
  // to make sure the index is not pointing to a non existing object, or the
  // playlist is empty, or the player has stopped

  if (_index >= _playlist->getNumberOfMainwindowTracks() ||
      _playlist->getNumberOfMainwindowTracks() == 0 || _playerStopped) {
    // clear all the outputs
    ui->labelCurrentSong->clear();
    ui->labelcurrentAlbum->clear();
    ui->labelcurrentArtist->clear();
    ui->labelTimeSong->setText("00:00");
    _oled->updateSong("", "", "");
    return;
  }

  // getting the infos from the playlist for this particular track
  QString title = (*_playlist)[_index].getTitle();
  QString album = (*_playlist)[_index].getAlbum();
  QString artist = (*_playlist)[_index].getArtist();
  QString time = convertSecToTimeString((*_playlist)[_index].getDuration());

  // in case the filelocation is invalid
  if (!_filelocationValid) {
    // set the outputs, but turn them to a red colour
    ui->labelCurrentSong->setStyleSheet("color:#FF0000");
    ui->labelcurrentAlbum->setStyleSheet("color:#FF0000");
    ui->labelcurrentArtist->setStyleSheet("color:#FF0000");
    ui->labelTimeSong->setStyleSheet("color:#FF0000");

    ui->labelCurrentSong->setText(title);
    ui->labelcurrentAlbum->setText(album);
    ui->labelcurrentArtist->setText(artist);
    ui->labelTimeSong->setText(time);
    _oled->updateSong("", "", "");
    return;
  } else {
    // set colour back to default colours
    ui->labelCurrentSong->setStyleSheet("");
    ui->labelcurrentAlbum->setStyleSheet("");
    ui->labelcurrentArtist->setStyleSheet("");
    ui->labelTimeSong->setStyleSheet("");
  }

  // try to get the artwork out of the music file
  // TODO failed so far
  // in case that doesnt work, try to get artwork from the internet
  // search by artist and album
  QUrl url = "http://ws.audioscrobbler.com/2.0/"
             "?method=album.getinfo&api_key=" +
             _apiKey + "&artist=" + artist + "&album=" + album + "&format=json";
  // qDebug() << "URl: " << url;
  _imagedata = false; // setting the image data to false, to make sure the
                      // first network request is getting the JSON data, the
                      // next request is getting the album image (png or jpg)
  QNetworkRequest request(url);
  _network->get(request);

  // sending the current song data to the main window and oled display
  ui->labelCurrentSong->setText(title);
  ui->labelcurrentAlbum->setText(album);
  ui->labelcurrentArtist->setText(artist);
  ui->labelTimeSong->setText(time);
  _oled->updateSong(title.toStdString(), album.toStdString(),
                    artist.toStdString());
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
  setItemBackgroundColour();
  if (status == QMediaPlayer::EndOfMedia) {
    stopPlaying();
    if (_playall) // if all songs should be played, _playall is true and the
                  // next song is started
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

  // stop playing
  stopPlaying();

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

  saveSettings(); // saving programm settings

  bool success = false;
  // cleaning up the database
  QMetaObject::invokeMethod(_worker, "cleanupDatabase",
                            Qt::BlockingQueuedConnection, &success);
  if (success)
    qDebug() << "Database clean up was successfull";

  // close database
  QMetaObject::invokeMethod(_worker, "closeDatabase",
                            Qt::BlockingQueuedConnection);

  // stop the worker thread for the database operations
  _dbthread->quit();
  _dbthread->wait();
}

// a recursive function to go through all the subdirectories and collect all
// the music files stored in there
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
      if (fileExtension4 == ".mp3" || fileExtension4 == ".aac" ||
          fileExtension4 == ".ac3" || fileExtension4 == ".wma" ||
          fileExtension4 == ".wav" || fileExtension5 == ".flac" ||
          fileExtension5 == ".eac3" || fileExtension5 == ".alac") {
        qDebug() << "File:" << entry.absoluteFilePath();
        _detectedMusicFiles.push_back(filefound);
      }
    }
  }
}

// to colour the background of the current playing song
void MainWindow::setItemBackgroundColour() {
  for (int row = 0; row < ui->tableWidgetCurrentPlaylist->rowCount(); ++row) {
    if (row == _index) {
      for (int col = 0; col < ui->tableWidgetCurrentPlaylist->columnCount();
           ++col) {
        QTableWidgetItem *item = ui->tableWidgetCurrentPlaylist->item(row, col);
        item->setBackground(Qt::darkGray);
      }
    } else {
      for (int col = 0; col < ui->tableWidgetCurrentPlaylist->columnCount();
           ++col) {
        QTableWidgetItem *item = ui->tableWidgetCurrentPlaylist->item(row, col);
        item->setBackground(QBrush());
      }
    }
  }
}

bool MainWindow::playTrack() {

  // to make sure the _index is not pointing outside the playlist range
  if (_playlist->getNumberOfMainwindowTracks() >= 1 && _index >= 0 &&
      _index < _playlist->getNumberOfMainwindowTracks()) {
    // pass the file location of that entry to the player source
    _playThisSong = (*_playlist)[_index].getFileLocation();

    // check if the file location is valid
    if (!QFile::exists(_playThisSong)) {
      _filelocationValid = false;
      updateTrackInfoDisplay();
      return false;
    } else {
      _filelocationValid = true;
      _player->setSource(QUrl::fromLocalFile(_playThisSong));

      // start playing the song
      _player->play();

      // set the infos of the current song in the display, running
      updateTrackInfoDisplay();
      return true;
    }
  }
  return false;
}

void MainWindow::initializeSettings() {

  // Set default values if the settings file or certain parameters do not
  // exist
  if (!_settings.contains("Api Key")) {
    _settings.setValue("Api Key", _apiKey);
  }
  if (!_settings.contains("Volume")) {
    _settings.setValue("Volume", _startVolume);
  }
  if (!_settings.contains("Default Playlist ID")) {
    _settings.setValue("Default Playlist ID", _defaultPlaylistID);
  }
  if (!_settings.contains("OLED Status")) {
    _settings.setValue("OLED Status", _statusOled);
  }
  if (!_settings.contains("OLED Bus")) {
    _settings.setValue("OLED Bus", QString::fromStdString(_oled->getBus()));
  }
  if (!_settings.contains("OLED Adress")) {
    _settings.setValue("OLED Adress",
                       QString::fromStdString(_oled->getAdress()));
  }

  qDebug() << "Settings initialized with default values!";
}

void MainWindow::saveSettings() {
  _settings.setValue("Api Key", _apiKey);
  _settings.setValue("Volume", _audio->volume());
  _settings.setValue("Default Playlist ID", _playlist->getPllID());
  _settings.setValue("OLED Status", _statusOled);
  _settings.setValue("OLED Bus", QString::fromStdString(_oled->getBus()));
  _settings.setValue("OLED Adress", QString::fromStdString(_oled->getAdress()));
}

void MainWindow::loadSettings() {
  _apiKey = _settings.value("Api Key").toString();
  _startVolume = _settings.value("Volume").toFloat();
  _defaultPlaylistID = _settings.value("Default Playlist ID").toInt();
  _statusOled = _settings.value("OLED Status").toBool();
  _oled->setBus(_settings.value("OLED Bus").toString().toStdString());
  _oled->setAdress(_settings.value("OLED Adress").toString().toStdString());
}

int MainWindow::randomNumberGenerator(
    const int &min, const int &max) { // Initialize a random number generator,
                                      // within the min and max range
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(min, max);
  return distrib(
      gen); // returning a random number between the min and max values
}
