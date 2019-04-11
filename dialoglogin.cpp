#include "dialoglogin.h"
#include "ui_dialoglogin.h"

DialogLogin::DialogLogin(QTranslator *translator,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLogin)
{
    ui->setupUi(this);
    this->setObjectName("LoginWidget");
    this->setStyleSheet("#LoginWidget{border-image: url(:/Title/login_w.png)}");
    this->setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    loginTrans = translator;

    Model_iniSetting iniRead;
    ui->comboBox_name->setCurrentText(iniRead.ReadIni_user("userName").toString());
    ui->lineEdit_password->setText(iniRead.ReadIni_user("userPassword").toString());
    ui->comboBoxLanguage->setCurrentIndex(iniRead.ReadIni_user("language").toInt());

    if(ui->comboBoxLanguage->currentIndex()==l_Chinese)
        on_comboBoxLanguage_currentIndexChanged(l_Chinese);

}

DialogLogin::~DialogLogin()
{
    delete ui;
}


void DialogLogin::accept()
{
    QString name = ui->comboBox_name->currentText();
    QString password = ui->lineEdit_password->text();
    if(name == "User")
    {
        userLogin.Permissions = OnlyUser;
    }
    else if(name == "Administrator")
    {
        if(password != "123.com")
        {
            ui->label_show->setText("password is Error.");
            return ;
        }
        userLogin.Permissions = Administrator;
    }
    else if(name == "Designer")
    {
        if(password != "123.com")
        {
            ui->label_show->setText("password is Error.");
            return ;
        }
        userLogin.Permissions = Designer;
    }

    userLogin.userName = name;
    userLogin.userPassword = password;
    userLogin.language = ui->comboBoxLanguage->currentIndex();

    Model_iniSetting iniWrite;
    iniWrite.WriteIni_user("userName",userLogin.userName);
    iniWrite.WriteIni_user("userPassword",userLogin.userPassword);
    iniWrite.WriteIni_user("language",userLogin.language);

    QDialog::accept();
}

void DialogLogin::on_comboBoxLanguage_currentIndexChanged(int index)
{
    if(index == l_Chinese)
        loginTrans->load("uav_tr_laCN");
    else if(index == l_English)
        loginTrans->load("uav_tr_laEN");

    ui->label->setText(tr("自动化测试系统"));
    ui->label_2->setText(tr("用户："));
    ui->label_3->setText(tr("密码："));
    //ui->retranslateUi(this);
}
