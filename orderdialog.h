#ifndef ORDERDIALOG_H
#define ORDERDIALOG_H

#include <QtWidgets>

class OrderDialog : public QDialog
{
    Q_OBJECT
public:
    OrderDialog(QWidget *parent = 0);
    QVBoxLayout *main_la;
    QLabel *hint;
    QHBoxLayout *btn_la;
    QPushButton *msb_btn;
    QPushButton *lsb_btn;
    ~OrderDialog();
    bool msb;
public slots:
    void on_msb();
    void on_lsb();
};

#endif // ORDERDIALOG_H
