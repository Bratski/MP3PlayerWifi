#include "dialogsearch.h"
#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(QWidget *parent, CPlaylistContainer *playlist,
                           bool *playlistChanged)
    : QDialog(parent), ui(new Ui::DialogSearch), _playlist(playlist),
      _playlistChanged(playlistChanged) {
  ui->setupUi(this);
  setWindowTitle("Search and filter");
  ui->tableWidgetFoundEntries->hideColumn(0);
  ui->tableWidgetFoundEntries->hideColumn(11);
  refreshtableWidgetFoundEntries();

  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogSearch::close);
  QObject::connect(ui->pushButtonFilter, &QPushButton::clicked, this,
                   &DialogSearch::filterPlaylist);
  QObject::connect(ui->pushButtonOK, &QPushButton::clicked, this,
                   &DialogSearch::openFilteredPlaylist);
}

DialogSearch::~DialogSearch() { delete ui; }

void DialogSearch::filterPlaylist() {
  // get the text to search for
  QString text = ui->lineEditSearch->text();

  qDebug() << "text: " << text;

  // check which combobox element is selected, determines the ways of filtering
  int selectedIndex = ui->comboBoxSongElements->currentIndex();

  qDebug() << "selected index: " << selectedIndex;

  // search in the playlist for the tracks, which its element is corresponding
  // to search text
  // save the positive ones in a temporary playlist

  switch (selectedIndex) {
  case 0:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byYear, text);

    break;
  case 1:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byAlbum, text);

    break;
  case 2:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byArtist, text);

    break;
  case 3:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byTitle, text);

    break;
  case 4:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byGenre, text);

    break;
  default:
    break;
  }

  // display the filtered ones in the table widget
  refreshtableWidgetFoundEntries();
}

void DialogSearch::openFilteredPlaylist() {
  // copy the filtered ptr vector to the mainwindow ptr vector (swapping), only
  // if there is one or more entry found
  if (_playlist->getNumberOfFilteredTracks() != 0) {
    _playlist->copyFilteredToMainwindow();
    *_playlistChanged = true;
  }

  // close the dialog
  this->close();
}

void DialogSearch::refreshtableWidgetFoundEntries() {

  // count the number of Tracks being found
  int rowCount = _playlist->getNumberOfFilteredTracks();
  // qDebug() << "row count: " << rowCount;

  // empty the current playlist
  ui->tableWidgetFoundEntries->clearContents();

  // set the table in mainwindow to the corresponding number of rows
  ui->tableWidgetFoundEntries->setRowCount(rowCount);

  // populate the table with data from the track vector (playlist)
  QTableWidgetItem *item;

  int row = 0;
  for (auto it = _playlist->beginFilterPtr(); it != _playlist->endFilterPtr();
       ++it) {
    item = new QTableWidgetItem((*it)->getID());
    ui->tableWidgetFoundEntries->setItem(row, 0, item);

    item = new QTableWidgetItem((*it)->getTitle());
    ui->tableWidgetFoundEntries->setItem(row, 1, item);

    item = new QTableWidgetItem((*it)->getArtist());
    ui->tableWidgetFoundEntries->setItem(row, 2, item);

    item = new QTableWidgetItem((*it)->getAlbum());
    ui->tableWidgetFoundEntries->setItem(row, 3, item);

    item = new QTableWidgetItem(QString::number((*it)->getYear()));
    ui->tableWidgetFoundEntries->setItem(row, 4, item);

    item = new QTableWidgetItem(QString::number((*it)->getNumber()));
    ui->tableWidgetFoundEntries->setItem(row, 5, item);

    item = new QTableWidgetItem((*it)->getGenre());
    ui->tableWidgetFoundEntries->setItem(row, 6, item);

    item = new QTableWidgetItem(convertSecToTimeString((*it)->getDuration()));
    ui->tableWidgetFoundEntries->setItem(row, 7, item);

    item = new QTableWidgetItem(QString::number((*it)->getBitrate()));
    ui->tableWidgetFoundEntries->setItem(row, 8, item);

    item = new QTableWidgetItem(QString::number((*it)->getSamplerate()));
    ui->tableWidgetFoundEntries->setItem(row, 9, item);

    item = new QTableWidgetItem(QString::number((*it)->getChannels()));
    ui->tableWidgetFoundEntries->setItem(row, 10, item);

    item = new QTableWidgetItem(((*it)->getFileLocation()));
    ui->tableWidgetFoundEntries->setItem(row, 11, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetFoundEntries->resizeColumnsToContents();
  ui->tableWidgetFoundEntries->setAlternatingRowColors(true);
}

// converts seconds and returns a QString displaying the time in this
// format "0:00:00", I tried to pass this function from the mainwindow by
// pointer, but failed! How can this be done?
const QString DialogSearch::convertSecToTimeString(const int &sec) {

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
