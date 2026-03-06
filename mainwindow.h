#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QPainterPath>
#include "apimanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void updateRecsList(const QJsonArray &tracks);
    void updateLikesList(const QJsonArray &tracks);
    void updateArtistList(const QJsonArray &artists);
    void updateFollowedArtistList(const QJsonArray &artists);
    void updateSearchList(const QJsonArray &tracks);
    void playTrack(QListWidgetItem *item);
    void onLikeClicked();
    void onFollowClicked();
    void onUnfollowClicked();
    void onLoginClicked();
    void onUserLoggedIn(int userId, const QString &username);
    void onSearchClicked();
    void togglePlayPause();
    void playNext();
    void playPrevious();
    void toggleShuffle();
    void toggleRepeat();
    void changeVolume(int value);
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void setPosition(int position);
private:
    int currentUserId;
    bool isShuffle;
    int repeatState;
    QLineEdit *usernameInput;
    QPushButton *btnLogin;
    QLabel *currentUserLabel;
    QLineEdit *searchInput;
    QPushButton *btnSearch;
    QStackedWidget *stackedWidget;
    QListWidget *recList;
    QListWidget *likesFolderList;
    QListWidget *artistList;
    QListWidget *followedArtistList;
    QListWidget *searchList;
    QPushButton *btnFollowing;
    QPushButton *btnLikeRecs;
    QPushButton *btnRemoveLike;
    QPushButton *btnFollow;
    QPushButton *btnUnfollow;
    QPushButton *btnLikeSearch;
    QLabel *currentCoverLabel;
    QLabel *currentTitleLabel;
    QLabel *currentArtistLabel;
    QPushButton *btnShuffle;
    QPushButton *btnPrev;
    QPushButton *btnPlayPause;
    QPushButton *btnNext;
    QPushButton *btnRepeat;
    QSlider *volumeSlider;
    QSlider *progressSlider;
    QLabel *timeLabel;
    ApiManager *apiManager;
    QNetworkAccessManager *imageManager;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QListWidget* getActiveTrackList();
    void loadCircularImage(const QString &urlString, QListWidgetItem *item);
    void loadSquareImage(const QString &urlString, QLabel *label);
};

#endif
