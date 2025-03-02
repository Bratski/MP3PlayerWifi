#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogManagement) {
  ui->setupUi(this);
  setWindowTitle("Playlist Management");
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogManagement::close);
}

DialogManagement::~DialogManagement() { delete ui; }
