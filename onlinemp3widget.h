#ifndef ONLINEMP3WIDGET_H
#define ONLINEMP3WIDGET_H

#include <QWidget>
#include <QNetworkRequest>          //HTTP的URL管理类
#include <QNetworkAccessManager>    //URL的上传管理
#include <QNetworkReply>            //网页回复数据触发信号的类
#include <QEventLoop>               //提供一种进入和离开事件循环的方法
#include <QJsonArray>               //封装JSON数组
#include <QJsonObject>              //封装JSON对象
#include <QJsonDocument>

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
#include <QDateTime>
#include <QCryptographicHash>
#include <QMenu>
#include <QFileDialog>

//系统托盘
#include<QSystemTrayIcon>

static QString kugouSearchApi = "https://complexsearch.kugou.com/v2/search/song?";
static QString kugouDownldadApi = "https://wwwapi.kugou.com/play/songinfo?";


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
    QString musicJsonAnalysis(QByteArray JsonData);
    QString getDownload_Md5(QString time,QString encode_album_audio_id);
    QString getSearch_Md5(QString songname,QString time);

    //系统托盘实现
    QSystemTrayIcon *mysystemTray;
    //响应系统托盘
    void TrayIconActivate(QSystemTrayIcon::ActivationReason reason);
    void initTrayIcon();        //初始化
    void quitmucisPlayer();     //退出

signals:
    void finish(QByteArray Data);
    void lyricShow(QString url);

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

    void updateDuration(qint64 value);                //跟新播放的进度条
    void lyricTextShow(QString str);
    void netReply(QNetworkReply *reply);
    void downloadPlayer(QString encode_album_audio_id);
    void playSearchMusic();                     //双击搜索列表，播放音乐
    void playHistoryMusic();                    //播放历史音乐
    void backgroundtoDefaultSkin();             //默认皮肤
    void backgroundtoDingyiSkin();              //自定义皮肤



private:
    Ui::OnlineMp3Widget *ui;
    bool mousePress = false;
    QPoint movePoint;
    QSqlDatabase db;

    QMediaPlayer *player;
    QMediaPlaylist *playerlist;

    QNetworkRequest *request;
    QNetworkAccessManager *manager;
    // QJsonDocument *document;
    bool canchange;                             //用于拖动进度条
    int row =-1;

    //更换皮肤菜单
    QMenu *menuChange;
};
#endif // ONLINEMP3WIDGET_H
