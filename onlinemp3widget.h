#ifndef ONLINEMP3WIDGET_H
#define ONLINEMP3WIDGET_H

#include <QWidget>
#include <QNetworkRequest>          //HTTP的URL管理类
#include <QNetworkAccessManager>    //URL的上传管理
#include <QNetworkReply>            //网页回复数据触发信号的类
#include <QEventLoop>               //提供一种进入和离开事件循环的方法
#include <QJsonArray>               //封装JSON数组
#include <QJsonObject>              //封装JSON对象

#include <QMediaPlayer>             //播放音乐相关
#include <QMediaPlaylist>

#include <QSqlDatabase>             //数据库相关
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QMessageBox>
#include <QTime>
#include <math.h>
#include <QMouseEvent>

static QString kugouSearchApi = "http://mobilecdn.kugou.com/api/v3/search/song?";
static QString kugouDownldadApi = "https://wwwapi.kugou.com/yy/index.php?";


QT_BEGIN_NAMESPACE
namespace Ui {
class OnlineMp3Widget;
}
QT_END_NAMESPACE

class OnlineMp3Widget : public QWidget
{
    Q_OBJECT

public:
    OnlineMp3Widget(QWidget *parent = nullptr);
    ~OnlineMp3Widget();

    void paintEvent(QPaintEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void hashJsonAnalysis(QByteArray JsonData);
    void httpAccess(QString url);

signals:
    void finish(QByteArray Data);

private slots:
    void on_btn_close_clicked();                //关闭窗口
    void on_btn_min_clicked();                  //最小化窗口
    void on_btn_change_clicked();               //更换皮肤
    void on_btn_search_clicked();               //搜索歌曲
    void on_btn_lastsong_clicked();             //上一首
    void on_btn_start_stop_clicked();           //播放
    void on_btn_nextsong_clicked();             //下一首
    void on_btn_loop_clicked();                 //循环播放
    void on_hs_sound_valueChanged(int value);   //音量调节

    void on_hs_songtime_sliderPressed();        //播放进度条调节
    void on_hs_songtime_sliderReleased();
    void on_hs_songtime_valueChanged(int value);

    void updateDuration(qint64);                //跟新播放的进度条
    void lyricTextShow(QString str);
    void netReplay(QNetworkReply *reply);


private:
    Ui::OnlineMp3Widget *ui;
    bool mousePress = false;
    QPoint movePoint;
    QSqlDatabase db;

    QMediaPlayer *player;
    QMediaPlaylist *playerlist;

    QNetworkRequest *request;
    QNetworkAccessManager *manager;
};
#endif // ONLINEMP3WIDGET_H
