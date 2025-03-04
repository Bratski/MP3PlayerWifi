#ifndef DIALOGADDPLAYLIST_H
#define DIALOGADDPLAYLIST_H

#include <QDialog>

namespace Ui {
class DialogAddPlaylist;
}

class DialogAddPlaylist : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddPlaylist(QWidget *parent = nullptr);
    ~DialogAddPlaylist();

private:
    Ui::DialogAddPlaylist *ui;
};

#endif // DIALOGADDPLAYLIST_H
