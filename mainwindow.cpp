#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cp2112.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("HIDtoSMBus bridge");
    model = new QStandardItemModel(0, 1, this);
    model->setHorizontalHeaderLabels({"Время","Команда","Название","Значение","Операция","Тип данных"});
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() //Подключить мост
{
    INT DevNumbers = CP2112_Find1(); //Сколько устройств подключено
    if (DevNumbers==0)
    {
        ui->labelStaus->setText("CP2112 не подключено");
        ui->labelStaus->setStyleSheet("background-color: #ff8282;");
        return;
    }
    else
    {
        ui->labelStaus->setText("CP2112 подключено");
        ui->labelStaus->setStyleSheet("background-color: lightGreen;");
    }
    if (!CP2112_Open(0,&m_hidSmbus)) //Если не открылось
    {
        HidSmbus_Close(m_hidSmbus);
        return;
    }
    else
    {
        CP2112_SetConfig(m_hidSmbus); //Передаем конфиг на устройство

        HidSmbus_GetSmbusConfig(m_hidSmbus, &bitRate, //Теперь читаем конфиг с устройства, чтобы убедится, что он записался
                                &address, &autoReadRespond,
                                &writeTimeout, &readTimeout,
                                &sclLowTimeout, &transferRetries);

        HID_SMBUS_DEVICE_STR serial={0};
        HID_SMBUS_DEVICE_STR manufacturer={0};
        HID_SMBUS_DEVICE_STR product={0};
        HidSmbus_GetOpenedString(m_hidSmbus,serial,HID_SMBUS_GET_SERIAL_STR);
        HidSmbus_GetOpenedString(m_hidSmbus,manufacturer,HID_SMBUS_GET_MANUFACTURER_STR);
        HidSmbus_GetOpenedString(m_hidSmbus,product,HID_SMBUS_GET_PRODUCT_STR);

        ui->labelManufactur->setText(manufacturer); //Тут полученный конфиг записываем на экран
        ui->labelProduct->setText(product);
        ui->labelSpeed->setText(QString::number(bitRate));
        ui->labelAddress->setText(QString::number(address));
        HidSmbus_GetTimeouts(m_hidSmbus,&response_timeout_ms); //Время ответа узнаем
        setConfigText(bitRate, writeTimeout,readTimeout, transferRetries,response_timeout_ms,autoReadRespond,sclLowTimeout); //запись на экран конфига

        ui->setConfigButton->setEnabled(true);
        ui->getConfigButton->setEnabled(true);
    }
}

void MainWindow::on_pushButton_6_clicked() //Задать слейв адрес
{
    ui->lbl_P_2->setText(ui->editAddress->text());
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked() //Прием данных с STM
{
    bool ok;
    uint16_t adress = ui->lbl_P_2->text().toInt(&ok,16);

    HID_SMBUS_S0	status0;
    QString receiveString;
    BYTE			buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE];
    BYTE                numBytesRead = 0;
    BYTE bufferWrite[4] = {0x01,0x02,0x03,0x04};
    BYTE data[1];
    QLabel *BatteryModeBitFlag [4] = { ui->lbl_Uin,ui->lbl_Uout,ui->lbl_I,ui->lbl_P };
    QLabel *DataFlag[4] = {ui->labelVoltIn,ui->labelVoltOut,ui->labelI,ui->labelP};
    for(int i = 0;i < 4 ; i++) {
        data[0] = bufferWrite[i];
        HidSmbus_WriteRequest(m_hidSmbus, adress, data, 1);        //Записываем что то на устройство отправляя буфер с данными
        QThread::msleep(10);
        HidSmbus_ReadRequest(m_hidSmbus, adress, 1);               //Иниализируем чтение
        HidSmbus_ForceReadResponse(m_hidSmbus, 1);                 //ответ на чтение хосту после того, как была выполнена передача чтения.
        if(HidSmbus_GetReadResponse(m_hidSmbus,&status0,buffer,HID_SMBUS_MAX_READ_RESPONSE_SIZE,&numBytesRead) == HID_SMBUS_SUCCESS) { //Тут уже читаем что пришло
            if(status0 == 2) { // статус 2 означает что все хорошо
                ui->labelStatusGet->setText("Чтение успешно");
                ui->labelStatusGet->setStyleSheet("background-color:  lightGreen;");
                receiveString = QString::number((float)buffer[0]/10);
                BatteryModeBitFlag[i]->setText(receiveString);
                QThread::msleep(10);
                DataFlag[i]->setStyleSheet("background-color: lightGreen;");
            }
            else  {
                DataFlag[i]->setStyleSheet("background-color: #ff8282");
                ui->labelStatusGet->setText("Ошибка чтения");
                ui->labelStatusGet->setStyleSheet("background-color: #ff8282;");
            }
        }
    }
    CP2112_SetGpioConfig(m_hidSmbus); //Чтобы светодиоды моргали при передаче или приеме
}

