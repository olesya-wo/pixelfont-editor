#include "orderdialog.h"

OrderDialog::OrderDialog(QWidget *parent)  : QDialog(parent)
{
    main_la = new QVBoxLayout;
    btn_la = new QHBoxLayout;
    hint = new QLabel("Upper pixel:\nMost significant bit?\nLeast significant bit?");
    msb_btn = new QPushButton("MSB");
    lsb_btn = new QPushButton("LSB");
    btn_la->addWidget(msb_btn);
    btn_la->addWidget(lsb_btn);
    main_la->addWidget(hint);
    main_la->addLayout(btn_la);
    setLayout(main_la);
    setWindowTitle("Options");
    connect(msb_btn,SIGNAL(clicked()),this,SLOT(on_msb()));
    connect(lsb_btn,SIGNAL(clicked()),this,SLOT(on_lsb()));
}

OrderDialog::~OrderDialog()
{

}

void OrderDialog::on_msb()
{
    msb=true;
    accept();
}
void OrderDialog::on_lsb()
{
    msb=false;
    accept();
}

