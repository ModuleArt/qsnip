#include "OAuth.h"
#include "src/Context.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>

const QString DROPBOX_URL = "https://api.dropboxapi.com/1/oauth2/token";
const QString DROPBOX_CLIENT_ID = "wmae5esncijnqie";
const QString DROPBOX_CLIENT_SECRET = "3fp3vak8ijd1l1e";

const QString GOOGLE_URL = "https://www.googleapis.com/oauth2/v4/token";
const QString GOOGLE_CLIENT_ID = "445396168271-7bankmpva981k4d74ebi11o7avgle1qa.apps.googleusercontent.com";
const QString GOOGLE_CLIENT_SECRET = "_QjArq04n7dFs3TgyyB5w3D-";

OAuth::OAuth() : refreshing_(false) {
    ui.setupUi(this);

    // FIXME
    // connect(&manager_, SIGNAL(finished(QNetworkReply*)),
    //         this, SLOT(onTokenReply(QNetworkReply*)));

    // Refresh tokens
    // connect(&Context::getInstance().google, SIGNAL(onRefreshToken(const UploadService)),
    //         this, SLOT(onRefreshToken(UploadService)));
}

void OAuth::setService(const UploadService service) {
    service_ = service;
    QString text("1. Open in browser <a href=\"{LINK}\">ссылку</a>.");

    switch (service) {
        case UploadService::DROPBOX:
            text.replace("{LINK}", "https://www.dropbox.com/1/oauth2/authorize?response_type=code&client_id=" + DROPBOX_CLIENT_ID);
            break;

        case UploadService::GOOGLE:
            text.replace("{LINK}", "https://accounts.google.com/o/oauth2/v2/auth?response_type=code&scope=https://www.googleapis.com/auth/drive.file&redirect_uri=urn:ietf:wg:oauth:2.0:oob&client_id=" + GOOGLE_CLIENT_ID);
            break;

        case UploadService::CLIPBOARD:
            break;
    }

    ui.link->setText(text.replace("{LINK}", text));
}

void OAuth::accept() {
    ui.submitButtons->setEnabled(false);

    switch (service_) {
        case UploadService::DROPBOX:
            getToken(DROPBOX_URL, DROPBOX_CLIENT_ID, DROPBOX_CLIENT_SECRET);
            break;

        case UploadService::GOOGLE:
            getToken(GOOGLE_URL, GOOGLE_CLIENT_ID, GOOGLE_CLIENT_SECRET);
            break;

        case UploadService::CLIPBOARD:
            break;
    }
}

void OAuth::onTokenReply(QNetworkReply* reply) {
    ui.submitButtons->setEnabled(true);

    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
    reply->close();

    Context& ctx = Context::getInstance();

    QJsonObject jsonObject = jsonResponse.object();
    if (reply->error() != QNetworkReply::NoError) {
        ctx.trayIcon->showMessage("Error", jsonObject["error_description"].toString(), QSystemTrayIcon::Critical, 10000);
        qInfo() << jsonResponse.toJson(QJsonDocument::Compact);
    } else {
        hide();

        QString token = jsonObject["access_token"].toString();
        switch (service_) {
            case UploadService::DROPBOX:
                ctx.settings->setDropboxToken(token);
                ctx.dropbox->setToken(token);
                break;

            case UploadService::GOOGLE:
                ctx.settings->setGoogleToken(token);
                ctx.google->setToken(token);

                if (jsonObject["refresh_token"].toString().length() > 0) {
                    ctx.settings->setGoogleRefreshToken(jsonObject["refresh_token"].toString());
                }
                break;

            case UploadService::CLIPBOARD:
                break;
        }

        ctx.settingsForm->setError("");

        if (!refreshing_) {
            ctx.settingsForm->show();
        }

        emit tokenRefreshed();
    }

    refreshing_ = false;
}

void OAuth::refreshToken(const UploadService service) {
    qInfo() << "Refreshing token";
    service_ = service;
    refreshing_ = true;

    switch (service) {
        case UploadService::GOOGLE:
            getToken(GOOGLE_URL, GOOGLE_CLIENT_ID, GOOGLE_CLIENT_SECRET);
            break;

        case UploadService::DROPBOX:
        case UploadService::CLIPBOARD:
            break;
    }
}

void OAuth::getToken(const QString& url, const QString& clientId, const QString& clientSecret) {
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QUrlQuery query;
    query.addQueryItem("client_id", clientId);
    query.addQueryItem("client_secret", clientSecret);

    if (!refreshing_) {
        query.addQueryItem("grant_type", "authorization_code");
        query.addQueryItem("code", ui.code->text());

        if (service_ == UploadService::GOOGLE) {
            query.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
        }
    } else {
        query.addQueryItem("grant_type", "refresh_token");
        query.addQueryItem("refresh_token", Context::getInstance().settings->googleRefreshToken());
    }

    manager_.post(request, query.toString().toLatin1());
}
