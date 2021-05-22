#include "sms.h"
#include "ui_sms.h"

#include <QTableWidget>
#include <QMdiSubWindow>

SMS::SMS(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SMS)
{
    ui->setupUi(this);
    setWindowTitle("Student Management System");

    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    ui->mdiArea->setTabPosition(QTabWidget::North);
    ui->mdiArea->setTabShape(QTabWidget::Rounded);
    ui->mdiArea->setTabsClosable(true);
    ui->mdiArea->setTabsMovable(true);

    this->tableIndex = -1;

    //状态栏（但是显示不出来）
    currentTimeLabel = new QLabel;
    ui->statusBar->addWidget(currentTimeLabel);
    currentTimeLabel->show();
    ui->statusBar->show();
    QTimer *timer = new QTimer(this);
    timer->start(1000); //每隔1000ms发送timeout的信号
    connect(timer, SIGNAL(timeout()),this,SLOT(timeUpdate()));

    //工具栏和状态栏
    this->ui->actionToolbar->setChecked(true);
    this->ui->actionStatus->setChecked(true);
}

SMS::~SMS()
{
    delete ui;
}

int SMS::getActiveTalbeIndex()
{
    if(this->ui->mdiArea->activeSubWindow()==nullptr){
        return -1;
    }
    int i = this->WindMap.find(this->ui->mdiArea->activeSubWindow()).value();
    qDebug()<<"This index is"<<i<<endl;
    return i;
}

void SMS::closeEvent(QCloseEvent *event)
{
    //自定义关闭事件，询问是否关闭
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList()){
        if(window->isActiveWindow()){
            int i = this->WindMap.find(window).value();
            subWnds[i]->closeEvent(event);
        }
    }
}

void SMS::on_actionNew_triggered()
{
    StuSubWindow *newWnd = new StuSubWindow();    subWnds.push_back(newWnd);

    QMdiSubWindow *child = ui->mdiArea->addSubWindow(newWnd);
    tableIndex++;
    WindMap.insert(child,tableIndex);


    //显示的是newWnd的名字，因此只要设置newWnd的名字就好
    newWnd->resize(700,700);
    newWnd->setWindowTitle(tr("学生信息表%1").arg(tableIndex+1));
    child->resize(700,700);
    child->show();
}

void SMS::on_actionAdd_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->addLine();
}

void SMS::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("关于 StudentMS"),
            tr("<p>"
               "<b>StudentMS</b>是是一个学生信息管理程序，是南京大学计算机科学与技术系《高级程序设计》课程的大作业"
               "该程序使用C++和Qt框架开发，可以实现了学生信息表的创建、保存、打开等功能。<br/>"
               "程序还处于迭代中，欢迎使用。<br/>"
               "有任何疑问可以发送邮件<a href=\"mailto:895254752@qq.com\">895254752@qq.com</a><br/>"
               "更多详情，可以到我的github pages上查看<a href=\"https://tsunaou.github.io/StudentMS.html\" target=\"_blank\"><b>Tsuna的学生管理系统</b></a>"
               "<hr/>"
               "<p style=\"text-align: right\">欧阳鸿荣 2018.12</p></p>"
               ));
}

bool SMS::showLoginBox()
{
    LoginDialog *login = new LoginDialog();
    while(true){
        if(login->exec() == QDialog::Accepted){
            if(login->getUSER()!=USERNAME || login->getPWD()!=PASSWORD){
                QMessageBox::warning(this,tr("提示"),
                         tr("请输入正确的用户名或密码"));
                continue;
            }else{
                QMessageBox::information(this,tr("登陆成功"),
                         tr("登陆成功，欢迎你"));
                return true;
            }
        }else if(login->exec() == QDialog::Rejected){
            return false;
        }else{
            return false;
        }
    }

}

void SMS::timeUpdate()
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString timestr = current_time.toString( "yyyy年MM月dd日 hh:mm:ss"); //设置显示的格式
    currentTimeLabel->setText(timestr); //设置label的文本内容为时间
}

void SMS::on_actionAlter_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->alterLine();
}

