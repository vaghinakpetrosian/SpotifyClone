#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class ApiManager : public QObject {
    Q_OBJECT
public:
    explicit ApiManager(QObject *parent = nullptr);
    void loginUser(const QString &username);
    void search(const QString &query);
    void fetchRecommendations(int userId);
    void fetchLikedSongs(int userId);
    void fetchArtists(int userId);
    void fetchFollowedArtists(int userId);
    void sendLike(int userId, int trackId);
    void sendFollow(int userId, int artistId);
    void fetchArtistTracks(int artistId);

signals:
    void userLoggedIn(int userId, const QString &username);
    void searchResultsReady(const QJsonArray &results);
    void recsReady(const QJsonArray &tracks);
    void likesReady(const QJsonArray &tracks);
    void artistsReady(const QJsonArray &artists);
    void followedArtistsReady(const QJsonArray &artists);
    void artistTracksReady(const QJsonArray &tracks);

private:
    QNetworkAccessManager *networkManager;
};

#endif
