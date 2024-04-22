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
        QString qstl = "create table if not exists songlist(id integer , FileName text , EMixSongID text);"; // 创建歌曲列表表格的SQL语句
        int ret = query.exec(qstl); // 执行SQL语句
        if (!ret) // 检查SQL执行是否成功
        {
            // SQL执行失败，显示错误信息
            QMessageBox::critical(nullptr, "create table songlist", db.lastError().text());
            qDebug() << db.lastError();
        }

        // 创建歌曲记录表格
        qstl = "create table if not exists songhistory(id integer primary key autoincrement, FileName text , EMixSongID text);";
        ret = query.exec(qstl);
        if (!ret)
        {
            QMessageBox::critical(nullptr, "create table songhistory", db.lastError().text());
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
            QSqlRecord rec = query.record(); // 获取查询结果的记录
            int singer_song_namekey = rec.indexOf("FileName"); // 获取歌曲名字段在查询结果中的索引
            QString strshow = rec.value(singer_song_namekey).toString(); // 构造要显示的字符串
            QListWidgetItem *item = new QListWidgetItem(strshow); // 创建列表项
            ui->lw_record->addItem(item); // 添加列表项到列表控件中
        }
    }

    //播放操作
    player = new QMediaPlayer;
    playerlist = new QMediaPlaylist;

    //绑定双击搜索列表播放音乐槽函数
    connect(ui->lw_search,&QListWidget::itemClicked,this,&OnlineMp3Widget::playSearchMusic);

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

//音乐的hash和ablum_id值解析，使用Json
void OnlineMp3Widget::hashJsonAnalysis(QByteArray JsonData)
{
    //qDebug()<< JsonData; // 打印输入的 JSON 数据，用于调试
    //移除callback123()
    // 找到第一个左括号 "(" 的位置
    int leftBracketIndex = JsonData.indexOf('(');
    if (leftBracketIndex != -1)
    {
        // 找到最后一个右括号 ")" 的位置
        int rightBracketIndex = JsonData.lastIndexOf(')');
        if (rightBracketIndex != -1)
        {
            // 提取 JSON 数据，去除包裹的部分
            JsonData = JsonData.mid(leftBracketIndex + 1, rightBracketIndex - leftBracketIndex - 1);
        }
    }
    //保存json查看数据
    QFile file("hash.json");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(JsonData);
        file.close();
    }


    // 将 JSON 数据解析为 QJsonDocument 对象
    QJsonDocument document = QJsonDocument::fromJson(JsonData);

    if(document.isObject()) // 如果解析后的对象是一个 JSON 对象
    {
        qDebug()<<"boject";
        QJsonObject data = document.object(); // 获取 JSON 对象中的"data"字段
        if(data.contains("data")) // 如果"data"字段存在
        {
            QJsonObject objectInfo = data.value("data").toObject(); // 获取"data"字段中的对象
            qDebug()<<"data";
            if(objectInfo.contains("lists")) // 如果"lists"字段存在
            {
                QJsonArray objectHash = objectInfo.value("lists").toArray(); // 获取"lists"字段中的数组
                qDebug()<<"lists";
                for(int i = 0; i < objectHash.count(); i++) // 遍历数组中的每个元素
                {
                    QString singer_song_name,EMixSongID;
                    QJsonObject album = objectHash.at(i).toObject(); // 获取数组元素中的对象

                    // 从对象中获取歌曲名、歌手名、专辑 ID 和哈希值
                    if(album.contains("FileName"))
                    {
                        singer_song_name = album.value("FileName").toString();
                    }
                    if(album.contains("EMixSongID"))
                    {
                        EMixSongID = album.value("EMixSongID").toString();
                    }
                    // 将解析出的信息插入数据库
                    QSqlQuery query;
                    QString sql = QString("insert into songlist values(%1,'%2','%3');").arg(QString::number(i)).arg(singer_song_name).arg(EMixSongID);
                    if(!query.exec(sql)) // 如果插入数据库失败
                    {
                        QMessageBox::critical(nullptr, "插入数据库错误", db.lastError().text());
                    }

                    // 在搜索展示框中显示歌曲名称和歌手名称
                    QListWidgetItem *item = new QListWidgetItem(singer_song_name);
                    ui->lw_search->addItem(item);
                }
            }
        }
    }
    // if(document.isArray())
    // {
    //     qDebug() <<"Array";
    // }
}

//访问HTTP网页
void OnlineMp3Widget::httpAccess(QString url)
{
    //实例化网络请求操作事项
    request = new QNetworkRequest;

    //将url网页地址存入request请求中
    request->setUrl(url);

    //实例化网络管理（访问）
    manager = new QNetworkAccessManager;

    //通过get,上传具体的请求
    manager->get(*request);

    //当网页回复消息，出发finish信号，读取数据
    connect(manager,&QNetworkAccessManager::finished,this,&OnlineMp3Widget::netReply);
}

