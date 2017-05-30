#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    //
    main_w = new QWidget;
    main_la = new QHBoxLayout;
    right_la = new QVBoxLayout;
    btn_la = new QHBoxLayout;
    prev_la = new QHBoxLayout;
    btn_new = new QPushButton("New");
    btn_open = new QPushButton("Open");
    btn_build = new QPushButton("Build");
    btn_save = new QPushButton("Save");
    btn_export = new QPushButton("Export");
    fnt_la = new QHBoxLayout;
    fnt_name=new QComboBox; fnt_name->setEditable(false);
    QFontDatabase fontDatabase;
    fnt_name->insertItems(0,fontDatabase.families());
    cp_name = new QComboBox;
    cp_name->setEditable(false);
    // Генерация локалей
    lt.generate();
    foreach (Locale l, lt.list)
    {
       cp_name->addItem(l.name);
    }
    cp_name->setCurrentText("System");
    fnt_name->setCurrentText("Terminal");
    fnt_la->addWidget(fnt_name);
    fnt_la->addWidget(cp_name);
    prev = new Preview;
    prev_inp = new QPlainTextEdit; prev_inp->setLineWrapMode(QPlainTextEdit::NoWrap);
    draw = new DrawArea;
    table = new CharTable;
    scroll = new QScrollArea;
    scroll->setWidget(draw);

    btn_la->addWidget(btn_new);
    btn_la->addWidget(btn_open);
    btn_la->addWidget(btn_build);
    btn_la->addWidget(btn_save);
    btn_la->addWidget(btn_export);
    prev_la->addWidget(prev);
    prev_la->addWidget(prev_inp);
    right_la->addLayout(btn_la);
    right_la->addLayout(fnt_la);
    right_la->addWidget(table,0,Qt::AlignCenter);
    right_la->addLayout(prev_la);
    main_la->addWidget(scroll);
    main_la->addLayout(right_la);

    main_w->setLayout(main_la);
    setCentralWidget(main_w);
    prev->font=draw->font;
    connect(this->table, SIGNAL(cell_changed(unsigned char)),this->draw, SLOT(set_cell(unsigned char)));
    connect(this->draw, SIGNAL(cell_changed(unsigned char)),this->table, SLOT(set_cell(unsigned char)));
    connect(this->prev_inp,SIGNAL(textChanged()),this,SLOT(on_text()));
    connect(this->draw, SIGNAL(pixel_changed()),this->prev,SLOT(need_repaint()));
    connect(this->btn_new,SIGNAL(clicked()),this,SLOT(on_new()));
    connect(this->btn_open,SIGNAL(clicked()),this,SLOT(on_open()));
    connect(this->btn_build,SIGNAL(clicked()),this,SLOT(on_build()));
    connect(this->btn_save,SIGNAL(clicked()),this,SLOT(on_save()));
    connect(this->btn_export,SIGNAL(clicked()),this,SLOT(on_export()));
    connect(fnt_name,SIGNAL(currentIndexChanged(int)),this,SLOT(on_fnt(int)));
    connect(cp_name,SIGNAL(currentIndexChanged(int)),this,SLOT(on_fnt(int)));
    // Загрузка стандартного шрифта
    QString file="Default.pfnt";
    QFile inp(file);
    if (!inp.open(QIODevice::ReadOnly)) return;
    inp.read(draw->font,draw->ch_w*draw->ch_h*256);
    inp.close();
    // Загрузка настроек предпросмотра
    SimpleSettings cfg;
    if (cfg.load_from_file("settings.cfg"))
    {
        prev->w=cfg.get_int("preview","w",14);
        prev->h=cfg.get_int("preview","h",6);
        prev->update_size();
        QList<int> b=cfg.get_int_arr("preview","background");
        if (b.count()==3)
            prev->back=QColor(b[0],b[1],b[2]);
        else
            QMessageBox::warning(this,"Cfg","'background' value not valid");
        QList<int> f=cfg.get_int_arr("preview","foreground");
        if (f.count()==3)
            prev->front=QColor(f[0],f[1],f[2]);
        else
            QMessageBox::warning(this,"Cfg","'foreground' value not valid");
        QList<QString> t=cfg.get_string_arr("preview","text");
        for (int i=0;i<t.count();++i) prev_inp->appendPlainText(t[i]);
        on_text();
    }else{
        QMessageBox::warning(this,"Cfg","Configuration file not found. Using default parameters.");
    }
    // Обновление
    prev_inp->document()->setDefaultFont(QFont(fnt_name->currentText()));
    table->cp=lt.get_data_by_name(cp_name->currentText());
    prev->need_repaint();
    draw->update();
    draw->repaint();
}
MainWindow::~MainWindow()
{

}
void MainWindow::on_new()
{
    memset(draw->font,0,draw->ch_w*draw->ch_h*256);
    draw->update();
    draw->repaint();
    prev->need_repaint();
}
void MainWindow::on_open()
{
    QString file=QFileDialog::getOpenFileName(this,"Select file",QDir::currentPath(), "Fonts (*.pfnt);;Images (*.png *.bmp)");
    if (file.isEmpty()) return;
    if (file.right(4)=="pfnt")
    {
        QFile inp(file);
        if (!inp.open(QIODevice::ReadOnly))
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Can't open file!");
            msg.exec();
            return;
        }
        inp.read(draw->font,draw->ch_w*draw->ch_h*256);
        inp.close();
    }
    if (file.right(4)==".bmp" || file.right(4)==".png")
    {
        QImage inp;
        if (!inp.load(file))
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Can't open file!");
            msg.exec();
            return;
        }
        if (inp.width()!=draw->ch_w*16 || inp.height()!=draw->ch_h*16)
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Image sizes do not match font sizes!");
            msg.exec();
            return;
        }
        QColor px;
        for (int r=0; r<16;++r)
        for (int c=0; c<16;++c)
        {
            for (int pc=0; pc<draw->ch_w;++pc)
            for (int pr=0; pr<draw->ch_h;++pr)
            {
                px=QColor::fromRgb(inp.pixel(c*draw->ch_w+pc,r*draw->ch_h+pr));
                draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+pr]=(px.red()+px.green()+px.blue())<384 ? 0 : 1;
            }
        }
    }
    prev->need_repaint();
    draw->update();
    draw->repaint();
}
void MainWindow::on_build()
{
    int res=bdlg.exec();
    if (res!=QDialog::Accepted) return;
    // Отрисовка шрифта во временное изображение
    QString family=fnt_name->currentText();
    if (family.length()==0){return;}
    QImage alpha;
    int weight = QFont::Normal;
    if (bdlg.fnt_bold->isChecked()){weight=QFont::Bold;}
    QFont fnt(family,bdlg.fnt_size->value(),weight,bdlg.fnt_ital->isChecked());
    fnt.setStrikeOut(bdlg.fnt_str->isChecked());
    fnt.setUnderline(bdlg.fnt_under->isChecked());
    int x,y,w,h,xoffs=bdlg.fnt_xoffs->value(),yoffs=bdlg.fnt_yoffs->value();
    alpha=QImage(draw->ch_w*16,draw->ch_h*16,QImage::Format_RGB32);
    QRgb black=QColor(Qt::black).rgb();
    alpha.fill(black);
    QPainter p;
    p.begin(&alpha);
    p.setPen(QPen(Qt::white));
    p.setFont(fnt);
    w=alpha.width()/16;
    h=alpha.height()/16;
    QString cp=lt.get_data_by_name(cp_name->currentText());
    for (unsigned short i=0; i<256; ++i)
    {
        x=(int)fmodf((float)i,16.0f);
        x*=w;
        y=i/16;
        y*=h;
        p.drawText(x+xoffs,y+yoffs,w,h,Qt::AlignLeft,cp.mid(i,1));
    }
    p.end();
    // Преобразование из полученного изображения
    QColor px;
    for (int r=0; r<16;++r)
    for (int c=0; c<16;++c)
    {
        for (int pc=0; pc<draw->ch_w;++pc)
        for (int pr=0; pr<draw->ch_h;++pr)
        {
            px=QColor::fromRgb(alpha.pixel(c*draw->ch_w+pc,r*draw->ch_h+pr));
            draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+pr]=(px.red()+px.green()+px.blue())<384 ? 0 : 1;
        }
    }
    prev->need_repaint();
    draw->update();
    draw->repaint();
}
void MainWindow::on_fnt(int)
{
    table->fnt=fnt_name->currentText();
    table->cp=lt.get_data_by_name(cp_name->currentText());
    table->update();
    table->repaint();
    prev_inp->document()->setDefaultFont(QFont(fnt_name->currentText()));
}
void MainWindow::on_save()
{
    QString filter;
    QString file=QFileDialog::getSaveFileName(this,"Select file",QDir::currentPath(), "Fonts (*.pfnt);;Images (*.png *.bmp)",&filter);
    if (file.isEmpty()) return;
    if (file.right(4)=="pfnt" || filter=="Fonts (*.pfnt)")
    {
        if (file.right(5)!=".pfnt") file.append(".pfnt");
        QFile out(file);
        if (!out.open(QIODevice::WriteOnly))
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Can't save file!");
            msg.exec();
            return;
        }
        out.write(draw->font,draw->ch_w*draw->ch_h*256);
        out.close();
    }
    if (file.right(4)==".bmp" || file.right(4)==".png"  || filter=="Images (*.png *.bmp)")
    {
        if (file.right(4)!=".png" && file.right(4)!=".bmp")
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Incorrect image format!");
            msg.exec();
            return;
        }
        QImage out=QImage(draw->ch_w*16,draw->ch_h*16,QImage::Format_RGB32);
        QRgb black=QColor(Qt::black).rgb();
        QRgb white=QColor(Qt::white).rgb();
        QRgb px;
        for (int r=0; r<16;++r)
        for (int c=0; c<16;++c)
        {
            for (int pc=0; pc<draw->ch_w;++pc)
            for (int pr=0; pr<draw->ch_h;++pr)
            {
                if (draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+pr]==0) px=white; else px=black;
                out.setPixel(c*draw->ch_w+pc,r*draw->ch_h+pr,px);
            }
        }
        bool res=false;
        if (file.right(4)==".bmp") res=out.save(file,"BMP");
        if (file.right(4)==".png") res=out.save(file,"PNG");
        if (!res)
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText("Can't save file!");
            msg.exec();
        }
    }
}
void MainWindow::on_export()
{
    QString file=QFileDialog::getSaveFileName(this,"Select file",QDir::currentPath(), "Header (*.h);;All files (*.*)");
    if (file.isEmpty()) return;
    int width=6;
    bool ok;
    width=QInputDialog::getInt(this,"Options","Enter char width:",draw->ch_w,2,draw->ch_w,1,&ok);
    if (!ok) return;
    OrderDialog odlg;
    int res=odlg.exec();
    if (res!=QDialog::Accepted) return;
    QFile out(file);
    if (!out.open(QIODevice::WriteOnly))
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText("Can't save file!");
        msg.exec();
        return;
    }
    QTextStream stream(&out);
    //stream.setCodec(cp_name->currentText().toLatin1().constData());
    stream << "// CodePage: " << cp_name->currentText() << "\n";
    stream << "const unsigned char FontTable[256][" << QString::number(width)<<"] = {\n";
    unsigned char byte;
    unsigned char b0,b1,b2,b3,b4,b5,b6,b7;
    QString b;
    char ch=0;
    QString cp=lt.get_data_by_name(cp_name->currentText());
    for (int r=0; r<16;++r)
    for (int c=0; c<16;++c)
    {
        byte=0;
        stream << "\t{ ";
        for (int pc=0; pc<width;++pc)
        {
            b0=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+0];
            b1=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+1];
            b2=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+2];
            b3=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+3];
            b4=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+4];
            b5=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+5];
            b6=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+6];
            b7=draw->font[(r*16+c)*draw->ch_w*draw->ch_h+pc*draw->ch_h+7];
            if (odlg.msb)
            {
                // Самый значимый бит - верхний пиксель
                byte=(b7<<0)|(b6<<1)|(b5<<2)|(b4<<3)|(b3<<4)|(b2<<5)|(b1<<6)|(b0<<7);
            }else
            {
                // Самый значимый бит - нижний пиксель
                byte=(b0<<0)|(b1<<1)|(b2<<2)|(b3<<3)|(b4<<4)|(b5<<5)|(b6<<6)|(b7<<7);
            }
            b=QString::number(byte,16);
            if (b.length()<2) b="0"+b;
            stream << "0x" << b;
            if (pc<width-1) stream << ",";
            stream << " ";
        }
        stream << "}";
        if ((r*16+c)<256-1) stream << ",";
        stream << "\t// ";
        ch=r*16+c;
        if (ch==0 || ch==9 || ch==10 || ch==13) stream << " ";  else  stream << cp.mid(ch,1);
        stream << " " << QString::number(r*16+c) << "\n";
    }
    stream << "};\n";
    out.close();
}
void MainWindow::on_text()
{
    prev->text=QString::fromLatin1(prev_inp->toPlainText().toLocal8Bit());
    prev->need_repaint();
}

