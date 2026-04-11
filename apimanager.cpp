#include "apimanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString BASE_URL = "http://localhost:8000";

ApiManager::ApiManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

void ApiManager::loginUser(const QString &username) {
    QUrl url(BASE_URL + "/login/" + username);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->post(request, QByteArray());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            int id = doc.object()["user_id"].toInt();
            QString name = doc.object()["username"].toString();
            emit userLoggedIn(id, name);
        }
        reply->deleteLater();
    });
}

void ApiManager::search(const QString &query) {
    QUrl url(BASE_URL + "/search/" + query);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit searchResultsReady(doc.object()["results"].toArray());
        }
        reply->deleteLater();
    });
}

void ApiManager::fetchRecommendations(int userId) {
    QUrl url(BASE_URL + "/recommend/" + QString::number(userId));
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit recsReady(doc.object()["recommendations"].toArray());
        }
        reply->deleteLater();
    });
}

void ApiManager::fetchLikedSongs(int userId) {
    QUrl url(BASE_URL + "/liked_songs/" + QString::number(userId));
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit likesReady(doc.object()["songs"].toArray());
        }
        reply->deleteLater();
    });
}

void ApiManager::fetchArtists(int userId) {
    QUrl url(BASE_URL + "/suggest_artists/" + QString::number(userId));
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit artistsReady(doc.object()["artists"].toArray());
        }
        reply->deleteLater();
    });
}

void ApiManager::fetchFollowedArtists(int userId) {
    QUrl url(BASE_URL + "/followed_artists/" + QString::number(userId));
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit followedArtistsReady(doc.object()["artists"].toArray());
        }
        reply->deleteLater();
    });
}

void ApiManager::sendLike(int userId, int trackId) {
    QUrl url(BASE_URL + "/like/" + QString::number(userId) + "/" + QString::number(trackId));
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, [reply]() { reply->deleteLater(); });
}

void ApiManager::sendFollow(int userId, int artistId) {
    QUrl url(BASE_URL + "/follow/" + QString::number(userId) + "/" + QString::number(artistId));
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, [reply]() { reply->deleteLater(); });
}

void ApiManager::fetchArtistTracks(int artistId) {
    QUrl url(BASE_URL + "/artist/" + QString::number(artistId) + "/tracks");
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit artistTracksReady(doc.object()["tracks"].toArray());
        }
        reply->deleteLater();
    });
}
