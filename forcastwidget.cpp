#include "forcastwidget.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
#include <QStandardPaths>
#include <QTimeZone>

#include <QMessageBox>

ForcastWidget::ForcastWidget(QWidget *parent)
    : QWidget(parent),
      m_settings("deepin", "gxde-dock-HTYWeather")
{
    setFixedWidth(300);
    QGridLayout *layout = new QGridLayout;
    for (int i=0; i<6; i++) {
        labelWImg[i] = new QLabel;
        QString icon_path = ":icon/Default/na.png";
        QString iconTheme = m_settings.value("IconTheme","").toString();
        if(iconTheme != ""){
            if(!iconTheme.startsWith("/")){
                icon_path = ":icon/" + iconTheme + "/na.png";
            }else{
                QString icon_path1 = iconTheme + "/na.png";
                QFile file(icon_path1);
                if(file.exists()){
                    icon_path = icon_path1;
                }
            }
        }
        labelWImg[i]->setPixmap(QPixmap(icon_path).scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        labelWImg[i]->setAlignment(Qt::AlignCenter);
        layout->addWidget(labelWImg[i],i,0);
        labelTemp[i] = new QLabel("25°C");
        labelTemp[i]->setAlignment(Qt::AlignCenter);
        layout->addWidget(labelTemp[i],i,1);
        if (i==0) {
            labelTemp[i]->setStyleSheet("color:white;font-size:20px;");
            labelDate[i] = new QLabel(tr("City"));
            labelDate[i]->setStyleSheet("color:white;font-size:20px;");
        } else {
            labelTemp[i]->setStyleSheet("color:white;font-size:12px;");
            labelDate[i] = new QLabel(tr("01-01 Mon"));
            labelDate[i]->setStyleSheet("color:white;font-size:12px;");
        }
        labelDate[i]->setAlignment(Qt::AlignCenter);
        layout->addWidget(labelDate[i],i,2);
    }
    setLayout(layout);
}

const QString ForcastWidget::GetLocalCityName()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString stemp = "", stip = "", surl="";
    QString log = currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\n";
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply;
    QString city;
    // 自动转换城市名称
    // 从 wttr.in 自动获取
    QString place_url = "https://wttr.in/?format=j1";
    reply = manager.get(QNetworkRequest(QUrl(place_url)));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray BA = reply->readAll();
    log += place_url + "\n";
    log += BA + "\n";
    QJsonParseError place_json;
    QJsonDocument place_json_document = QJsonDocument::fromJson(BA, &place_json);
    if (place_json.error == QJsonParseError::NoError) {
        QJsonObject nearest_area = place_json_document.object().value("nearest_area").toArray().at(0).toObject();
        city = nearest_area.value("areaName").toArray().at(0).toObject().value("value").toString();
    }
    return city;
}

const QString ForcastWidget::GetCityEnglishName(const QString name)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString stemp = "", stip = "", surl="";
    QString log = currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\n";
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply;
    QString city = name;
    // 自动转换城市名称
    // 从 wttr.in 自动获取
    QString place_url = "https://wttr.in/" + name + "?format=j1";
    reply = manager.get(QNetworkRequest(QUrl(place_url)));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray BA = reply->readAll();
    log += place_url + "\n";
    log += BA + "\n";
    QJsonParseError place_json;
    QJsonDocument place_json_document = QJsonDocument::fromJson(BA, &place_json);
    if (place_json.error == QJsonParseError::NoError) {
        QJsonObject nearest_area = place_json_document.object().value("nearest_area").toArray().at(0).toObject();
        city = nearest_area.value("areaName").toArray().at(0).toObject().value("value").toString();
    }
    return city;
}

void ForcastWidget::WeatherName()
{
    tr("Clouds");
    tr("Clear");
    tr("Rain");
    tr("Snow");
}