Preview::Preview(QWidget *parent) : QWidget(parent)
{
    back=QColor(Qt::white);
    front=QColor(85,150,0);
    zoom=1;
    w=14;
    h=6;
    ch_w=CHAR_W;
    ch_h=CHAR_H;
    setFocusPolicy(Qt::StrongFocus);
    update_size();
}
void Preview::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    int lx=(this->width()-ch_w*w*zoom)/2;
    int ly=(this->height()-ch_h*h*zoom)/2;
    QPainter p(this);
    // Фон
    p.setBrush(QBrush(back,Qt::SolidPattern));
    p.drawRect(0,0,this->width()-1,this->height()-1);
    // Текст
    p.setPen(front);
    char xpos=0;
    char ypos=0;
    int ch=0;
    for (int i=0; i<text.length();++i)
    {
        if (text[i]=='\n') {++ypos; xpos=0;}
        if (xpos>w-1) continue;
        if (ypos>h-1) break;
        ch=((unsigned char)text[i].unicode())*ch_w*ch_h;
        for (char c=0; c<ch_w;++c)
            for (char r=0; r<ch_h;++r)
            {
                if (font[ch+c*ch_h+r]!=0)
                {
                    p.drawPoint(lx+xpos*ch_w*zoom+c*zoom,ly+ypos*ch_h*zoom+r*zoom);
                    if (zoom==2)
                    {
                        p.drawPoint(lx+xpos*ch_w*zoom+c*zoom+1,ly+ypos*ch_h*zoom+r*zoom);
                        p.drawPoint(lx+xpos*ch_w*zoom+c*zoom+1,ly+ypos*ch_h*zoom+r*zoom+1);
                        p.drawPoint(lx+xpos*ch_w*zoom+c*zoom,ly+ypos*ch_h*zoom+r*zoom+1);
                    }
                }
            }
        if (text[i]!='\n') ++xpos;
    }
    // Рамка
    p.setPen(Qt::gray);
    p.drawLine(lx-1,ly-1,lx+ch_w*w*zoom,ly-1);
    p.drawLine(lx-1,ly+ch_h*h*zoom,lx+ch_w*w*zoom,ly+ch_h*h*zoom);
    p.drawLine(lx-1,ly-1,lx-1,ly+ch_h*h*zoom);
    p.drawLine(lx+ch_w*w*zoom,ly-1,lx+ch_w*w*zoom,ly+ch_h*h*zoom);
}
void Preview::need_repaint()
{
    this->update();
    this->repaint();
}
void Preview::update_size()
{
    setMinimumSize(ch_w*w*2+2,ch_h*h*2+2);
}
void Preview::wheelEvent(QWheelEvent *e)
{
    zoom=e->angleDelta().y()<0 ? 1 : 2;
    e->accept();
    need_repaint();
}

