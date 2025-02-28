#include "dialogsearch.h"
#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogSearch) {
  ui->setupUi(this);
  setWindowTitle("Search and filter");
  QObject::connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), this,
                   SLOT(cancel()));
}

DialogSearch::~DialogSearch() { delete ui; }

void DialogSearch::cancel() { this->close(); }
