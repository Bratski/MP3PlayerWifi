#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent, COled *oled, QString *apikey,
                               bool *statusoled, QThread *rtcthread,
                               CRotaryEncoderWorker *workerrtc, bool *statusrtc)
    : QDialog(parent), ui(new Ui::DialogSettings), _oled(oled), _apiKey(apikey),
      _statusOled(statusoled), _rtcthread(rtcthread), _workerrtc(workerrtc),
      _statusRTC(statusrtc) {
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
    QMessageBox::warning(this, "Error", "no Oled-display could be initialized");
  } else {
    QMessageBox::information(this, "Success",
                             "Oled-display succesfully initialized");
  }
}

void DialogSettings::initializeRTC() {
  // hasnt the thread been started yet?
  if (!_rtcthread->isRunning())
    _rtcthread->start();
  // get the pin numbers from the textlines in the dialog
  QString pinSW, pinCLK, pinDT;

  pinSW = ui->lineEditRTCPin1->text();
  pinCLK = ui->lineEditRTCPin2->text();
  pinDT = ui->lineEditRTCPin3->text();

  // check if they are actually numbers
  bool ok;

  if (pinSW.toInt(&ok))
    _pinSW = pinSW.toInt();
  if (pinCLK.toInt(&ok))
    _pinSW = pinSW.toInt();
  if (pinDT.toInt(&ok))
    _pinSW = pinDT.toInt();

  // invoke the initialisation
  bool success = false;
  QMetaObject::invokeMethod(_workerrtc, "setPins", Qt::BlockingQueuedConnection,
                            _pinSW, _pinCLK, _pinDT);
  QMetaObject::invokeMethod(_workerrtc, "initialize",
                            Qt::BlockingQueuedConnection, &success);

  if (!success) {
    _rtcthread->quit();
    _rtcthread->wait();
    QMessageBox::warning(this, "Error",
                         "Rotary Encoder could NOT be initialized");
  }

  // send message successful or not

  else {

    // TODO sync the volumes
    // QMetaObject::invokeMethod(_workerrtc, "setRotaryCounter",
    //                           Qt::BlockingQueuedConnection, int());
    QMetaObject::invokeMethod(_workerrtc, "run", Qt::QueuedConnection);
    QMessageBox::information(this, "Success",
                             "Rotary Encoder succesfully initialized");
  }
}

void DialogSettings::toggleRTCButtons(bool checked) {
  *_statusRTC = checked;
  ui->lineEditRTCPin1->setEnabled(*_statusRTC);
  ui->lineEditRTCPin2->setEnabled(*_statusRTC);
  ui->lineEditRTCPin3->setEnabled(*_statusRTC);
  ui->pushButtonInitializeRTC->setEnabled(*_statusRTC);

  if (!*_statusRTC && _rtcthread->isRunning()) {
    QMetaObject::invokeMethod(_workerrtc, "stop", Qt::BlockingQueuedConnection);
    _rtcthread->quit();
    _rtcthread->wait();
  }
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
  if (_rtcthread->isRunning()) {
    uint pin1, pin2, pin3;
    QMetaObject::invokeMethod(_workerrtc, "getPinSWITCH",
                              Qt::BlockingQueuedConnection, &pin1);
    QMetaObject::invokeMethod(_workerrtc, "getPinCLK",
                              Qt::BlockingQueuedConnection, &pin2);
    QMetaObject::invokeMethod(_workerrtc, "getPinDT",
                              Qt::BlockingQueuedConnection, &pin3);

    ui->lineEditRTCPin1->setText(QString::number(pin1));
    ui->lineEditRTCPin2->setText(QString::number(pin2));
    ui->lineEditRTCPin3->setText(QString::number(pin3));
  }
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
