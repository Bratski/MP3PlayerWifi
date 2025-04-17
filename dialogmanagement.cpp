#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget* parent,
                                   CPlaylistContainer* playlist,
                                   CDatabaseWorker* workerdb,
                                   bool* playlistChanged)
    : QDialog(parent), ui(new Ui::DialogManagement), _playlist(playlist),
      _workerdb(workerdb), _playlistChanged(playlistChanged) {
  ui->setupUi(this);

  // initialize the window
  setWindowTitle("Playlist Management");
  _activeplaylistid = _playlist->getPllID();
  readDatabase();
  ui->tableWidgetPlaylists->hideColumn(0);

  // makes the delete key to delete the playlists as well
  _action.setShortcut(Qt::Key_Delete);
  this->addAction(&_action);
  QObject::connect(&_action, &QAction::triggered, this,
                   &DialogManagement::deletePlaylist);

  // setting the button connections
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogManagement::close);
  QObject::connect(ui->pushButtonOpen, &QPushButton::clicked, this,
                   &DialogManagement::openPlaylist);
  QObject::connect(ui->pushButtonAdd, &QPushButton::clicked, this,
                   &DialogManagement::addNewPlaylist);
  QObject::connect(ui->pushButtonDelete, &QPushButton::clicked, this,
                   &DialogManagement::deletePlaylist);
  QObject::connect(ui->pushButtonExport, &QPushButton::clicked, this,
                   &DialogManagement::exportXML);
  QObject::connect(ui->pushButtonImport, &QPushButton::clicked, this,
                   &DialogManagement::importXML);

  // event of an item has been edited in the table widget
  QObject::connect(ui->tableWidgetPlaylists, &QTableWidget::itemClicked, this,
                   &DialogManagement::dealWithMouseOneTimeClick);
  QObject::connect(ui->tableWidgetPlaylists, &QTableWidget::itemChanged, this,
                   &DialogManagement::namePlaylistEdited);
  QObject::connect(ui->tableWidgetPlaylists, &QTableWidget::itemDoubleClicked,
                   this, &DialogManagement::openPlaylist);

  // refresh table after saving db operation and importing a xml playlist file
  QObject::connect(_workerdb, &CDatabaseWorker::progressReady, this,
                   &DialogManagement::readDatabase);
}

DialogManagement::~DialogManagement() {
  // for (auto pllptr : _importedplaylists) {
  //   delete pllptr;
  // }
  delete ui;
}