// 解析 JSON 数据，获取音乐播放 URL
QString OnlineMp3Widget::musicJsonAnalysis(QByteArray JsonData)
{
    // 保存 JSON 数据到文件中以便查看
    QFile file("download.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(JsonData);
        file.close();
    }

    // 解析 JSON 数据
    QJsonDocument document = QJsonDocument::fromJson(JsonData);
    if (document.isObject())
    {
        QJsonObject data = document.object();
        if (data.contains("data"))
        {
            QJsonObject objectPlayurl = data.value("data").toObject();
            // 如果包含歌词，发送歌词显示信号
            if (objectPlayurl.contains("lyrics"))
            {
                emit lyricShow(objectPlayurl.value("lyrics").toString());
            }
            // 返回音乐播放 URL
            if (objectPlayurl.contains("play_url"))
            {
                return objectPlayurl.value("play_url").toString();
                qDebug()<<objectPlayurl.value("play_url").toString();
            }
        }
    }
}

QString OnlineMp3Widget::getDownload_Md5(QString time, QString encode_album_audio_id)
{
    // 构建签名列表
    QStringList signature_list;
    signature_list << "NVPh5oo715z5DIWAeQlhMDsWXXQV4hwt"
                   << "appid=1014"
                   << "clienttime=" + time
                   << "clientver=20000"
                   << "dfid=11SITU3au0iw0OdGgJ0EhTvI"
                   << "encode_album_audio_id=" + encode_album_audio_id
                   << "mid=707708a817d80eedd95f2ae68bc57780"
                   << "platid=4"
                   << "srcappid=2919"
                   << "token="
                   << "userid=0"
                   << "uuid=707708a817d80eedd95f2ae68bc57780"
                   << "NVPh5oo715z5DIWAeQlhMDsWXXQV4hwt";

    // 将签名列表中的元素连接成一个字符串
    QString string = signature_list.join("");
    //qDebug()<< string;
    //生成 MD5 哈希
    QByteArray hashedData = QCryptographicHash::hash(string.toUtf8(), QCryptographicHash::Md5);

    // 将哈希数据转换为十六进制字符串
    QString md5Hash = hashedData.toHex();

    return md5Hash;
}

