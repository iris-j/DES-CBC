#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "des.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include<cstring>
#include<QFileDialog>
#include<QMessageBox>
#include<iostream>
#include<stdlib.h>
#include<time.h>
extern string input;
extern string output;
extern bitset<64> initkey;
extern bitset<48> subkey[8];
extern bitset<64> IV;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->encrypt,SIGNAL(clicked()),this,SLOT(push_encrypt()));
    connect(ui->load,SIGNAL(clicked()),this,SLOT(fileread()));
    connect(ui->decode,SIGNAL(clicked()),this,SLOT(push_decode()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::fileread()
{
   QString path=QFileDialog::getOpenFileName(this,tr("Open code file"),"/",tr("Txt Files(*.txt)"));
   if(path.length()==0)
       QMessageBox::information(NULL,tr("Path"),tr("You didn't select any files."));
   else
       QMessageBox::information(NULL,tr("Path"),tr("You selected")+path);
   QFile *file=new QFile(path);
   file->open(QIODevice::ReadOnly|QIODevice::Text);
   QTextStream in(file);
   QString data = in.readAll();
   input=data.toStdString();
   file->close();
}

void MainWindow::filewrite(string output)
{
    QString path=QFileDialog::getOpenFileName(this,tr("Save to file"),"/",tr("Txt Files(*.txt)"));
    if(path.length()==0)
        QMessageBox::information(NULL,tr("Path"),tr("You didn't select any files."));
    else
        QMessageBox::information(NULL,tr("Path"),tr("You selected")+path);
    QFile *file=new QFile(path);
    file->open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(file);
    out<<QString::fromStdString(output);
    file->close();
}

/*
void MainWindow::push_encrypt(){//字符串版本的
    srand((unsigned)time(NULL));
    QString qsinitkey = ui->inputkey->toPlainText();
    initkey = string_to_bitset(qsinitkey.toStdString());
    getsubkey(initkey);
    string sIV;
    for(int i=0; i<64; i++)
        sIV+=char(rand() % 2 + '0');
    IV = binary_to_bitset(sIV);
    output+=bitset_to_binary(encrypt(IV));
    //对不足位的进行无用字符填充
    int remain = input.length()-input.length()/8*8;//多余的字节数
    for(int i=0; i<7-remain; i++)
        input+="h";
    input+=7-remain+'0';

    //IV承担Y[i-1]的功能
    bitset<64> plain, cipher;
    for(int i=0; i<input.length();i=i+8){
        plain = string_to_bitset(input.substr(i,8));
        cipher = encrypt(plain ^ IV);
        IV = cipher;
        output+=bitset_to_binary(cipher);
    }
    ui->display->setText(QString::fromStdString(output));
    filewrite(output);
    QMessageBox message(QMessageBox::NoIcon, "Information", "Successful Encode!");
    message.setIconPixmap(QPixmap("encryption.PNG"));
    message.exec();


}
//QString::fromStdString()
void MainWindow::push_decode(){
    QString qsinitkey = ui->inputkey->toPlainText();
    initkey = string_to_bitset(qsinitkey.toStdString());
    getsubkey(initkey);
    bitset<64> plain, cipher;
    output="";
    IV = decode(binary_to_bitset(input.substr(0, 64)));
    for(int i=64;i<input.length();i=i+64){
        cipher = binary_to_bitset(input.substr(i,64));
        plain = decode(cipher)^IV;
        IV = cipher;
        output+=bitset_to_string(plain);
    }
    int del = output[output.length()-1]-'0';
    ui->display->setText(QString::fromStdString(output.substr(0, output.length()-del-1)));
    filewrite(output.substr(0, output.length()-del-1));
    QMessageBox message(QMessageBox::NoIcon, "Information", "Successful Decode!");
    message.setIconPixmap(QPixmap("decode.PNG"));
    message.exec();
}

*/

void MainWindow::push_encrypt(){//01串版本的
    srand((unsigned)time(NULL));
    QString qsinitkey = ui->inputkey->toPlainText();
    initkey = binary_to_bitset(qsinitkey.toStdString());
    getsubkey(initkey);
    string sIV;
    for(int i=0; i<64; i++)
        sIV+=char(rand() % 2 + '0');
    IV = binary_to_bitset(sIV);
    output+=bitset_to_binary(encrypt(IV));
    //对不足位的进行随机01字符填充
    int remain = input.length()-input.length()/64*64;//多余的bit数
    int fill = remain<56?(56-remain):(120-remain);
    for(int i=0; i<fill; i++)
        input+=char(rand() % 2 + '0');
    for(int i=0; i<8; i++)//从低位到高位
        input+=(((fill)>>i)&0x01) +'0';

    //IV承担Y[i-1]的功能
    bitset<64> plain, cipher;
    for(int i=0; i<input.length();i=i+64){
        plain = binary_to_bitset(input.substr(i,64));
        cipher = encrypt(plain ^ IV);
        IV = cipher;
        output+=bitset_to_binary(cipher);
    }
    ui->display->setText(QString::fromStdString(output));
    filewrite(output);
    QMessageBox message(QMessageBox::NoIcon, "Information", "Successful Encode!");
    message.setIconPixmap(QPixmap("encryption.PNG"));
    message.exec();
}

void MainWindow::push_decode(){
    QString qsinitkey = ui->inputkey->toPlainText();
    initkey = binary_to_bitset(qsinitkey.toStdString());
    getsubkey(initkey);
    bitset<64> plain, cipher;
    output="";
    IV = decode(binary_to_bitset(input.substr(0, 64)));
    for(int i=64;i<input.length();i=i+64){
        cipher = binary_to_bitset(input.substr(i,64));
        plain = decode(cipher)^IV;
        IV = cipher;
        output+=bitset_to_binary(plain);
    }
    int del=0;
    for(int i = output.length()-1; i>=output.length()-8; i--)
        del = (del<<1) + output[i]-'0';
    ui->display->setText(QString::fromStdString(output.substr(0, output.length()-del-8)));
    filewrite(output.substr(0, output.length()-del-8));
    QMessageBox message(QMessageBox::NoIcon, "Information", "Successful Decode!");
    message.setIconPixmap(QPixmap("decode.PNG"));
    message.exec();
}