void DialogManagement::openPlaylist() {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // check if one row has been selected, if yes, which one? If not return with
  // error message

  QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetPlaylists->selectedRanges();

  if (selectedRanges.size() == 0) {
    QMessageBox::warning(this, "Error",
                         "No playlist has been selected to be opened!");
    _isEditing = false;
    return;
  }

  // if yes, is it only one row or more?
  if (selectedRanges.first().rowCount() != 1) {
    QMessageBox::warning(this, "Error",
                         "Only 1 Playlist must be selected to be opened!");
    _isEditing = false;
    return;
  }

  // only ask to save the playlist to database, if the playlist has been
  // edited (_playlistChanged is set true)

  if (*_playlistChanged) {
    QMessageBox msg;
    msg.addButton("Yes", QMessageBox::YesRole);
    msg.addButton("No", QMessageBox::NoRole);
    msg.setWindowTitle("Save Playlist?");
    msg.setIcon(QMessageBox::Warning);
    msg.setText("Do you want to save the changes to the playlist?");

    // msg.exec() returns on x86 "3" if norole, "2" if yesrole on ARM64 "1" if
    // norole, "0" if yesrole
    int nr = msg.exec();
    // qDebug() << "return value int msg.exec: " << nr;
    if (nr == 2 || nr == 0) {
      emit saveToDBMainWindow(_playlist);
    }
  }

  // get the items at the selected row
  int selectedRow = selectedRanges.first().topRow();
  QTableWidgetItem* idItem =
      ui->tableWidgetPlaylists->item(selectedRow, 0); // Column 0 (ID)
  QTableWidgetItem* nameItem =
      ui->tableWidgetPlaylists->item(selectedRow, 1); // Column 1 (Name)

  // empty the playlist
  _playlist->clear();

  // extract the PllID and PllName from the items and set the _playlist name and
  // id
  _playlist->setPllID(idItem->text().toInt());
  _playlist->setPllName(nameItem->text());

  bool success = false;
  // fill the playlist with the database tracks
  QMetaObject::invokeMethod(
      _workerdb, "readPlaylistTracksFromDatabase", Qt::BlockingQueuedConnection,
      Q_ARG(CPlaylistContainer*, _playlist), Q_ARG(bool*, &success));

  // if all went well, the playlist is a exact representation of the database,
  // and no need to save it again
  if (success)
    *_playlistChanged = false;

  // qDebug() << "success: " << success;
  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogManagement::addNewPlaylist() {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  QString name;
  name = ui->lineEditNewPlaylist->text();

  // check if there is a name in the input field,
  if (name.size() == 0) {
    QMessageBox::warning(this, "Error", "No valid name in the input field");
    _isEditing = false;
    return;
  }

  bool success = false;
  bool doubleName = false;
  // add a new playlist with the name to the database
  QMetaObject::invokeMethod(_workerdb, "addNewPlaylist",
                            Qt::BlockingQueuedConnection, Q_ARG(QString, name),
                            Q_ARG(bool*, &success), Q_ARG(bool*, &doubleName));

  if (success) {
    qDebug() << "added the name playlist " << name << " successfully";
  }
  if (doubleName) {
    QMessageBox::warning(this, "Error",
                         "Name already in use, choose another name");
  }

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // update the table
  readDatabase();
}

void DialogManagement::deletePlaylist() {
  // if the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // check if rows are selected in the table
  QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetPlaylists->selectedRanges();

  // delete the selected rows from the database

  // Iterate through each selected range
  for (const QTableWidgetSelectionRange& range : selectedRanges) {
    // Iterate through each row in the range
    for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
      // Access the items in the row
      QTableWidgetItem* idItem =
          ui->tableWidgetPlaylists->item(row, 0); // Column 0 (ID)
      QTableWidgetItem* nameItem =
          ui->tableWidgetPlaylists->item(row, 1); // Column 1 (Name)

      // Get the ID and Name
      int id = idItem->text().toInt();
      QString name = nameItem->text();

      // if the playlist is currently opened in the main window or is the first
      // entry in the database, it can not be deleted
      if (id == 1) {
        QMessageBox::warning(this, "Error",
                             "The default playlist: " + name +
                                 " cannot be deleted!");
        _isEditing = false;
        return;
      }

      if (name == _playlist->getPllName()) {
        QMessageBox::warning(this, "Error",
                             "The activated playlist: " + name +
                                 " in the main window cannot be deleted!");
        _isEditing = false;
        return;
      }

      // warning are you sure
      QMessageBox msg;
      msg.addButton("Yes", QMessageBox::YesRole);
      msg.addButton("No", QMessageBox::NoRole);
      msg.setWindowTitle("Delete?");
      msg.setIcon(QMessageBox::Warning);
      msg.setText("Are you sure to delete this playlist: " + name +
                  " inclusive its contents?");

      // msg.exec() returns on x86 "3" if norole, "2" if yesrole on ARM64 "1" if
      // norole, "0" if yesrole
      int nr = msg.exec();
      // qDebug() << "return value int msg.exec: " << nr;
      if (nr == 3 || nr == 1) {
        _isEditing = false;
        return;
      }

      bool success = false;
      // delete the playlist from the database
      QMetaObject::invokeMethod(_workerdb, "deletePlaylist",
                                Qt::BlockingQueuedConnection,
                                Q_ARG(QString, name), Q_ARG(bool*, &success));
    }
  }

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // refresh the table
  readDatabase();
}

