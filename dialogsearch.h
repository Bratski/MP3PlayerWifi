#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include <QDialog>

namespace Ui {
class DialogSearch;
}

class DialogSearch : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSearch(QWidget *parent = nullptr);
    ~DialogSearch();

public slots:
    void cancel();

private:
    Ui::DialogSearch *ui;
};

#endif // DIALOGSEARCH_H
