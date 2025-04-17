#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include "cplaylistcontainer.h"
#include <QDialog>
#include <QTableWidget>

// needed to use a function pointer to a memberfunction of the mainwindow class
class MainWindow; // forward declaration of the mainwindow class
typedef const QString (MainWindow::*timefunctionptrtype)(
    const int& sec); // without "typedef" or "using" it is far more complicated

namespace Ui {
class DialogSearch;
}

class DialogSearch : public QDialog {
  Q_OBJECT

public:
  explicit DialogSearch(
      QWidget* parent = nullptr,
      MainWindow* parentw =
          nullptr, // needed to call the member function of mainwindow
      CPlaylistContainer* playlist = nullptr, bool* playlistChanged = nullptr,
      timefunctionptrtype =
          nullptr); // needed to call the member function of mainwindow
  ~DialogSearch();

public slots:
  void filterPlaylist();
  void openFilteredPlaylist();
  void filterOnItem(QTableWidgetItem* item);

private:
  Ui::DialogSearch* ui;
  MainWindow* _parentw; // needed to call the member function of mainwindow
  CPlaylistContainer* _playlist;
  bool* _playlistChanged;
  timefunctionptrtype _convertTime;
  const QString convertTime(const int& sec) {
    return (_parentw->*_convertTime)(
        sec); // this calls the member function in the MainWindow Class, note
              // that both pointers are needed: the pointer to the main window
              // object and its memberfunction
  }
  void refreshtableWidgetFoundEntries();
  // const QString convertSecToTimeString(
  //     const int& sec); // I tried to use the function from the mainwindow
  //     class,
  //                      // by a function pointer parameter, did not work, so
  //                      // using a copy instead // TODO
};

#endif // DIALOGSEARCH_H