void ForcastWidget::updateWeather()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString stemp = "", stip = "", surl="";
    QString log = currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\n";
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply;

    QString city = m_settings.value("city","").toString();
    QString country = m_settings.value("country","").toString();
    if(city != "" && country != ""){
        QString icon_path = ":icon/Default/na.png";
        QString iconTheme = m_settings.value("IconTheme","").toString();
        if(iconTheme != ""){
            if(!iconTheme.startsWith("/")){
                icon_path = ":icon/" + iconTheme + "/na.png";
            }else{
                QString icon_path1 = iconTheme + "/na.png";
                QFile file(icon_path1);
                if(file.exists()){
                    icon_path = icon_path1;
                }
            }
        }
        emit weatherNow("Weather", "Temp", currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\nGetting weather of " + city + "," + country, QPixmap(icon_path));
        // 自动转换城市名称
        QString place_url = "https://wttr.in/?format=j1";
        if(city.replace(" ", "") != "") {
            city = GetCityEnglishName(city.replace(" ", ""));
        }
        else {
            city = GetLocalCityName();
        }
        // 获取天气信息
        QString appid = "8f3c852b69f0417fac76cd52c894ba63";
        surl = "https://api.openweathermap.org/data/2.5/forecast?q=" + city + "&appid=" + appid;
        /*if(city.replace(" ", "") != "") {
            surl = "https://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + country + "&appid=" + appid;
        }
        else {
            surl = "https://api.openweathermap.org/data/2.5/forecast?q=" + city + "&appid=" + appid;
        }*/
        reply = manager.get(QNetworkRequest(QUrl(surl)));
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        QByteArray BA = reply->readAll();
        log += surl + "\n";
        log += BA + "\n";
        QJsonParseError JPE;
        QJsonDocument JD = QJsonDocument::fromJson(BA, &JPE);
        if (JPE.error == QJsonParseError::NoError) {
            QString cod = JD.object().value("cod").toString();
            if(cod == "200"){
                QJsonObject JO_city = JD.object().value("city").toObject();
                QJsonObject coord = JO_city.value("coord").toObject();
                double lat = coord.value("lat").toDouble();
                double lon = coord.value("lon").toDouble();
                m_settings.setValue("lat", lat);
                m_settings.setValue("lon", lon);
                QDateTime time_sunrise = QDateTime::fromMSecsSinceEpoch(JO_city.value("sunrise").toInt()*1000L, Qt::LocalTime);
                QDateTime time_sunset = QDateTime::fromMSecsSinceEpoch(JO_city.value("sunset").toInt()*1000L, Qt::LocalTime);
                QJsonArray list = JD.object().value("list").toArray();
                int r = 0;
                for (int i=0; i<list.size(); i++) {
                    QDateTime date = QDateTime::fromMSecsSinceEpoch(list[i].toObject().value("dt").toInt()*1000L, Qt::UTC);
                    QString sdate = date.toString("MM-dd ddd");
                    QString dt_txt = list[i].toObject().value("dt_txt").toString();
                    double temp = list[i].toObject().value("main").toObject().value("temp").toDouble() - 273.15;
                    stemp = QString::number(qRound(temp)) + "°C";
                    if(m_settings.value("TemperatureUnit","°C").toString() == "°F"){
                        stemp = QString::number(qRound(temp*1.8 + 32)) + "°F";
                    }
                    QString humidity = "RH: " + QString::number(list[i].toObject().value("main").toObject().value("humidity").toInt()) + "%";
                    QString weather = tr(list[i].toObject().value("weather").toArray().at(0).toObject().value("main").toString().toStdString().c_str());
                    QString icon_name = list[i].toObject().value("weather").toArray().at(0).toObject().value("icon").toString() + ".png";
                    QString icon_path = ":icon/Default/" + icon_name;
                    QString iconTheme = m_settings.value("IconTheme","").toString();
                    if(iconTheme != ""){
                        if(!iconTheme.startsWith("/")){
                            icon_path = ":icon/" + iconTheme + "/" + icon_name;
                        }else{
                            QString icon_path1 = iconTheme + "/" + icon_name;
                            QFile file(icon_path1);
                            if(file.exists()){
                                icon_path = icon_path1;
                            }
                        }
                    }
                    QString wind = "Wind: " + QString::number(list[i].toObject().value("wind").toObject().value("speed").toDouble()) + "m/s, " + QString::number(qRound(list[i].toObject().value("wind").toObject().value("deg").toDouble())) + "°";
                    log += dt_txt + ", " + date.toString("yyyy-MM-dd HH:mm:ss ddd") + ", " + stemp + ", " + humidity + ","+ weather + ", " + icon_path + ", " + wind + "\n";
                    if(date.time() == QTime(12,0,0)){
                        if (r == 0) {
                            QPixmap pixmap(icon_path);
                            labelWImg[0]->setPixmap(pixmap.scaled(80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                            labelTemp[0]->setText(stemp);
                            labelDate[0]->setText(JO_city.value("name").toString());
                            labelWImg[1]->setPixmap(QPixmap(icon_path).scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                            labelTemp[1]->setText(weather + " " + stemp);
                            labelDate[1]->setText(sdate);
                            stip = city + ", " + country + "\n" + weather + "\n" + stemp + "\n" + humidity + "\n" + wind + "\nSunrise: " + time_sunrise.toString("hh:mm") + "\nSunset: " + time_sunset.toString("hh:mm") + "\nRefresh：" + currentDateTime.toString("HH:mm:ss");
                            emit weatherNow(weather, stemp, stip, pixmap);
                            r++;
                        } else {
                            labelWImg[r]->setPixmap(QPixmap(icon_path).scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                            labelTemp[r]->setText(weather + " " + stemp);
                            labelDate[r]->setText(sdate);
                        }
                        r++;
                    }
                }
            } else {
                emit weatherNow("Weather", "Temp", city + ", " + country + "\n" + cod + "\n" + JD.object().value("message").toString(), QPixmap(":icon/na.png"));
            }
        }else{
            emit weatherNow("Weather", "Temp", QString(BA), QPixmap(":icon/na.png"));
        }

        // log
        QString path = QStandardPaths::standardLocations(QStandardPaths::CacheLocation).first() + "/HTYWeather.log";
        QFile file(path);
        if (file.open(QFile::WriteOnly)) {
            file.write(log.toUtf8());
            file.close();
        }
    }
}
