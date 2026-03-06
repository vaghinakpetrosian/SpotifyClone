#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class ApiManager : public QObject {
    Q_OBJECT

public:
    explicit ApiManager(QObject *parent = nullptr);
    void fetchRecommendations(int userId);
    void fetchLikedSongs(int userId);
    void fetchArtists(int userId);
    void sendLike(int userId, int trackId);
    void sendFollow(int userId, int artistId);
    void fetchFollowedArtists(int userId);

    void loginUser(const QString &username);
    void search(const QString &query);

signals:
    void recsReady(const QJsonArray &tracks);
    void likesReady(const QJsonArray &tracks);
    void artistsReady(const QJsonArray &artists);
    void followedArtistsReady(const QJsonArray &artists);
    void userLoggedIn(int userId, const QString &username);
    void searchResultsReady(const QJsonArray &tracks);
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *networkManager;
};

#endif