DrawArea::DrawArea(QWidget *parent) : QWidget(parent)
{
    current=0;
    ch_w=CHAR_W;
    ch_h=CHAR_H;
    px_per_px=4;
    setMaximumSize((px_per_px+1)*ch_w*16+1,(px_per_px+1)*ch_h*16+1);
    setMinimumSize((px_per_px+1)*ch_w*16+1,(px_per_px+1)*ch_h*16+1);
    memset(font,0,ch_w*ch_h*256);
    memset(buf,0,ch_w*ch_h);
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
}
void DrawArea::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    // Пиксели
    int char_pos_x;
    int char_pos_y;
    int pos;
    for (int r=0; r<16;++r) // Цикл по символам попорядку
        for (int c=0;c<16;++c)
        {
            char_pos_x=(px_per_px+1)*ch_w*c;
            char_pos_y=(px_per_px+1)*ch_h*r;
            for (int pc=0; pc<ch_w;++pc) // Цикл по столбцам символа
                for (int pr=0;pr<ch_h;++pr)
                {
                    pos=(r*16+c)*(ch_w*ch_h)+pc*ch_h+pr;
                    if (font[pos]==0)
                    {
                        p.setPen(Qt::white);
                        p.setBrush(QBrush(Qt::white,Qt::SolidPattern));
                    }else
                    {
                        p.setPen(Qt::black);
                        p.setBrush(QBrush(Qt::black,Qt::SolidPattern));
                    }
                    p.drawRect(char_pos_x+pc*(px_per_px+1),char_pos_y+pr*(px_per_px+1),px_per_px,px_per_px);
                }
        }
    // Сетка линий между пикселями
    p.setPen(Qt::green);
    for (int r=1; r<16*ch_h;++r) // Горизонтальные
    {
        p.drawLine(0,(px_per_px+1)*r,(px_per_px+1)*ch_w*16,(px_per_px+1)*r);
    }
    for (int c=1;c<16*ch_w;++c) // Вертикальные
    {
        p.drawLine((px_per_px+1)*c,0,(px_per_px+1)*c,(px_per_px+1)*ch_h*16);
    }
    QPen pen(Qt::gray);
    p.setPen(pen);
    // Верхняя рамка
    p.drawLine(0,0,(px_per_px+1)*ch_w*16,0);
    // Нижняя рамка
    p.drawLine(0,(px_per_px+1)*ch_h*16,(px_per_px+1)*ch_w*16,(px_per_px+1)*ch_h*16);
    // Левая рамка
    p.drawLine(0,0,0,(px_per_px+1)*ch_h*16);
    // Правая рамка
    p.drawLine((px_per_px+1)*ch_w*16,0,(px_per_px+1)*ch_w*16,(px_per_px+1)*ch_h*16);
    // Сетка линий между символами
    pen.setWidth(2);
    p.setPen(pen);
    for (int r=1; r<16;++r) // Горизонтальные
    {
        p.drawLine(0,(px_per_px+1)*ch_h*r,(px_per_px+1)*ch_w*16,(px_per_px+1)*ch_h*r);
    }
    for (int c=1;c<16;++c) // Вертикальные
    {
        p.drawLine((px_per_px+1)*ch_w*c,0,(px_per_px+1)*ch_w*c,(px_per_px+1)*ch_h*16);
    }
    // Выделенный символ
    pen.setColor(Qt::red);
    p.setPen(pen);
    char x,y;
    y=current/16;
    x=(char)fmodf((float)current,16.0f);
    int pos_x=(px_per_px+1)*ch_w*x;
    int pos_y=(px_per_px+1)*ch_h*y;
    int lw=(px_per_px+1)*ch_w;
    int lh=(px_per_px+1)*ch_h;
    // Горизонтальные
    p.drawLine(pos_x,pos_y,pos_x+lw,pos_y);
    p.drawLine(pos_x,pos_y+lh,pos_x+lw,pos_y+lh);
    // Вертикальные
    p.drawLine(pos_x,pos_y,pos_x,pos_y+lh);
    p.drawLine(pos_x+lw,pos_y,pos_x+lw,pos_y+lh);
}
void DrawArea::mousePressEvent(QMouseEvent *e)
{
    int col=e->pos().x()/((px_per_px+1)*ch_w);
    int row=e->pos().y()/((px_per_px+1)*ch_h);
    // Правый клик или левый с контролом
    if ((e->modifiers()==Qt::ControlModifier && e->button()==Qt::LeftButton) || (e->button()==Qt::RightButton))
    {
        current=row*16+col;
        update();
        repaint();
        cell_changed(current);
        e->accept();
        return;
    }
    // Левый клик без контролов
    if (e->modifiers()==Qt::NoModifier && e->button()==Qt::LeftButton)
    {
        if (row*16+col!=current) return;
        char pr=(e->pos().y()-row*(px_per_px+1)*ch_h)/(px_per_px+1);
        char pc=(e->pos().x()-col*(px_per_px+1)*ch_w)/(px_per_px+1);
        int pos=(row*16+col)*(ch_w*ch_h)+pc*ch_h+pr;
        font[pos] = font[pos]==0 ? 1 : 0;
        update();
        repaint();
        pixel_changed();
    }
}
void DrawArea::mouseMoveEvent(QMouseEvent *e)
{
    int col=e->pos().x()/((px_per_px+1)*ch_w);
    int row=e->pos().y()/((px_per_px+1)*ch_h);
    if (row*16+col!=current) return;
    // Зажатая левая кнопка с контролом - рисование
    if (e->modifiers()==Qt::ControlModifier && e->buttons()==Qt::LeftButton)
    {
        char pr=(e->pos().y()-row*(px_per_px+1)*ch_h)/(px_per_px+1);
        char pc=(e->pos().x()-col*(px_per_px+1)*ch_w)/(px_per_px+1);
        int pos=(row*16+col)*(ch_w*ch_h)+pc*ch_h+pr;
        font[pos] = 1;
        update();
        repaint();
        pixel_changed();
    }
    // Зажатая левая кнопка с шифтом - стирание
    if (e->modifiers()==Qt::ShiftModifier && e->buttons()==Qt::LeftButton)
    {
        char pr=(e->pos().y()-row*(px_per_px+1)*ch_h)/(px_per_px+1);
        char pc=(e->pos().x()-col*(px_per_px+1)*ch_w)/(px_per_px+1);
        int pos=(row*16+col)*(ch_w*ch_h)+pc*ch_h+pr;
        font[pos] = 0;
        update();
        repaint();
        pixel_changed();
    }
    e->accept();
}
void DrawArea::wheelEvent(QWheelEvent *e)
{
    if (e->angleDelta().y()<0)
    {
        if (px_per_px>1) --px_per_px;
    }else
    {
        if (px_per_px<32) ++px_per_px;
    }
    setMaximumSize((px_per_px+1)*ch_w*16+1,(px_per_px+1)*ch_h*16+1);
    setMinimumSize((px_per_px+1)*ch_w*16+1,(px_per_px+1)*ch_h*16+1);
    update();
    repaint();
    e->accept();
}
void DrawArea::set_cell(unsigned char ch)
{
    current=ch;
    update();
    repaint();
}
void DrawArea::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers()!=Qt::ControlModifier) return;
    // Инвертирование
    if (e->key()==Qt::Key_I)
    {
        for (int i=0; i<ch_w*ch_h*256;++i) font[i]=font[i]==0 ? 1 : 0;
    }
    // Копирование
    if (e->key()==Qt::Key_C)
    {
        memcpy(buf,&font[current*ch_w*ch_h],ch_w*ch_h);
    }
    // Вставка
    if (e->key()==Qt::Key_V)
    {
        memcpy(&font[current*ch_w*ch_h],buf,ch_w*ch_h);
    }
    // Смещение вверх
    if (e->key()==Qt::Key_W)
    {
        int pos=current*ch_w*ch_h;
        // Бэкап первой строки
        for (int c=0; c<ch_w;++c) offs_w[c]=font[pos+c*ch_h];
        // Смещение вверх
        for (int c=0; c<ch_w;++c)
        for (int r=0; r<ch_h-1;++r)
        {
            font[pos+c*ch_h+r]=font[pos+c*ch_h+r+1];
        }
        // Копирование строки из бэкапа
        for (int c=0; c<ch_w;++c) font[pos+c*ch_h+ch_h-1]=offs_w[c];
    }
    // Смещение вниз
    if (e->key()==Qt::Key_S)
    {
        int pos=current*ch_w*ch_h;
        // Бэкап последней строки
        for (int c=0; c<ch_w;++c) offs_w[c]=font[pos+c*ch_h+ch_h-1];
        // Смещение вниз
        for (int c=0; c<ch_w;++c)
        for (int r=ch_h-1; r>0;--r)
        {
            font[pos+c*ch_h+r]=font[pos+c*ch_h+r-1];
        }
        // Копирование строки из бэкапа
        for (int c=0; c<ch_w;++c) font[pos+c*ch_h]=offs_w[c];
    }
    // Смещение влево
    if (e->key()==Qt::Key_A)
    {
        int pos=current*ch_w*ch_h;
        // Бэкап символа без левого столбца
        memcpy(offs,&font[pos+ch_h],(ch_w-1)*ch_h);
        // Бэкап левого столбца
        memcpy(offs_h,&font[pos],ch_h);
        // Вставка символа
        memcpy(&font[pos],offs,(ch_w-1)*ch_h);
        // Вставка левого столбца вправо
        memcpy(&font[pos+(ch_h*(ch_w-1))],offs_h,ch_h);
    }
    // Смещение вправо
    if (e->key()==Qt::Key_D)
    {
        int pos=current*ch_w*ch_h;
        // Бэкап символа без правого столбца
        memcpy(offs,&font[pos],(ch_w-1)*ch_h);
        // Бэкап правого столбца
        memcpy(offs_h,&font[pos+(ch_h*(ch_w-1))],ch_h);
        // Вставка символа
        memcpy(&font[pos+ch_h],offs,(ch_w-1)*ch_h);
        // Вставка правого столбца слево
        memcpy(&font[pos],offs_h,ch_h);
    }
    // Отразить по вертикали
    if (e->key()==Qt::Key_G)
    {
        int pos=current*ch_w*ch_h;
        for (int r=0; r<(ch_h/2);++r)
        {
            // Бэкап верхней строчки
            for (int c=0; c<ch_w;++c) offs_w[c]=font[pos+c*ch_h+r];
            // Копирование нижней строчки на место верхней
            for (int c=0; c<ch_w;++c) font[pos+c*ch_h+r]=font[pos+c*ch_h+(ch_h-r-1)];
            // Копирование в нижнюю строчку из бэкапа
            for (int c=0; c<ch_w;++c) font[pos+c*ch_h+(ch_h-r-1)]=offs_w[c];
        }
    }
    // Отразить по горизонтали
    if (e->key()==Qt::Key_H)
    {
        int pos=current*ch_w*ch_h;
        for (int c=0; c<(ch_w/2);++c)
        {
            // Бэкап левого столбца
            memcpy(offs_h,&font[pos+ch_h*c],ch_h);
            // Копирование правого столбца на место левого
            memcpy(&font[pos+ch_h*c],&font[pos+ch_h*(ch_w-c-1)],ch_h);
            // Копирование в правый стобец из бэкапа
            memcpy(&font[pos+ch_h*(ch_w-c-1)],offs_h,ch_h);
        }
    }
    update();
    repaint();
    pixel_changed();
}

