#include "dialogmanagement.h"
#include "ui_dialogmanagement.h"

DialogManagement::DialogManagement(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogManagement) {
  ui->setupUi(this);
  setWindowTitle("Playlist Management");
  QObject::connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), this,
                   SLOT(cancel()));
}

DialogManagement::~DialogManagement() { delete ui; }

void DialogManagement::cancel() { this->close(); }
