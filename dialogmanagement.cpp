#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent,
                                   CPlaylistContainer *playlist)
    : QDialog(parent), ui(new Ui::DialogManagement), _playlist(playlist) {
  ui->setupUi(this);

  // initialize the window
  setWindowTitle("Playlist Management");
  readDatabase();
  // ui->tableWidgetPlaylists->hideColumn(0);

  // setting the button connections
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogManagement::close);
  QObject::connect(ui->pushButtonOpen, &QPushButton::clicked, this,
                   &DialogManagement::openPlaylist);
  QObject::connect(ui->pushButtonAdd, &QPushButton::clicked, this,
                   &DialogManagement::addNewPlaylist);
  QObject::connect(ui->pushButtonDelete, &QPushButton::clicked, this,
                   &DialogManagement::deletePlaylist);

  // event of an item has been edited in the table widget
  QObject::connect(ui->tableWidgetPlaylists, &QTableWidget::itemChanged, this,
                   &DialogManagement::namePlaylistEdited);
}

DialogManagement::~DialogManagement() { delete ui; }

void DialogManagement::openPlaylist() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  // check if one row has been selected, if yes, which one? If not return with
  // error message

 QList<QTableWidgetSelectionRange> selectedRanges = ui->tableWidgetPlaylists->selectedRanges();

  if (selectedRanges.size() != 1) {
    QMessageBox::warning(
        this, "Error",
        "Only 1 playlist at the time can be selected to be opened!");
    return;
  }

  // get the items at the selected row
  int selectedRow = selectedRanges.first().topRow();
  QTableWidgetItem *idItem = ui->tableWidgetPlaylists->item(selectedRow, 0); // Column 0 (ID)
  QTableWidgetItem *nameItem = ui->tableWidgetPlaylists->item(selectedRow, 1); // Column 1 (Name)

  // empty the playlist
  _playlist->clear();

  // extract the PllID and PllName from the items and set the _playlist name and id
  _playlist->setPllID(idItem->text().toInt());
  _playlist->setPllName(nameItem->text());

  // fill the playlist with the database tracks
  _playlist->readPlaylistFromDatabase();

  // prevents interfering with the namePlaylistEdited function
  blockSignals(false);
  // leave the dialog management and go back to mainwindow
  this->close();
}

void DialogManagement::addNewPlaylist() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  QString name;
  name = ui->lineEditNewPlaylist->text();

  // check if there is a name in the input field,
  if (name.size() == 0) {
    QMessageBox::warning(this, "Error", "No valid name in the input field");
    return;
  }

  // create a query, and check if name already exists in the database
  QSqlQuery query;

  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  if (!query.exec())
    return;

  if (query.first()) {
    QMessageBox::warning(this, "Error",
                         "The playlist already exists, please change the name");
    return;
  }

  // create the new playlist in the database
  query.prepare("INSERT INTO Playlist (PllName) VALUES (:PllName) ");
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    QMessageBox::warning(this, "Error",
                         "Error creating new Playlist in the Database");
    return;
  }

  // update the table
  readDatabase();
  blockSignals(false);
}