void DialogManagement::namePlaylistEdited(QTableWidgetItem* item) {
  // If the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // get the edited name
  QString name = item->text();

  // get the database id, where the new name must be stored
  int row = item->row();
  int id = ui->tableWidgetPlaylists->item(row, 0)->text().toInt();

  bool success = false;
  bool doubleName = false;
  // update the database
  QMetaObject::invokeMethod(_workerdb, "updatePlaylistInDatabase",
                            Qt::BlockingQueuedConnection, Q_ARG(QString, name),
                            Q_ARG(int, id), Q_ARG(bool*, &success),
                            Q_ARG(bool*, &doubleName));

  if (doubleName) {
    QMessageBox::warning(this, "Error",
                         "Name already in use, choose another name");
  }

  // change the name of the current playlist, only if the playlist is activated
  // in the main window!
  if (!doubleName && success && id == _activeplaylistid)
    _playlist->setPllName(name);

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // update the table
  readDatabase();
}

void DialogManagement::dealWithMouseOneTimeClick(QTableWidgetItem* item) {
  // create a short break of 400 ms (arbitrary but is the time a regular double
  // click is needed for) to check if a second click is coming

  QTimer::singleShot(400, this, [this, item]() {
    // did the row change?
    if (_lastrow != item->row())
      _firstclick = true;

    // is this the first one click on the row?
    if (_firstclick) {
      _lastrow = item->row();
      _firstclick = false;
      return;
    }
    // edit the row if it was not the first one click, and the row did not
    // change
    if (item->row() == _lastrow && item &&
        item == ui->tableWidgetPlaylists->currentItem()) {
      ui->tableWidgetPlaylists->editItem(item);
    }
  });
}

