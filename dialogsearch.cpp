#include "dialogsearch.h"
#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(QWidget* parent, MainWindow* parentw,
                           CPlaylistContainer* playlist, bool* playlistChanged,
                           timefunctionptrtype convertTime)
    : QDialog(parent), ui(new Ui::DialogSearch), _parentw(parentw),
      _playlist(playlist), _playlistChanged(playlistChanged),
      _convertTime(convertTime) {

  ui->setupUi(this);

  // initializing window
  setWindowTitle("Search and filter");
  ui->tableWidgetFoundEntries->hideColumn(0);
  ui->tableWidgetFoundEntries->hideColumn(11);
  // load the search window with the same songs as the mainwindow
  _playlist->filterPlaylist(CPlaylistContainer::art_t::byMainWindow, "");
  refreshtableWidgetFoundEntries();

  // connect the button events to functions
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogSearch::close);
  QObject::connect(ui->pushButtonFilter, &QPushButton::clicked, this,
                   &DialogSearch::filterPlaylist);
  QObject::connect(ui->pushButtonOK, &QPushButton::clicked, this,
                   &DialogSearch::openFilteredPlaylist);

  // to filter for a item double clicked in the table widget
  QObject::connect(ui->tableWidgetFoundEntries,
                   &QTableWidget::itemDoubleClicked, this,
                   &DialogSearch::filterOnItem);
}

DialogSearch::~DialogSearch() { delete ui; }

void DialogSearch::filterPlaylist() {
  // get the text to search for
  QString text = ui->lineEditSearch->text();

  // qDebug() << "text: " << text;
  int selectedIndex;
  // check which combobox element is selected, determines the ways of filtering
  if (text.isEmpty())
    selectedIndex = -1; // filter default, the whole playlist, in case no text
                        // is in the editline
  else
    selectedIndex = ui->comboBoxSongElements->currentIndex();

  // qDebug() << "selected index: " << selectedIndex;

  // search in the playlist for the tracks, which its element is corresponding
  // to search text
  // save the positive ones in the filter pointer vector in the
  // cplaylistcontainer

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
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byMainWindow, "");
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

void DialogSearch::filterOnItem(QTableWidgetItem* item) {
  // which column belongs to the item?
  int col = item->column();

  // get the text from the item
  QString text = item->text();

  // start filter on the column type
  switch (col) {
  case 1:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byTitle, text);
    break;
  case 2:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byArtist, text);
    break;
  case 3:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byAlbum, text);
    break;
  case 4:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byYear, text);
    break;
  case 6:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byGenre, text);
    break;
  default:
    _playlist->filterPlaylist(CPlaylistContainer::art_t::byMainWindow, "");
    break;
  }

  // show the results in the table
  refreshtableWidgetFoundEntries();
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
  QTableWidgetItem* item;

  int row = 0;
  // iterating through the filter pointer vector
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
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetFoundEntries->setItem(row, 4, item);

    item = new QTableWidgetItem(QString::number((*it)->getNumber()));
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetFoundEntries->setItem(row, 5, item);

    item = new QTableWidgetItem((*it)->getGenre());
    ui->tableWidgetFoundEntries->setItem(row, 6, item);

    item = new QTableWidgetItem(convertTime((*it)->getDuration())); // now the member function in mainwindow is
                                // called for the time conversion
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetFoundEntries->setItem(row, 7, item);

    item = new QTableWidgetItem(QString::number((*it)->getBitrate()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetFoundEntries->setItem(row, 8, item);

    item = new QTableWidgetItem(QString::number((*it)->getSamplerate()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetFoundEntries->setItem(row, 9, item);

    item = new QTableWidgetItem(QString::number((*it)->getChannels()));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidgetFoundEntries->setItem(row, 10, item);

    item = new QTableWidgetItem(((*it)->getFileLocation()));
    ui->tableWidgetFoundEntries->setItem(row, 11, item);

    ++row;
  }

  // customizing the looks
  ui->tableWidgetFoundEntries->resizeColumnsToContents();
  ui->tableWidgetFoundEntries->setColumnWidth(1, 300);
  ui->tableWidgetFoundEntries->setAlternatingRowColors(true);
}

// converts seconds and returns a QString displaying the time in this
// format "0:00:00", I tried to pass this function from the mainwindow by
// pointer, but failed! How can this be done? --> 2 things must be passed: the
// function pointer itself and the pointer to the mainwindow object const
// QString DialogSearch::convertSecToTimeString(const int& sec) {

//   int seconds = sec % 60;
//   int min = (sec / 60) % 60;
//   int hr = (sec / (60 * 60));

//   QString timeExHr = QString::number(min).rightJustified(2, '0') + ":" +
//                      QString::number(seconds).rightJustified(2, '0');
//   QString timeInHr = QString::number(hr) + ":" + timeExHr;

//   if (!hr)
//     return timeExHr;
//   else
//     return timeInHr;
// }
