#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget* parent, COled* oled, QString* apikey,
                               bool* statusoled, CRotaryencoder* rtc,
                               bool* statusrtc)
    : QDialog(parent), ui(new Ui::DialogSettings), _oled(oled), _apiKey(apikey),
      _statusOled(statusoled), _rtc(rtc), _statusRTC(statusrtc) {
  ui->setupUi(this);

  // initialize
  setWindowTitle("Settings");
  showOledData();
  showRTCData();
  ui->checkBoxOled->setChecked(*_statusOled);
  ui->checkBoxRTC->setChecked(*_statusRTC);
  toggleOledButtons(*_statusOled);
  toggleRTCButtons(*_statusRTC);
  ui->lineEditApiKey->setText(*_apiKey);

  // connect button
  QObject::connect(ui->pushButtonCancel, &QPushButton::clicked, this,
                   &DialogSettings::close);
  QObject::connect(ui->pushButtonAutoDetectOled, SIGNAL(clicked(bool)), this,
                   SLOT(autodetectOled()));
  QObject::connect(ui->pushButtonInitializeOled, SIGNAL(clicked(bool)), this,
                   SLOT(initializeOled()));
  QObject::connect(ui->pushButtonInitializeRTC, &QPushButton::clicked, this,
                   &DialogSettings::initializeRTC);
  QObject::connect(ui->pushButtonSave, &QPushButton::clicked, this,
                   &DialogSettings::saveSettings);

  // connect checkbox events to functions
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
    *_statusOled = false;
    QMessageBox::warning(this, "Error", "no Oled-display could be initialized");
  } else {
    QMessageBox::information(this, "Success",
                             "Oled-display succesfully initialized");
  }
}

void DialogSettings::initializeRTC() {

  // get the pin numbers from the textlines in the dialog
  QString chipNUMBER, pinSW, pinCLK, pinDT;

  chipNUMBER = ui->lineEditChipNumber->text();
  pinSW = ui->lineEditRTCPin1->text();
  pinCLK = ui->lineEditRTCPin2->text();
  pinDT = ui->lineEditRTCPin3->text();

  // check if they are actually numbers
  bool ok;

  if (chipNUMBER.toInt(&ok))
    _chipNUMBER = chipNUMBER.toInt();
  if (pinSW.toInt(&ok))
    _pinSW = pinSW.toInt();
  if (pinCLK.toInt(&ok))
    _pinSW = pinSW.toInt();
  if (pinDT.toInt(&ok))
    _pinSW = pinDT.toInt();

  // set pins and chip numbers
  _rtc->setChipnumber(_chipNUMBER);
  _rtc->setPins(_pinSW, _pinCLK, _pinDT);

  // invoke the initialisation
  *_statusRTC = _rtc->initialize();

  // send message successful or not
  if (!*_statusRTC && !_rtc->start()) {
    QMessageBox::warning(this, "Error",
                         "Rotary Encoder could NOT be initialized");
  }

  else {
    QMessageBox::information(this, "Success",
                             "Rotary Encoder succesfully initialized");
  }
}

void DialogSettings::toggleRTCButtons(bool checked) {
  *_statusRTC = checked;
  ui->lineEditChipNumber->setEnabled(*_statusRTC);
  ui->lineEditRTCPin1->setEnabled(*_statusRTC);
  ui->lineEditRTCPin2->setEnabled(*_statusRTC);
  ui->lineEditRTCPin3->setEnabled(*_statusRTC);
  ui->pushButtonInitializeRTC->setEnabled(*_statusRTC);
}

void DialogSettings::saveSettings() {
  *_apiKey = ui->lineEditApiKey->text();
  this->close();
}

void DialogSettings::showOledData() {
  ui->lineEditi2cAdress->setText(QString::fromStdString(_oled->getAdress()));
  ui->lineEditi2cBus->setText(QString::fromStdString(_oled->getBus()));
}

void DialogSettings::showRTCData() {
  ui->lineEditChipNumber->setText(QString::number(_rtc->getChipnumber()));
  ui->lineEditRTCPin1->setText(QString::number(_rtc->getPinSWITCH()));
  ui->lineEditRTCPin2->setText(QString::number(_rtc->getPinCLK()));
  ui->lineEditRTCPin3->setText(QString::number(_rtc->getPinDT()));
}

void DialogSettings::toggleOledButtons(bool checked) {
  *_statusOled = checked;
  ui->pushButtonAutoDetectOled->setEnabled(*_statusOled);
  ui->pushButtonInitializeOled->setEnabled(*_statusOled);
  ui->lineEditi2cAdress->setEnabled(*_statusOled);
  ui->lineEditi2cBus->setEnabled(*_statusOled);

  if (!*_statusOled)
    _oled->turnOff();
}
