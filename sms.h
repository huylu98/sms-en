#ifndef SMS_H
#define SMS_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QVector>
#include <QMap>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QLabel>
#include <QTimer>

#include "stusubwindow.h"
#include "studialog.h"
#include "logindialog.h"
#include "styleselector.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SMS; }
QT_END_NAMESPACE

class SMS : public QMainWindow
{
    Q_OBJECT

public:
    explicit SMS(QWidget *parent = nullptr);
    ~SMS();
    int getActiveTalbeIndex();
    bool showLoginBox();
    void timeUpdate();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    //File
    void on_actionNew_triggered();
    void on_actionClose_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionOpen_triggered();
    void on_actionPrint_triggered();


    //Edit
    void on_actionAdd_triggered();
    void on_actionAlter_triggered();
    void on_actionDelete_triggered();
    void on_actionSortID_triggered();
    void on_actionSortName_triggered();
    void on_actionSort_triggered();
    void on_actionFilter_triggered();

    //View
    void on_actionToolbar_toggled(bool arg1);
    void on_actionStatus_toggled(bool arg1);


    //WindowWindow
    void on_actionTabBar_triggered();
    void on_actionWindow_triggered();
    void on_actionTileSubWindows_triggered();
    void on_actionSascadeSubWindows_triggered();

    //About
    void on_actionAbout_triggered();

    //Theme
    void on_actionOriginQss_triggered();
    void on_actionQssBlue_triggered();


private:
    Ui::SMS *ui;
    QVector<StuSubWindow*> subWnds;     //维护子窗口
    QMap<QMdiSubWindow*,int> WindMap;   //存储当前活动的窗口
    QMap<QString,int> OpenMap;          //存储当前打开的文件(后期扩展可以防止打开同一个窗口)
    int tableIndex;                     //子窗口的数目
    QLabel* currentTimeLabel;           //时间戳（没用到）
};
#endif // SMS_H
