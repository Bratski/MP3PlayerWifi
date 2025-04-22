#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget* parent, COled* oled, QString* apikey,
                               bool* statusoled,
                               CRotaryEncoderWorker* workerrtc, bool* statusrtc)
    : QDialog(parent), ui(new Ui::DialogSettings), _oled(oled), _apiKey(apikey),
      _statusOled(statusoled), _workerrtc(workerrtc), _statusRTC(statusrtc) {
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
                   &DialogSettings::cancel);
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
    QMessageBox::warning(this, "Error", "NO Oled-display could be initialized");
  } else {
      *_statusOled = true;
      QMessageBox::information(this, "Success",
                             "Oled-display succesfully initialized");
  }
}

void DialogSettings::initializeRTC() {

  // get the pin numbers from the textlines in the dialog
  QString chipNUMBER, pinSW, pinCLK, pinDT;
  int chipnumber = 4;
  uint pinsw = 22, pinclk = 17, pindt = 27;

  chipNUMBER = ui->lineEditChipNumber->text();
  pinSW = ui->lineEditRTCPin1->text();
  pinCLK = ui->lineEditRTCPin2->text();
  pinDT = ui->lineEditRTCPin3->text();

  // check if they are actually numbers
  bool ok;

  if (chipNUMBER.toInt(&ok))
    chipnumber = chipNUMBER.toInt();
  if (pinSW.toInt(&ok))
    pinsw = pinSW.toInt();
  if (pinCLK.toInt(&ok))
    pinclk = pinCLK.toInt();
  if (pinDT.toInt(&ok))
    pindt = pinDT.toInt();

  // set pins and chip numbers
  _workerrtc->setChipnumber(chipnumber);
  _workerrtc->setPins(pinsw, pinclk, pindt);

  // invoke the initialisation
  bool success = false;
  _workerrtc->initialize(&success);

  // start the event loop
  if (success) {
    // success = false; // useless because its a queuedconnection
    QMetaObject::invokeMethod(_workerrtc, "run", Qt::QueuedConnection,
                              Q_ARG(bool*, &success));
  }
  *_statusRTC = success;

  // send message successful or not
  if (!*_statusRTC) {
    QMessageBox::warning(this, "Error",
                         "NO Rotary Encoder could be initialized");
  }

  else {
    QMessageBox::information(this, "Success",
                             "Rotary Encoder succesfully initialized");
  }
}

void DialogSettings::toggleRTCButtons(bool checked) {
  _optionsActivatedRTC = checked;
  ui->lineEditChipNumber->setEnabled(checked);
  ui->lineEditRTCPin1->setEnabled(checked);
  ui->lineEditRTCPin2->setEnabled(checked);
  ui->lineEditRTCPin3->setEnabled(checked);
  ui->pushButtonInitializeRTC->setEnabled(checked);
}

void DialogSettings::saveSettings() {
  if (!_optionsActivatedRTC) {
    _workerrtc->stop();
    _workerrtc->disconnect();
    *_statusRTC = false;
  }
  if (!_optionsActivatedOLED) {
      _oled->turnOff();
      *_statusOled = false;
  }
  *_apiKey = ui->lineEditApiKey->text();
  this->close();
}

void DialogSettings::cancel() {
  this->close();
}

void DialogSettings::showOledData() {
  ui->lineEditi2cAdress->setText(QString::fromStdString(_oled->getAdress()));
  ui->lineEditi2cBus->setText(QString::fromStdString(_oled->getBus()));
}

void DialogSettings::showRTCData() {
  ui->lineEditChipNumber->setText(QString::number(_workerrtc->getChipnumber()));
  ui->lineEditRTCPin1->setText(QString::number(_workerrtc->getPinSW()));
  ui->lineEditRTCPin2->setText(QString::number(_workerrtc->getPinCLK()));
  ui->lineEditRTCPin3->setText(QString::number(_workerrtc->getPinDT()));
}

void DialogSettings::toggleOledButtons(bool checked) {
  _optionsActivatedOLED = checked;
  ui->pushButtonAutoDetectOled->setEnabled(checked);
  ui->pushButtonInitializeOled->setEnabled(checked);
  ui->lineEditi2cAdress->setEnabled(checked);
  ui->lineEditi2cBus->setEnabled(checked);
}
