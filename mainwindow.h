#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SLABCP2112.h"
#include "QStringListModel"
#include "QStandardItemModel"
#include "QTime"
#include <QDebug>
#include <QThread>
#include "SMBusConfig.h"
#include "stmcomands.h"
#include <bitset>
#include "string.h"
#include <QString>
#include <QStringList>
#include <iostream>
#include <QListView>
#include <QStringList>
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    HID_SMBUS_DEVICE    m_hidSmbus;
    DWORD bitRate;              //Для этого нужно объявить кучу переменных
    BYTE address;
    BOOL autoReadRespond;
    WORD writeTimeout;
    WORD readTimeout;
    BOOL sclLowTimeout;
    WORD transferRetries;
    DWORD response_timeout_ms;
    QStandardItemModel*model;
    QString time = QTime::currentTime().toString("HH:mm:ss");
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setConfigText(DWORD bit, WORD writeTim, WORD readTim, WORD transferRetr, DWORD responseTim, BOOL autoReadResp, BOOL sclLowTim);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_setConfigButton_clicked();

    void on_getConfigButton_clicked();

    void on_readWordButton_clicked();

    void on_deleteButton_clicked();

    void on_writeButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
};
#endif // MAINWINDOW_H