void SMS::on_actionDelete_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->deleteLine();
}

void SMS::on_actionSortID_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->sortByID();
}

void SMS::on_actionSortName_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->sortByName();
}

void SMS::on_actionClose_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->close();
}

void SMS::on_actionSave_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->saveFile();
}


void SMS::on_actionSaveAs_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->saveFileAs();
}


void SMS::on_actionOpen_triggered()
{
    //打开文件
    //得到文件路径和文件名，在检验合法性后以这个文件名创建窗口，然后导入文件内容

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("打开文件"),QDir::currentPath(),tr("Text File (*.txt)"));

    //检验该路径的文件是否已经被打开过了,如果是，则将当前活动窗口切换给他
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList()){
        if(window->isActiveWindow()){
            int i = this->WindMap.find(window).value();
            if(subWnds[i]->getFilePath() == fileName){
                //当前活动窗口中有当前路径的
                ui->mdiArea->setActiveSubWindow(window);
                return;
            }
        }
    }


    if(fileName == ""){//如果用户直接关闭了文件浏览对话框，那么文件名就为空值，直接返回
        return;
    }
    else{
       QFile file(fileName);
       if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           QMessageBox::warning(this,tr("错误"),tr("打开文件失败"));
           return;
       }
       else
       {
           if(!file.isReadable())
           {
               QMessageBox::warning(this,tr("错误"),tr("该文件不可读"));
           }
           else
           {
               //该文件可以成功打开

               //文件标识，系统只能打开特定格式文件（防止出错）
               QTextStream textStream(&file);
               QString lineInfo;
               lineInfo = textStream.readLine();
               if(lineInfo != FILE_KEY){
                   QMessageBox::warning(this,tr("错误"),tr("该文件不兼容于该系统"));
                   file.close();
                   return;
               }

               //是目标对象的文件,可以创建新窗口然后导入
               this->on_actionNew_triggered();
               int activeIndex = this->getActiveTalbeIndex();
               if(activeIndex == -1){
                   //理论上不会运行到这里
                   QMessageBox::warning(this,tr("提示"),
                            tr("您当前未创建(或打开)文件，请先创建一个文件。"));
                   return;
               }

               QFileInfo fi = QFileInfo(fileName);
               QStringList sections = fi.fileName().split(QRegExp("[.]")); //分割birthday
               this->ui->mdiArea->activeSubWindow()->setWindowTitle(sections.at(0));
               subWnds[activeIndex]->setWindowTitle(fi.fileName());
               subWnds[activeIndex]->importFile(fileName);

           }
           file.close();
       }
    }
}



void SMS::on_actionTabBar_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::TabbedView); //设置视口模式：tabBar模式
}

void SMS::on_actionWindow_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView); //设置视口模式：tabBar模式
}

void SMS::on_actionSascadeSubWindows_triggered()
{
    ui->mdiArea-> cascadeSubWindows();
}

void SMS::on_actionTileSubWindows_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void SMS::on_actionSort_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->sortByUser();
}

void SMS::on_actionFilter_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->filter();
}

void SMS::on_actionToolbar_toggled(bool arg1)
{
    if(arg1){
        this->ui->toolBar->show();
        this->ui->mainToolBar->show();
    }else{
        this->ui->toolBar->hide();
        this->ui->mainToolBar->hide();
    }
}

void SMS::on_actionStatus_toggled(bool arg1)
{
    if(arg1){
        this->ui->statusBar->show();
    }else{
        this->ui->statusBar->hide();
    }
}

void SMS::on_actionPrint_triggered()
{
    int activeIndex = this->getActiveTalbeIndex();
    if(activeIndex == -1){
        QMessageBox::warning(this,tr("提示"),
                 tr("您当前未创建(或打开)文件，请先创建一个文件。"));
        return;
    }
    subWnds[activeIndex]->printTable();
}

void SMS::on_actionOriginQss_triggered()
{
    StyleSelector::setStyle(":/qss/stylesheet/none.qss");
}

void SMS::on_actionQssBlue_triggered()
{
    StyleSelector::setStyle(":/qss/stylesheet/studentMS.qss");
}


