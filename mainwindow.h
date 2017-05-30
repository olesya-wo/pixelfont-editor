#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <math.h>
#include <QMessageBox>
#include "builddialog.h"
#include "orderdialog.h"
#include "simplesettings.h"

#define CHAR_W  6
#define CHAR_H  8

/*  ===============================================================
 * | ___________________________________ <New><Open><Save><Export> |
 * || Draw area                         | <FontName> <CodePage>    |
 * ||                                   | ________________________ |
 * ||                                   ||ACSI                    ||
 * ||                                   ||                        ||
 * ||          16x16 cell               ||     16x16cell          ||
 * ||                                   ||                        ||
 * ||                                   ||________________________||
 * ||                                   | ________  ______________ |
 * ||                                   ||Preview ||              ||
 * ||___________________________________||________||______________||
 * |_______________________________________________________________|
*/

struct Locale
{
    QString name;
    QString data;
};

class LocTable
{
public:
    QList<Locale> list;
    bool get_by_data(Locale &s)
    {
        for (int i=0; i<list.count();++i)
        {
           if (list[i].data==s.data)
           {
               list[i].name+="/"+s.name;
               return true;
           }
        }
        return false;
    }
    void generate()
    {
        QList<QByteArray> codecs=QTextCodec::availableCodecs();
        QByteArray encodedString;
        for (int i=0; i<256;++i) encodedString.append((char)i);
        Locale c;
        foreach (QByteArray name, codecs)
        {
            QTextCodec *codec = QTextCodec::codecForName(name);
            c.name = QString(name);
            c.data = codec->toUnicode(encodedString);
            if (!get_by_data(c)) list.append(c);
        }
    }
    QString get_data_by_name(const QString &name)
    {
        foreach (Locale l, list)
        {
            if (l.name==name) return l.data;
        }
        return "";
    }
};

class Preview : public QWidget
{
    Q_OBJECT
public:
    Preview(QWidget *parent = 0);
    unsigned char w,h;
    unsigned char ch_w,ch_h;
    QString text;
    char zoom;
    char *font;
    QColor back;
    QColor front;
private:
    void paintEvent(QPaintEvent *e);
    void wheelEvent(QWheelEvent *e);
public slots:
    void need_repaint();
    void update_size();
};

class DrawArea : public QWidget
{
    Q_OBJECT
public:
    DrawArea(QWidget *parent = 0);
    unsigned char ch_w,ch_h;
    unsigned char current;
    unsigned char px_per_px;
    char font[CHAR_W*CHAR_H*256];
    char buf[CHAR_W*CHAR_H];
    char offs[(CHAR_W-1)*CHAR_H];
    char offs_h[CHAR_H];
    char offs_w[CHAR_W];
private:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
signals:
    void cell_changed(unsigned char ch);
    void pixel_changed();
public slots:
    void set_cell(unsigned char ch);
};

class CharTable : public QWidget
{
    Q_OBJECT
public:
    CharTable(QWidget *parent = 0);
    unsigned char ch_w,ch_h;
    unsigned char current;
    QString fnt;
    QString cp;
private:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
signals:
    void cell_changed(unsigned char ch);
public slots:
    void set_cell(unsigned char ch);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QWidget *main_w;
    QHBoxLayout *main_la;
    QVBoxLayout *right_la;
    QHBoxLayout *btn_la;
    QPushButton *btn_new;
    QPushButton *btn_open;
    QPushButton *btn_build;
    QPushButton *btn_save;
    QPushButton *btn_export;
    QHBoxLayout *fnt_la;
    QComboBox *fnt_name;
    QComboBox *cp_name;
    QScrollArea *scroll;
    DrawArea *draw;
    CharTable *table;
    QHBoxLayout *prev_la;
    Preview *prev;
    QPlainTextEdit *prev_inp;
    BuildDialog bdlg;
    LocTable lt;
public slots:
    void on_new();
    void on_open();
    void on_save();
    void on_export();
    void on_text();
    void on_build();
    void on_fnt(int);
};

#endif // MAINWINDOW_H