CharTable::CharTable(QWidget *parent) : QWidget(parent)
{
    current=0;
    ch_w=32;
    ch_h=32;
    fnt="Arial";
    setMaximumSize(ch_w*16+1,ch_h*16+1);
    setMinimumSize(ch_w*16+1,ch_h*16+1);
}
void CharTable::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setPen(Qt::black);
    // Символы
    p.setFont(QFont(fnt,10,1,false));
    for (int r=0; r<16;++r)
        for (int c=0;c<16;++c)
        {
            p.drawText(c*ch_w+5,r*ch_h+15,cp.mid(r*16+c,1));
        }
    // коды символов
    p.setFont(QFont("Arial",8,1,false));
    for (int r=0; r<16;++r)
        for (int c=0;c<16;++c)
        {
            p.drawText(c*ch_w+12,r*ch_h+ch_h-1,QString::number(r*16+c));
        }
    // Сетка линий между символами
    for (int r=1; r<16;++r) // Горизонтальные
    {
        p.drawLine(0,ch_h*r,ch_w*16,ch_h*r);
    }
    for (int c=1;c<16;++c) // Вертикальные
    {
        p.drawLine(ch_w*c,0,ch_w*c,ch_h*16);
    }
    // Верхняя рамка
    p.drawLine(0,0,ch_w*16,0);
    // Нижняя рамка
    p.drawLine(0,ch_h*16,ch_w*16,ch_h*16);
    // Левая рамка
    p.drawLine(0,0,0,ch_h*16);
    // Правая рамка
    p.drawLine(ch_w*16,0,ch_w*16,ch_h*16);
    // Выделенный символ
    p.setPen(Qt::red);
    char x,y;
    y=current/16;
    x=(char)fmodf((float)current,16.0f);
    int pos_x=ch_w*x;
    int pos_y=ch_h*y;
    int lw=ch_w;
    int lh=ch_h;
    // Горизонтальные
    p.drawLine(pos_x,pos_y,pos_x+lw,pos_y);
    p.drawLine(pos_x,pos_y+lh,pos_x+lw,pos_y+lh);
    // Вертикальные
    p.drawLine(pos_x,pos_y,pos_x,pos_y+lh);
    p.drawLine(pos_x+lw,pos_y,pos_x+lw,pos_y+lh);
}
void CharTable::mousePressEvent(QMouseEvent *e)
{
    int col=e->pos().x()/ch_w;
    int row=e->pos().y()/ch_h;
    current=row*16+col;
    update();
    repaint();
    cell_changed(current);
    e->accept();
}
void CharTable::set_cell(unsigned char ch)
{
    current=ch;
    update();
    repaint();
}
