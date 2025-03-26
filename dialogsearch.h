#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include "cplaylistcontainer.h"
#include <QDialog>
#include <QTableWidget>

namespace Ui {
class DialogSearch;
}

class DialogSearch : public QDialog {
  Q_OBJECT

public:
  explicit DialogSearch(QWidget* parent = nullptr,
                        CPlaylistContainer* playlist = nullptr,
                        bool* playlistChanged = nullptr);
  ~DialogSearch();

public slots:
  void filterPlaylist();
  void openFilteredPlaylist();
  void filterOnItem(QTableWidgetItem* item);

private:
  Ui::DialogSearch* ui;
  CPlaylistContainer* _playlist;
  bool* _playlistChanged;
  void refreshtableWidgetFoundEntries();
  const QString convertSecToTimeString(
      const int& sec); // I tried to use the function from the mainwindow class,
                       // by a function pointer parameter, did not work, so
                       // using a copy instead // TODO
};

#endif // DIALOGSEARCH_H