QString OnlineMp3Widget::getSearch_Md5(QString songname, QString time)
{
    // 构建签名列表
    QStringList signature_list;
    signature_list <<   "NVPh5oo715z5DIWAeQlhMDsWXXQV4hwt"
                   <<   "appid=1014"
                   <<   "bitrate=0"
                   <<   "callback=callback123"
                   <<   "clienttime=" + time
                   <<   "clientver=1000"
                   <<   "dfid=11SITU3au0iw0OdGgJ0EhTvI"
                   <<   "filter=10"
                   <<   "inputtype=0"
                   <<   "iscorrection=1"
                   <<   "isfuzzy=0"
                   <<   "keyword=" + songname
                   <<   "mid=707708a817d80eedd95f2ae68bc57780"
                   <<   "page=1"
                   <<   "pagesize=30"
                   <<   "platform=WebFilter"
                   <<   "privilege_filter=0"
                   <<   "srcappid=2919"
                   <<   "token="
                   <<   "userid=0"
                   <<   "uuid=707708a817d80eedd95f2ae68bc57780"
                   <<   "NVPh5oo715z5DIWAeQlhMDsWXXQV4hwt";

    // 将签名列表中的元素连接成一个字符串
    QString string = signature_list.join("");
    //qDebug()<< string;
    //生成 MD5 哈希
    QByteArray hashedData = QCryptographicHash::hash(string.toUtf8(), QCryptographicHash::Md5);

    // 将哈希数据转换为十六进制字符串
    QString md5Hash = hashedData.toHex();

    return md5Hash;
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


//搜索歌曲
void OnlineMp3Widget::on_btn_search_clicked()
{
    // 清空搜索队列
    ui->lw_search->clear();

    // 清理数据库中已经存储的 hash 等数据
    QSqlQuery query;
    QString sql = "delete from songlist;" ;

    if(!query.exec(sql))
    {
        QMessageBox::critical(nullptr,"错误",query.lastError().text());
    }
    QDateTime time = QDateTime::currentDateTime();
    // 将当前时间转换为自纪元以来的秒数，并将其转换为字符串
    QString currentTimeString = QString::number(time.toSecsSinceEpoch()*1000);

    QString signaturecode = getSearch_Md5(ui->le_search->text(),currentTimeString);
    // 根据用户输入的 MP3 名称发起操作请求
    QString url = kugouSearchApi + QString("callback=callback123"
                                           "&srcappid=2919"
                                           "&clientver=1000"
                                           "&clienttime=%1"
                                           "&mid=707708a817d80eedd95f2ae68bc57780"
                                           "&uuid=707708a817d80eedd95f2ae68bc57780"
                                           "&dfid=11SITU3au0iw0OdGgJ0EhTvI"
                                           "&keyword=%2"
                                           "&page=1"
                                           "&pagesize=30"
                                           "&bitrate=0"
                                           "&isfuzzy=0"
                                           "&inputtype=0"
                                           "&platform=WebFilter"
                                            "&userid=0"
                                           "&iscorrection=1"
                                           "&privilege_filter=0"
                                           "&filter=10"
                                           "&token="
                                           "&appid=1014"
                                           "&signature=%3"
                                           ).arg(currentTimeString).arg(ui->le_search->text()).arg(signaturecode);

    // 发起 HTTP 请求
    httpAccess(url);

    QByteArray JsonData;
    QEventLoop loop;

    // 等待 HTTP 请求完成并获取数据
    auto c = connect(this, &OnlineMp3Widget::finish, [&](const QByteArray &data){
        JsonData = data;
        loop.exit(1);
    });
    loop.exec();
    disconnect(c);

    // 解析获取的 JSON 数据
    hashJsonAnalysis(JsonData);
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

void OnlineMp3Widget::updateDuration(qint64)
{

}

void OnlineMp3Widget::lyricTextShow(QString str)
{

}

// 读取网络数据槽函数
void OnlineMp3Widget::netReply(QNetworkReply *reply)
{
    // 获取响应状态码，200 属于正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << status_code;

    // 重定向目标属性
    reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        // 如果没有发生网络错误，则读取响应数据
        QByteArray data = reply->readAll();
        // 发射自定义的 finish 信号，将响应数据传递给槽函数
        emit finish(data);
    }
    else
    {
        // 如果发生了网络错误，则打印错误信息
        qDebug() << reply->errorString();
    }
}

// 音乐歌曲的下载和播放
void OnlineMp3Widget::downloadPlayer(QString encode_album_audio_id)
{
    //构建下载歌曲的 URL
    QDateTime time = QDateTime::currentDateTime();
    // 将当前时间转换为自纪元以来的秒数，并将其转换为字符串
    QString currentTimeString = QString::number(time.toSecsSinceEpoch()*1000);
    // currentTimeString = "1713782920612";
    // QString encode_album_audio_id = "j5yn384";
    QString signaturecode = getDownload_Md5(currentTimeString,encode_album_audio_id);
    QString url = kugouDownldadApi + QString("srcappid=2919"
                                             "&clientver=20000"
                                             "&clienttime=%1"
                                             "&mid=707708a817d80eedd95f2ae68bc57780"
                                             "&uuid=707708a817d80eedd95f2ae68bc57780"
                                             "&dfid=11SITU3au0iw0OdGgJ0EhTvI"
                                             "&appid=1014"
                                             "&platid=4"
                                             "&encode_album_audio_id=%2"
                                             "&token="
                                             "&userid=0"
                                             "&signature=%3"
                                             ).arg(currentTimeString).arg(encode_album_audio_id).arg(signaturecode);

    // 发起 HTTP 请求获取歌曲数据
    httpAccess(url);

    QByteArray JsonData;
    QEventLoop loop;

    // 等待 HTTP 请求完成并获取数据
    auto d = connect(this, &OnlineMp3Widget::finish, [&](const QByteArray &data){
        JsonData = data;
        loop.exit(1);
    });
    loop.exec();
    disconnect(d);

    // 解析要播放的音乐
    QString music = musicJsonAnalysis(JsonData);

    // 设置媒体并播放音乐
    player->setMedia(QUrl(music));
    // 设置音量
    player->setVolume(50);
    // 设置音量滚动条
    ui->hs_sound->setValue(50);
    // 播放音乐
    player->play();
}

// 双击搜索列表，播放音乐
void OnlineMp3Widget::playSearchMusic()
{
    // 获取双击的歌曲索引，即数据表的 ID 号
    int row = ui->lw_search->currentRow();
    qDebug() << "row" << row;

    QSqlQuery query;
    QString sql = QString("select * from songlist where id = %1;").arg(row);
    if (!query.exec(sql))
    {
        QMessageBox::critical(nullptr, "select * from songlist where id =", db.lastError().text());
    }

    // 将选中的音乐的数据信息存入历史数据表
    QString songname, singername, album_id, hash;
    while (query.next())
    {
        QSqlRecord record = query.record();
        int songkey = record.indexOf("songname");
        int singerkey = record.indexOf("singername");
        int albumkey = record.indexOf("album_id");
        int hashkey = record.indexOf("hash");

        songname = query.value(songkey).toString();
        singername = query.value(singerkey).toString();
        album_id = query.value(albumkey).toString();
        hash = query.value(hashkey).toString();

        // 查询历史数据表中是否已经存在该歌曲的记录
        sql = QString("select hash from songhistory where hash = '%1'").arg(hash);
        if (!query.exec(sql))
        {
            QMessageBox::critical(nullptr, "select hash from songhistory where hash =", db.lastError().text());
        }
        // 如果不存在该记录，则将其存入历史数据表
        if (query.next() == NULL)
        {
            sql = QString("insert into songhistory values(NULL, '%1', '%2', '%3', '%4')").arg(songname).arg(singername).arg(album_id).arg(hash);
            if (!query.exec(sql))
            {
                QMessageBox::critical(nullptr, "insert error", db.lastError().text());
            }
            // 将歌手和歌名放入历史歌曲表中显示
            QString show = songname + " " + singername;
            QListWidgetItem *item = new QListWidgetItem(show);
            ui->lw_record->addItem(item);
        }
    }
    // 下载并播放选中的音乐
    downloadPlayer(album_id);
}




