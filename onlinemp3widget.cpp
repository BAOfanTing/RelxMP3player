#include "onlinemp3widget.h"
#include "ui_onlinemp3widget.h"
#include <QPainter>

OnlineMp3Widget::OnlineMp3Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OnlineMp3Widget)
{
    ui->setupUi(this);
    //禁止窗口改变大小
    this->setFixedSize(this->geometry().size());
    //去掉标题
    this->setWindowFlag(Qt::FramelessWindowHint);

    // 1、连接数据库，如果不存在则创建
    if (QSqlDatabase::contains("song")) // 检查是否存在名为"song"的数据库连接
    {
        db = QSqlDatabase::database("song"); // 如果存在，则获取该数据库连接
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE"); // 如果不存在，则添加一个SQLite数据库连接
        db.setDatabaseName("song.db"); // 设置数据库文件名为"song.db"
    }

    // 2、打开数据库，读取数据表
    if (!db.open()) // 尝试打开数据库
    {
        // 打开数据库失败，显示错误信息
        QMessageBox::critical(nullptr, "错误", db.lastError().text());
    }
    else
    {
        // 3、定义查询对象，执行数据库操作
        QSqlQuery query; // 定义数据库查询对象
        QString qstl = "create table if not exists songlist(id integer , songname text , singername text , album_id text)"; // 创建歌曲列表表格的SQL语句
        int ret = query.exec(qstl); // 执行SQL语句
        if (!ret) // 检查SQL执行是否成功
        {
            // SQL执行失败，显示错误信息
            QMessageBox::critical(nullptr, "错误", db.lastError().text());
            qDebug() << db.lastError();
        }

        // 创建歌曲记录表格
        qstl = "create table if not exists songhistory(id integer primary key autoincrement, songname text , singername text)";
        ret = query.exec(qstl);
        if (!ret)
        {
            QMessageBox::critical(nullptr, "错误", db.lastError().text());
        }

        // 查询歌曲历史记录表中的数据并显示
        qstl = "select * from songhistory";
        if (!query.exec(qstl)) // 执行查询操作
        {
            // 查询失败，显示错误信息
            QMessageBox::critical(nullptr, "错误", db.lastError().text());
        }
        while (query.next()) // 遍历查询结果
        {
            QString songname, singername;
            QSqlRecord rec = query.record(); // 获取查询结果的记录
            int songnamekey = rec.indexOf("songname"); // 获取歌曲名字段在查询结果中的索引
            int singerkey = rec.indexOf("singername"); // 获取歌手名字段在查询结果中的索引
            songname = query.value(songnamekey).toString(); // 获取歌曲名
            singername = query.value(singerkey).toString(); // 获取歌手名

            QString strshow = songname + "--" + singername; // 构造要显示的字符串
            QListWidgetItem *item = new QListWidgetItem(strshow); // 创建列表项
            ui->lw_record->addItem(item); // 添加列表项到列表控件中
        }
    }

}

OnlineMp3Widget::~OnlineMp3Widget()
{
    delete ui;
}

void OnlineMp3Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //painter.drawPixmap(0,0,width(),height(),QPixmap(":/res/dingyilang.png"));
}

//移动事件
void OnlineMp3Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePress)  // 如果鼠标被按下
    {
        QPoint movepos = event->globalPos();  // 获取鼠标当前的全局位置
        move(movepos - movePoint);  // 移动窗口位置
    }
}

//鼠标释放事件
void OnlineMp3Widget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)  // 避免未使用的参数警告
    mousePress = false;  // 鼠标释放，标记鼠标状态为未按下
}

//鼠标点击事件
void OnlineMp3Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)  // 如果鼠标左键被按下
    {
        mousePress = true;  // 标记鼠标状态为按下
    }
    movePoint = event->globalPos() - pos();  // 计算鼠标相对于窗口左上角的偏移量
}

void OnlineMp3Widget::on_btn_close_clicked()
{
    this->close();
}


void OnlineMp3Widget::on_btn_min_clicked()
{

}


void OnlineMp3Widget::on_btn_change_clicked()
{

}


void OnlineMp3Widget::on_btn_search_clicked()
{

}


void OnlineMp3Widget::on_btn_lastsong_clicked()
{

}


void OnlineMp3Widget::on_btn_start_stop_clicked()
{

}


void OnlineMp3Widget::on_btn_nextsong_clicked()
{

}


void OnlineMp3Widget::on_btn_loop_clicked()
{

}


void OnlineMp3Widget::on_hs_sound_valueChanged(int value)
{

}


void OnlineMp3Widget::on_hs_songtime_sliderPressed()
{

}


void OnlineMp3Widget::on_hs_songtime_sliderReleased()
{

}


void OnlineMp3Widget::on_hs_songtime_valueChanged(int value)
{

}

