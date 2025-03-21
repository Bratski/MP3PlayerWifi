#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent, COled *oled)
    : QDialog(parent), ui(new Ui::DialogSettings), _oled(oled) {
  ui->setupUi(this);

  // initialize
  setWindowTitle("Settings");
  showOledData();
  toggleOledButtons(_statusOled);
  toggleRTCButtons(_statusRTC);

  // connect button / checkbox events to functions
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogSettings::close);
  QObject::connect(ui->pushButtonAutoDetectOled, SIGNAL(clicked(bool)), this,
                   SLOT(autodetectOled()));
  QObject::connect(ui->pushButtonInitializeOled, SIGNAL(clicked(bool)), this,
                   SLOT(initializeOled()));

  QObject::connect(ui->checkBoxOled, SIGNAL(toggled(bool)), this,
                   SLOT(toggleOledButtons(bool)));
  QObject::connect(ui->checkBoxRTC, SIGNAL(toggled(bool)), this,
                   SLOT(toggleRTCButtons(bool)));
}

DialogSettings::~DialogSettings() { delete ui; }

void DialogSettings::autodetectOled() {
  _oled->autodetect();
  showOledData();
}

void DialogSettings::initializeOled() {
  _oled->setBus(ui->lineEditi2cBus->text().toStdString());
  _oled->setAdress(ui->lineEditi2cAdress->text().toStdString());

  if (!_oled->initialize()) {
    QMessageBox::warning(this, "Error", "no Oled-display could be initialized");
  } else {
    QMessageBox::information(this, "Success",
                             "Oled-display succesfully initialized");
  }
}

void DialogSettings::toggleRTCButtons(bool checked) {
  _statusRTC = checked;
  ui->lineEditRTCPin1->setEnabled(_statusRTC);
  ui->lineEditRTCPin2->setEnabled(_statusRTC);
  ui->lineEditRTCPin3->setEnabled(_statusRTC);
}

void DialogSettings::showOledData() {
  ui->lineEditi2cAdress->setText(QString::fromStdString(_oled->getAdress()));
  ui->lineEditi2cBus->setText(QString::fromStdString(_oled->getBus()));
}

void DialogSettings::toggleOledButtons(bool checked) {
  _statusOled = checked;
  ui->pushButtonAutoDetectOled->setEnabled(_statusOled);
  ui->pushButtonInitializeOled->setEnabled(_statusOled);
  ui->lineEditi2cAdress->setEnabled(_statusOled);
  ui->lineEditi2cBus->setEnabled(_statusOled);

  if (!_statusOled)
    _oled->turnOff();
}