void MainWindow::on_pushButton_4_clicked() //Получить данные по пмбас
{
    bool ok;
    uint16_t adress = ui->lbl_P_2->text().toInt(&ok,16);
    QLabel*BitFlagsStatus[16] = { //Массив из строк интерфейса
        ui->StatusNONE_OF,ui->StatusCML,ui->StatusTEMP,ui->StatusVIN_UV,
        ui->StatusIOUT_OC,ui->StatusVOUT_OV,ui->StatusOFF,ui->StatusBUSY,
        ui->StatusUNKNOWN,ui->StatusOTHER,ui->StatusFANS,ui->StatusPG,
        ui->StatusMRFC,ui->StatusINPUT,ui->StatusIOUTPOUT,ui->StatusVOUT
    };
    for (BYTE i=0; i<16;i++)
    {
        INT     bitFlags;
        if (ReadWord(m_hidSmbus, &bitFlags, 0x79,adress)) //Получили слово где каждый бит отображает статус
        {
            std::bitset<16> bits = bitFlags;
            BOOL y;
            y=bits.test(i);
            switch (y)
            {
            case 1: //1 это значение бита
                BitFlagsStatus[i]->setStyleSheet("background-color:  #ff8282");
                break;
            case 0:
                BitFlagsStatus[i]->setStyleSheet("background-color: lightGreen");
                break;
            }
            ui->labelStatusGet_2->setText("Чтение успешно");
            ui->labelStatusGet_2->setStyleSheet("background-color:  lightGreen");
        }
        else {
            BitFlagsStatus[i]->setStyleSheet("default");
            ui->labelStatusGet_2->setText("Ошибка чтения");
            ui->labelStatusGet_2->setStyleSheet("background-color:  #ff8282");
        }
    }
    const INT Addresses [7] =
    {
        0x88,   // Uin
        0x89,   // Iin
        0x21,   // VOUT_TRANSITION_RATE
        0x8c,   // Iout
        0x8d,   // TEMPERATURE1
        0x8e,   // TEMPERATURE2
        0x96,   // Pout
    };

    QLabel*DataPMBus[7] = {ui->lblUin, ui->lblIin,ui->lblUout,ui->lblIout,ui->lblTmp1,ui->lblTmp2,ui->lblPout }; //Массивы строк
    QLabel*DataPMBusStatus[7] = {ui->FlagVin,ui->FlagIin,ui->FlagVout,ui->FlagIout,ui->FlagTm1,ui->FlagTm2,ui->FlagPout};

    for (int i = 0; i < 7; i++) {
        QString str="";
        INT     data_word;
        INT     addr=Addresses[i];
        switch (addr)
        {
        case 0x88:  // Uin
        case 0x89:  // Iin
        case 0x8c:  // Iout
        case 0x8d:  // TEMPERATURE1
        case 0x8e:  // TEMPERATURE2
        case 0x96:  // Pout
        {
            if (ReadWord(m_hidSmbus, &data_word, Addresses[i],adress)) //Читаем что получили по команде
            {
                str = QString::number((float)data_word); //Преобразем в строку и ниже выводим на экран
                DataPMBus[i]->setText(str);
                DataPMBusStatus[i]->setStyleSheet("background-color: lightGreen");
            }
            else DataPMBusStatus[i]->setStyleSheet("background-color: #ff8282");
        }
            break;
        case 0x21: if (ReadWord(m_hidSmbus, &data_word, Addresses[i],adress))
            {
                str = QString::number((float)data_word/256);
                DataPMBus[i]->setText(str);
                DataPMBusStatus[i]->setStyleSheet("background-color: lightGreen");
            }
            else DataPMBusStatus[i]->setStyleSheet("background-color: #ff8282");
            break; // VOUT_TRANSITION_RATE
        }
    }
}

void MainWindow::on_setConfigButton_clicked() //Отправить параметры настроеек
{
    BOOL                opened;
    BOOL autoReadResponse = ui->checkBoxAutoResponse->checkState();
    BOOL scl_low_timeout = ui->checkBoxSCL->checkState();
    HID_SMBUS_STATUS    status;
    DWORD bitRate = ui->lineEditBitRate->text().toInt(); //Преобразуем значения из строк в нужный тип данных
    DWORD response_timeout_ms = ui->lineEditResponseTime->text().toInt();
    WORD write_timeout_ms = ui->lineEditWriteTime->text().toInt();
    WORD read_timeout_ms = ui->lineEditReadTime->text().toInt();
    WORD transfer_retries = ui->lineEditRetries->text().toInt();

    if(HidSmbus_IsOpened(m_hidSmbus, &opened) == HID_SMBUS_SUCCESS && opened) //Если открыток
    {
        status =  HidSmbus_SetSmbusConfig(m_hidSmbus, bitRate, ACK_ADDRESS, autoReadResponse, write_timeout_ms, read_timeout_ms, scl_low_timeout, transfer_retries); //Задаем конфиг
        if(status != HID_SMBUS_SUCCESS)
        {
            ui->labelStatusParametrs->setText("Ошибка");
            ui->labelStatusParametrs->setStyleSheet("background: #ff8282");
        }
        HidSmbus_SetTimeouts(m_hidSmbus, response_timeout_ms); //Задаем время на ответ
        ui->labelStatusParametrs->setText("Данные заданы");
        ui->labelStatusParametrs->setStyleSheet("background: lightGreen");
    }
}