void DialogManagement::deletePlaylist() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);

  // check if rows are selected in the table
  QList<QTableWidgetSelectionRange> selectedRanges =
      ui->tableWidgetPlaylists->selectedRanges();

  // delete the selected rows from the database
  QSqlQuery query;

  // Iterate through each selected range
  for (const QTableWidgetSelectionRange &range : selectedRanges) {
    // Iterate through each row in the range
    for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
      // Access the items in the row
      QTableWidgetItem *idItem =
          ui->tableWidgetPlaylists->item(row, 0); // Column 0 (ID)
      QTableWidgetItem *nameItem =
          ui->tableWidgetPlaylists->item(row, 1); // Column 1 (Name)

      // Get the ID and Name
      int id = idItem->text().toInt();
      QString name = nameItem->text();

      // if the playlist is currently opened in the main window or is the first
      // entry in the database, it can not be deleted
      if (name == _playlist->getPllName() || id == 1) {
        QMessageBox::warning(this, "Error",
                             "The current playlist cannot be deleted!");
        return;
      }

      // warning are you sure
      QMessageBox msg;
      msg.addButton("Yes", QMessageBox::YesRole);
      msg.addButton("No", QMessageBox::NoRole);
      msg.setWindowTitle("Delete?");
      msg.setIcon(QMessageBox::Warning);
      msg.setText(
          "Are you sure to delete this playlist, inclusive its contents?");

      // msg.exec() returns "3" if norole, "2" if yesrole
      if (msg.exec() == 3) {
        return;
      }
      // creating the deletion query
      query.prepare("DELETE FROM Playlist WHERE PllName = :PllName ");
      query.bindValue(":PllName", name);

      // deleting the database playlist entry
      if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Error removing Playlists!");
        return;
      }
    }
  }
  // refresh the table
  readDatabase();

  // prevents interfering with the namePlaylistEdited function
  blockSignals(false);
}

void DialogManagement::namePlaylistEdited(QTableWidgetItem *item) {
  // If the flag is set, ignore the signal to avoid recursion
  if (isEditing) {
    return;
  }

  // Set the flag to true to prevent recursive calls
  isEditing = true;
  // get the edited name
  QString name = item->text();

  // get the database id, where the new name must be stored
  int row = item->row();
  int id = ui->tableWidgetPlaylists->item(row, 0)->text().toInt();

  // check if the edited name already exists in the database
  QSqlQuery query;
  query.prepare("SELECT PllID FROM Playlist WHERE PllName = :PllName ");
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    QMessageBox::warning(this, "Error",
                         "Error checking the Playlist name in the Database");
    return;
  }
  if (query.first()) {
    QMessageBox::warning(this, "Error",
                         "The playlist already exists, please change the name");
    readDatabase();
    return;
  }

  // update the database at position id
  query.prepare("UPDATE Playlist SET PllName = :PllName WHERE PllID = :PllID ");
  query.bindValue(":PllID", id);
  query.bindValue(":PllName", name);

  if (!query.exec()) {
    QMessageBox::warning(
        this, "Error",
        "Error updating name in the Playlist name in the Database");
    return;
  }

  // update the table
  readDatabase();
  // prevents interfering with the namePlaylistEdited function
  isEditing = false;
}

void DialogManagement::readDatabase() {
  // prevents interfering with the namePlaylistEdited function
  blockSignals(true);
  // empty the list with playlists
  ui->tableWidgetPlaylists->clearContents();
  ui->tableWidgetPlaylists->setRowCount(0);

  // create a query, and find in the database
  QSqlQuery query;
  query.prepare("SELECT Playlist.PllID, Playlist.PllName FROM Playlist ");

  if (!query.exec()) {
    QMessageBox::warning(this, "Error",
                         "Error selecting the Playlist name in the Database");
    return;
  }

  // count the number of Playlists being found
  int rowCount = 0;
  while (query.next())
    ++rowCount;

  // set the table in Dialog Management to the corresponding number of rows
  ui->tableWidgetPlaylists->setRowCount(rowCount);

  // populate the table with data from query
  QTableWidgetItem *item;
  query.seek(-1); // reset query to start position
  int row = 0;
  while (query.next()) {
    // Playlist ID
    item = new QTableWidgetItem(query.value(0).toString());
    ui->tableWidgetPlaylists->setItem(row, 0, item);
    // Playlist Name
    item = new QTableWidgetItem(query.value(1).toString());
    ui->tableWidgetPlaylists->setItem(row, 1, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetPlaylists->resizeColumnsToContents();
  ui->tableWidgetPlaylists->setAlternatingRowColors(true);

  // prevents interfering with the namePlaylistEdited function
  blockSignals(false);
}