void DialogManagement::importXML() { // TODO
  // if the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // open file browser, select xml file to import
  QFile file = QFileDialog::getOpenFileName(
      this, "XML file to import", QDir::homePath(), "XML Files (*.xml)");

  if (file.fileName().isEmpty()) {
    // QMessageBox::warning(this, "Error", "No valid filename entered!");
    _isEditing = false;
    return;
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Error",
                         file.fileName() + " could not be opened");
    _isEditing = false;
    return;
  }
  // create a playlist vector
  CPlaylistContainer playlistImport;
  // track elements
  QString TraID;
  QString TraTitle;
  QString TraArtist;
  QString TraAlbum;
  int TraYear;
  int TraNumber;
  QString TraGenre;
  int TraDuration;
  int TraBitrate;
  int TraSamplerate;
  int TraChannels;
  QString TraFileLocation;

  // and add the xml file output
  QXmlStreamReader xml(&file);

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartElement) {
      // if (xml.name() == "Playlist") {
      //   // Read attributes
      //   QXmlStreamAttributes attributes = xml.attributes();
      //   if (attributes.hasAttribute("ID")) {
      //     playlistImport.setPllID(attributes.value("ID").toInt());
      //   }
      //   qDebug() << "pll id: " << playlistImport.getPllID();
      // }

      if (xml.name() == QLatin1String("Name")) {
        xml.readNext();
        playlistImport.setPllName(xml.text().toString());
        qDebug() << "pll name: " << playlistImport.getPllName();
      }

      if (xml.name() == QLatin1String("Track")) {

        // Read attributes
        QXmlStreamAttributes attributes = xml.attributes();
        if (attributes.hasAttribute("ID")) {
          TraID = attributes.value("ID").toString();
        }

        // Read child elements
        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                 xml.name() == QLatin1String("Track"))) {

          if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("Title")) {
              xml.readNext();
              TraTitle = xml.text().toString();
            } else if (xml.name() == QLatin1String("Artist")) {
              xml.readNext();
              TraArtist = xml.text().toString();
            } else if (xml.name() == QLatin1String("Album")) {
              xml.readNext();
              TraAlbum = xml.text().toString();
            } else if (xml.name() == QLatin1String("Year")) {
              xml.readNext();
              TraYear = xml.text().toInt();
            } else if (xml.name() == QLatin1String("Number")) {
              xml.readNext();
              TraNumber = xml.text().toInt();
            } else if (xml.name() == QLatin1String("Genre")) {
              xml.readNext();
              TraGenre = xml.text().toString();
            } else if (xml.name() == QLatin1String("Duration")) {
              xml.readNext();
              TraDuration = xml.text().toInt();
            } else if (xml.name() == QLatin1String("Bitrate")) {
              xml.readNext();
              TraBitrate = xml.text().toInt();
            } else if (xml.name() == QLatin1String("Samplerate")) {
              xml.readNext();
              TraSamplerate = xml.text().toInt();
            } else if (xml.name() == QLatin1String("Channels")) {
              xml.readNext();
              TraChannels = xml.text().toInt();
            } else if (xml.name() == QLatin1String("FileLocation")) {
              xml.readNext();
              TraFileLocation = xml.text().toString();
            }
          }

          xml.readNext();
        }
      }

      if (!TraID.isEmpty()) {
        qDebug() << "Track ID: " << TraID;
        qDebug() << "Track Title: " << TraTitle;
        qDebug() << "Track Artist: " << TraArtist;
        qDebug() << "Track Album: " << TraAlbum;
        qDebug() << "Track Year: " << TraYear;
        qDebug() << "Track Number: " << TraNumber;
        qDebug() << "Track Genre: " << TraGenre;
        qDebug() << "Track Duration: " << TraDuration;
        qDebug() << "Track Bitrate: " << TraBitrate;
        qDebug() << "Track Samplerate: " << TraSamplerate;
        qDebug() << "Track Channels: " << TraChannels;
        qDebug() << "Track Filelocation: " << TraFileLocation;

        CTrack newtrack(TraID, TraTitle, TraArtist, TraAlbum, TraYear,
                        TraNumber, TraGenre, TraDuration, TraBitrate,
                        TraSamplerate, TraChannels, TraFileLocation);
        playlistImport.addTrack(newtrack);
      }
    }
  }

  // in case the xml file is not readable, or corrupt
  if (xml.hasError()) {
    QMessageBox::warning(this, "XML Error",
                         "XML file corrupt, error: " + xml.errorString());
    file.close();
    _isEditing = false;
    return;
  }

  file.close();

  // check if the playlist name already exists in the database, if so give it
  // another name, add the playlist

  bool success = false;
  while (!success) {
    bool doubleName = false;
    // add a new playlist with the name to the database
    QMetaObject::invokeMethod(
        _workerdb, "addNewPlaylist", Qt::BlockingQueuedConnection,
        Q_ARG(QString, playlistImport.getPllName()), Q_ARG(bool*, &success),
        Q_ARG(bool*, &doubleName));

    if (doubleName) {
      bool ok;
      QString otherpllname = QInputDialog::getText(
          this, tr("Error"),
          tr("Name already in use, please enter another name: "),
          QLineEdit::Normal, QString(), &ok);
      if (!otherpllname.isEmpty() && ok)
        playlistImport.setPllName(otherpllname);
      else {
        _isEditing = false;
        return;
      }
    }
  }

  // get the pll id, if the new list name was added to the database
  // successfully, if not cancel import
  success = false;
  QMetaObject::invokeMethod(
      _workerdb, "setPllIDbasedOnName", Qt::BlockingQueuedConnection,
      Q_ARG(CPlaylistContainer*, &playlistImport), Q_ARG(bool*, &success));

  if (!success) {
    QMessageBox::warning(
        this, "Error",
        "Playlistname could not be bound to an id in the current database");
    _isEditing = false;
    return;
  }

  qDebug() << "Playlist ID in the database: " << playlistImport.getPllID();
  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;

  // save the playlist vector to the database
  emit saveToDBMainWindow(&playlistImport);

  // refresh the table in de management window, emit when ready, because
  // readDatabase is not refreshed as savetoDBMW is a Queued Operation!
}

