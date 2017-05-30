#include "builddialog.h"

BuildDialog::BuildDialog(QWidget *parent)  : QDialog(parent)
{
    main_la = new QVBoxLayout;
    fnt_la = new QHBoxLayout;
    pos_la = new QHBoxLayout;
    build_btn = new QPushButton("Build");
    fnt_size=new QSpinBox;
    fnt_size->setRange(2,16);
    fnt_size->setValue(5);
    fnt_ital=new QCheckBox("Italic");
    fnt_bold=new QCheckBox("Bold");
    fnt_under=new QCheckBox("Underline");
    fnt_str=new QCheckBox("Strikeout");
    fnt_la->addWidget(new QLabel("Size -"));
    fnt_la->addWidget(fnt_size);
    fnt_la->addWidget(fnt_ital);
    fnt_la->addWidget(fnt_bold);
    fnt_la->addWidget(fnt_under);
    fnt_la->addWidget(fnt_str);

    fnt_xoffs=new QSpinBox;
    fnt_xoffs->setRange(-16,16);
    fnt_yoffs=new QSpinBox;
    fnt_yoffs->setRange(-16,16);
    pos_la->addWidget(new QLabel("X offset -"),0,Qt::AlignRight);
    pos_la->addWidget(fnt_xoffs);
    pos_la->addWidget(new QLabel("Y offset -"),0,Qt::AlignRight);
    pos_la->addWidget(fnt_yoffs);
    main_la->addLayout(fnt_la);
    main_la->addLayout(pos_la);
    main_la->addWidget(build_btn);
    setLayout(main_la);
    connect(build_btn,SIGNAL(clicked()),this,SLOT(on_ok()));
    setWindowTitle("Font options");
}
BuildDialog::~BuildDialog(){}

void BuildDialog::on_ok()
{
    this->accept();
}

