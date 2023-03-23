#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jsonparser.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include "facade.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::outputFile(std::string str){
    int line = 1;
    std::string output_str = "1";
    for (unsigned i = 0; i < str.length(); i++)
    {
        output_str += str[i];
        if (str[i] == '\n'){
            output_str += std::to_string(++line);
        }
    }
    ui->textEdit->setText(QString::fromStdString(output_str));
}

void MainWindow::on_btn_clicked(){
    JsonParser jc;
    Facade facade;
    QString fileAdress = QFileDialog::getOpenFileName(this, "Open a file", "C://");
    std::string openResult = facade.ReadFile((fileAdress.toUtf8()).data());
    if (!openResult.empty()) {
        ui->textEdit->setText(QString::fromUtf8(openResult));
    }else {
        std::string str = facade.File().c_str();
        outputFile(str);
        try {
            QString res = QString::fromUtf8(facade.Check(jc).c_str());
            ui->lbl_res->setText(res);
        }  catch (const ParserError& e) {
            ui->lbl_res->setText(e.Description().c_str());
        }
    }
}