void DialogManagement::exportXML() {
  // if the flag is set, ignore the signal to avoid recursion
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // which playlist is selected in the management table? Only one playlist
  // at the time?! check if one row has been selected, if yes, which one?
  // If not return with error message

  QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetPlaylists->selectedRanges();

  if (selectedRanges.size() == 0) {
    QMessageBox::warning(this, "Error",
                         "No playlist has been selected to be exported!");
    _isEditing = false;
    return;
  }

  // if yes, is it only one row or more?
  if (selectedRanges.first().rowCount() != 1) {
    QMessageBox::warning(this, "Error",
                         "Only 1 Playlist can be selected to be exported!");
    _isEditing = false;
    return;
  }

  // get the items at the selected row
  int selectedRow = selectedRanges.first().topRow();
  QTableWidgetItem* idItem =
      ui->tableWidgetPlaylists->item(selectedRow, 0); // Column 0 (ID)
  QTableWidgetItem* nameItem =
      ui->tableWidgetPlaylists->item(selectedRow, 1); // Column 1 (Name)

  // create a new playlist object for export
  CPlaylistContainer playlistExport;
  //_playlist->clear();

  // extract the PllID and PllName from the items and set the _playlist
  // name and id
  playlistExport.setPllID(idItem->text().toInt());
  playlistExport.setPllName(nameItem->text());

  bool success = false;
  // fill the playlist with the database tracks
  QMetaObject::invokeMethod(
      _workerdb, "readDataBasePlaylist", Qt::BlockingQueuedConnection,
      Q_ARG(CPlaylistContainer*, &playlistExport), Q_ARG(bool*, &success));

  if (!success) {
    QMessageBox::warning(
        this, "Error",
        "Selected Playlist could not be read from the database!");
    _isEditing = false;
    return;
  }

  // open filebrowser to select/create file and location where the xml
  // file should be saved
  QFile file = QFileDialog::getSaveFileName(
      this, "Export to ", QDir::homePath(), "XML Files (*.xml)");

  // cancel if no filename has been selected
  if (file.fileName().isEmpty()) {
    // QMessageBox::warning(this, "Error", "No valid filename entered!");
    _isEditing = false;
    return;
  }

  // add the extension to the filename, in case it does not end with .xml
  if (!file.fileName().endsWith(".xml"))
    file.setFileName(file.fileName() + ".xml");
  // qDebug() << "filename: " << file.fileName();

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Error",
                         file.fileName() + " could not be opened");
    _isEditing = false;
    return;
  }

  // start the xmlwriter
  QXmlStreamWriter xmlWriter(&file);
  xmlWriter.setAutoFormatting(true); // For human-readable output

  // read the selected playlist and convert this to xml
  xmlWriter.writeStartDocument();

  // Root element playlist
  xmlWriter.writeStartElement("Playlist");
  xmlWriter.writeAttribute("ID", QString::number(playlistExport.getPllID()));
  xmlWriter.writeTextElement("Name", playlistExport.getPllName());

  // Write each track as an XML element
  for (auto it = playlistExport.beginPtr(); it != playlistExport.endPtr();
       ++it) {
    xmlWriter.writeStartElement("Track");

    xmlWriter.writeAttribute("ID", (*it)->getID());
    xmlWriter.writeTextElement("Title", (*it)->getTitle());
    xmlWriter.writeTextElement("Artist", (*it)->getArtist());
    xmlWriter.writeTextElement("Album", (*it)->getAlbum());
    xmlWriter.writeTextElement("Year", QString::number((*it)->getYear()));
    xmlWriter.writeTextElement("Number", QString::number((*it)->getNumber()));
    xmlWriter.writeTextElement("Genre", (*it)->getGenre());
    xmlWriter.writeTextElement("Duration",
                               QString::number((*it)->getDuration()));
    xmlWriter.writeTextElement("Bitrate", QString::number((*it)->getBitrate()));
    xmlWriter.writeTextElement("Samplerate",
                               QString::number((*it)->getSamplerate()));
    xmlWriter.writeTextElement("Channels",
                               QString::number((*it)->getChannels()));
    xmlWriter.writeTextElement("FileLocation", (*it)->getFileLocation());

    xmlWriter.writeEndElement();
  }
  xmlWriter.writeEndElement();
  xmlWriter.writeEndDocument();

  // and save it to the file
  file.close();

  QMessageBox::information(this, "Success",
                           "Playlist " + playlistExport.getPllName() +
                               " successfully exported to " + file.fileName());

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;
}