void MainWindow::on_getConfigButton_clicked() //Получить параметры настроеек
{
    if(HidSmbus_GetSmbusConfig(m_hidSmbus, &bitRate,&address, &autoReadRespond, //Получили конфиг
                               &writeTimeout, &readTimeout,&sclLowTimeout, &transferRetries) == HID_SMBUS_SUCCESS) {
        ui->labelStatusParametrs->setText("Данные приняты");
        ui->labelStatusParametrs->setStyleSheet("background: lightGreen");
    }
    HidSmbus_GetTimeouts(m_hidSmbus,&response_timeout_ms); //Получили время на ответ
    setConfigText(bitRate, writeTimeout,readTimeout, transferRetries,response_timeout_ms,autoReadRespond,sclLowTimeout); //Отобразили на экран
}

void MainWindow::on_readWordButton_clicked() //Чение пользователем команды PMBus
{
    bool ok;
    uint16_t adress = ui->lbl_P_2->text().toInt(&ok,16);
    uint16_t commandAdres = ui->lineEditCommandAdrRead->text().toInt(&ok,16);
    INT     data_word;
    BYTE byte;
    QString str;
    switch (ui->comboBoxType->currentIndex()) { //какой тип данных (в свитче индекс обьекта из комбобокса типо 0 индекс у /256)
    case 0 :
        ReadWord(m_hidSmbus, &data_word,commandAdres,adress);
        str = QString::number((float)data_word/256);
        break;
    case 1 :
        ReadWord(m_hidSmbus, &data_word,commandAdres,adress);
        str = QString::number((float)data_word);
        break;
    case 2 :
        ReadByte(m_hidSmbus,&byte,commandAdres,adress);
        str = QString::number(byte,16);
        break;
    }
    QList<QStandardItem*> newRow; //Заполняем массив для модели которая отображается с помощью tableView
    newRow << new QStandardItem(QTime::currentTime().toString("HH:mm:ss"));
    newRow << new QStandardItem(ui->lineEditCommandAdrRead->text());
    newRow << new QStandardItem(ui->lineEditNameRead->text());
    newRow << new QStandardItem(str);
    newRow << new QStandardItem("Чтение");
    newRow << new QStandardItem(ui->comboBoxType->currentText());
    model->appendRow(newRow);
}

void MainWindow::on_deleteButton_clicked() //Удаление строчки
{
    model->removeRows(ui->tableView->currentIndex().row(),1);
}

void MainWindow::on_writeButton_clicked() //Записать значение по команде PMBus
{
    bool ok;
    uint16_t adress = ui->lbl_P_2->text().toInt(&ok,16);
    uint16_t commandAdres = ui->lineEditCommandAdrWrite->text().toInt(&ok,16);
    WORD rawValue = ui->lineEditValueWrite->text().toInt(&ok,16);
    BYTE bufferUser[3];
    BYTE                numBytesWrite = 3;
    switch (ui->comboBoxType_2->currentIndex()) { //Смотрим какой тип данных и заполняем буфер
    case 0 :
        rawValue = rawValue*256;
        bufferUser[0] = commandAdres;
        bufferUser[1] = rawValue & 0xFF;//нижний бит
        bufferUser[2] = (rawValue >> 8) & 0xFF;//верхний бит
        break;
    case 1 : // для обычного слова
        bufferUser[0] = commandAdres;
        bufferUser[1] = rawValue & 0xFF;
        bufferUser[2] = (rawValue >> 8) & 0xFF;
        break;
    case 2 : // для байта
        bufferUser[0] = commandAdres;
        bufferUser[1] = rawValue;
        numBytesWrite = 2;
        break;
    }
    HidSmbus_WriteRequest(m_hidSmbus, adress << 1, bufferUser, numBytesWrite); //Запись на устргйство
    QList<QStandardItem*> newRow;  //Заполняем данные для модели которая отображается с помощью tableView
    newRow << new QStandardItem(QTime::currentTime().toString("HH:mm:ss"));
    newRow << new QStandardItem(ui->lineEditCommandAdrWrite->text());
    newRow << new QStandardItem(ui->lineEditNameWrite->text());
    newRow << new QStandardItem(ui->lineEditValueWrite->text());
    newRow << new QStandardItem("Запись");
    newRow << new QStandardItem(ui->comboBoxType_2->currentText());
    model->appendRow(newRow);
}

void MainWindow::setConfigText(DWORD bit, WORD writeTim, WORD readTim, WORD transferRetr,DWORD responseTim,BOOL autoReadResp,BOOL sclLowTim)
{
    ui->lineEditBitRate->setText(QString::number(bit));
    ui->lineEditWriteTime->setText(QString::number(writeTim));
    ui->lineEditReadTime->setText(QString::number(readTim));
    ui->lineEditRetries->setText(QString::number(transferRetr));
    ui->lineEditResponseTime->setText(QString::number(responseTim));
    ui->checkBoxAutoResponse->setChecked(autoReadResp);
    ui->checkBoxSCL->setChecked(sclLowTim);
}
