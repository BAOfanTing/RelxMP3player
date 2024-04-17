#ifndef ONLINEMP3WIDGET_H
#define ONLINEMP3WIDGET_H

#include <QWidget>
#include <QNetworkRequest> //HTTP的URL管理类
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTime>
#include <math.h>


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

private:
    Ui::OnlineMp3Widget *ui;
};
#endif // ONLINEMP3WIDGET_H