void DialogManagement::readDatabase() {
  // If the flag is set, ignore the signal to avoid recursion
  // qDebug() << "readDatabase is executed";
  if (_isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  _isEditing = true;

  // qDebug() << "readDatabase is executed";
  // temporare QString vector
  std::vector<QString> playlistsInDatabase;

  // for pi compilation necessary QT 6.4.2
  std::vector<QString>* pllptr = &playlistsInDatabase;

  // empty the list with playlists
  ui->tableWidgetPlaylists->clearContents();
  ui->tableWidgetPlaylists->setRowCount(0);
  playlistsInDatabase.clear();
  // qDebug() << "playlistsInDatabase vector size: " <<
  // playlistsInDatabase.size();

  bool success = false;
  // find in the database
  QMetaObject::invokeMethod(
      _workerdb, "getPlaylistsFromDatabase", Qt::BlockingQueuedConnection,
      Q_ARG(std::vector<QString>*, pllptr), Q_ARG(bool*, &success));

  if (success) {
    // count the number of Playlists being found
    int rowCount = (playlistsInDatabase.size() / 2);

    // qDebug() << "rowCount: " << rowCount;
    // set the table in Dialog Management to the corresponding number of
    // rows
    ui->tableWidgetPlaylists->setRowCount(rowCount);

    // populate the table with data from query
    QTableWidgetItem* item;

    int row = 0;
    for (size_t i = 0; i < playlistsInDatabase.size(); ++i) {

      // Playlist ID
      if (i % 2 == 0) {
        item = new QTableWidgetItem(playlistsInDatabase[i]);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        item->setFlags(item->flags() &
                       ~Qt::ItemIsEditable); // make the item read only
        ui->tableWidgetPlaylists->setItem(row, 0, item);
      }
      // Playlist Name
      else {
        item = new QTableWidgetItem(playlistsInDatabase[i]);
        ui->tableWidgetPlaylists->setItem(row, 1, item);
        ++row;
      }
    }

    // customizing the looks
    ui->tableWidgetPlaylists->resizeColumnsToContents();
    ui->tableWidgetPlaylists->setAlternatingRowColors(true);
  }
  // colour the activited playlist
  setItemBackgroundColour();

  // prevents interfering with the namePlaylistEdited function
  _isEditing = false;
}

void DialogManagement::setItemBackgroundColour() {
  // to colour the background of the current playing song
  int rowactive = -1;
  for (int row = 0; row < ui->tableWidgetPlaylists->rowCount(); ++row) {
    for (int col = 0; col < ui->tableWidgetPlaylists->columnCount(); ++col) {
      QTableWidgetItem* item = ui->tableWidgetPlaylists->item(row, col);
      if (item->text().toInt() == _playlist->getPllID() ||
          rowactive == item->row()) {
        rowactive = item->row();
        item->setBackground(Qt::darkGray);
      } else
        item->setBackground(QBrush());
    }
  }
}
