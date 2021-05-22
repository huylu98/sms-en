#include "logindialog.h"
#include "ui_logindialog.h"
#include "config.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->UserEdit->setPlaceholderText("Please enter the administrator username");
    ui->PwdEdit->setEchoMode(QLineEdit::Password);
    ui->PwdEdit->setPlaceholderText("Please enter the password (no more than 11 digits)");
    ui->PwdEdit->setMaxLength(11);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=](){ qDebug("ok or save");}); //绑定
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [=](){ qDebug("cancel"); });
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getUSER()
{
    return ui->UserEdit->text();
}

QString LoginDialog::getPWD()
{
    return ui->PwdEdit->text();
}
