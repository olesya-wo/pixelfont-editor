#ifndef BUILDDIALOG_H
#define BUILDDIALOG_H

#include <QtWidgets>

class BuildDialog : public QDialog
{
    Q_OBJECT
public:
    BuildDialog(QWidget *parent = 0);
    QVBoxLayout *main_la;
    QPushButton *build_btn;
    QHBoxLayout *pos_la;
    QSpinBox *fnt_xoffs;
    QSpinBox *fnt_yoffs;
    QHBoxLayout *fnt_la;
    QSpinBox *fnt_size;
    QCheckBox *fnt_ital;
    QCheckBox *fnt_bold;
    QCheckBox *fnt_under;
    QCheckBox *fnt_str;
    ~BuildDialog();
public slots:
    void on_ok();
};

#endif // BUILDDIALOG_H
