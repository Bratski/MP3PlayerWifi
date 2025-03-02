#ifndef DIALOGMANAGEMENT_H
#define DIALOGMANAGEMENT_H

#include <QDialog>

namespace Ui {
class DialogManagement;
}

class DialogManagement : public QDialog {
  Q_OBJECT

public:
  explicit DialogManagement(QWidget *parent = nullptr);
  ~DialogManagement();

public slots:
  void cancel();

private:
  Ui::DialogManagement *ui;
};

#endif // DIALOGMANAGEMENT_H
