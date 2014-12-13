#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QFile>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include "server.h"

Server::Server(QObject *parent) :
    QObject(parent),
    _manager(new QNetworkAccessManager(this))
{
}

Server::Server(QString url, QObject *parent) :
    _url(url),
    QObject(parent),
    _manager(new QNetworkAccessManager(this))
{
}

Server::~Server()
{
}

void Server::setUrl(QString url)
{
    _url = url;
}

void Server::upload(QByteArray bytes)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"image\"; filename=\"screen.png\""));

    imagePart.setBody(bytes);
    multiPart->append(imagePart);

    QNetworkRequest request;
    request.setUrl(QUrl("http://" + _url + "/screen/upload"));
    request.setRawHeader("User-Agent", "OpenScreenCloud client");

    connect(_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(uploaded(QNetworkReply*)));

    _reply = _manager->post(request, multiPart);

    _reply->setParent(_manager);
    multiPart->setParent(_reply); // delete the multiPart with the reply
}

void Server::uploaded(QNetworkReply *reply)
{
    disconnect(_manager, SIGNAL(finished(QNetworkReply*)),
               this, SLOT(uploaded(QNetworkReply*)));

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
    reply->close();

    QJsonObject jsonObject = jsonResponse.object();

    if (0 == jsonObject["status"].toString().compare("ok")) {
        qDebug() << jsonObject["url"].toString();
    }
}

void Server::version()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://" + _url + "/dist/version"));
    request.setRawHeader("User-Agent", "OpenScreenCloud client");

    _manager->get(request);

    connect(_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(versionFromServer(QNetworkReply*)));
}

void Server::versionFromServer(QNetworkReply *reply)
{
    if (QNetworkReply::NoError != reply->error()) {
        emit(connectionError());
        return;
    }

    emit(serverVersion(QString(reply->readAll().simplified())));
}
