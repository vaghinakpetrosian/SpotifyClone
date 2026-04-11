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
#include <QEvent>
#include <QPointer>
#include <QSet>
#include "apimanager.h"

class MainWindow : public QMainWindow
{
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
    void updateArtistTracksList(const QJsonArray &tracks);
    void playTrack(QListWidgetItem *item);
    void onLikeClicked();
    void toggleLikePlayingTrack();
    void onFollowClicked();
    void onUnfollowClicked();
    void onLoginClicked();
    void onUserLoggedIn(int userId, const QString &username);
    void onSearchClicked();
    void onArtistClicked(QListWidgetItem *item);
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
    void openNowPlayingWindow();
private:
    int currentUserId;
    int currentTrackId;
    bool isShuffle;
    int repeatState;
    int customRepeatN;
    int customRepeatStartRow;
    int previousPageIndex;
    int currentLyricIndex;

    QLabel *nowPlayingTimeLabel;
    QListWidget *currentPlayingList;
    QSet<int> likedTrackIds;
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
    QWidget *artistProfilePage;
    QLabel *artistProfileNameLabel;
    QLabel *artistProfileImageLabel;
    QListWidget *artistProfileTrackList;
    QPushButton *btnBackFromProfile;
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
    QPushButton *btnLikePlaying;
    QSlider *volumeSlider;
    QSlider *progressSlider;
    QLabel *timeLabel;
    ApiManager *apiManager;
    QNetworkAccessManager *imageManager;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QString currentCoverUrl;
    QString currentLyrics;
    QWidget *playerBarWidget;
    QWidget *nowPlayingPage;
    QPushButton *btnBackFromNowPlaying;
    QLabel *nowPlayingCoverLabel;
    QLabel *nowPlayingTitleLabel;
    QLabel *nowPlayingArtistLabel;
    QListWidget *lyricsListWidget;
    QSlider *nowPlayingSlider;
    QLabel *nowPlayingCurrentTime;
    QLabel *nowPlayingTotalTime;
    QPushButton *btnNowPlayingLike;
    QPushButton *btnNowPlayingShuffle;
    QPushButton *btnNowPlayingPrev;
    QPushButton *btnNowPlayingPlayPause;
    QPushButton *btnNowPlayingNext;
    QPushButton *btnNowPlayingRepeat;
    QListWidget* getActiveTrackList();
    void loadCircularImage(const QString &urlString, QListWidgetItem *item);
    void loadSquareImage(const QString &urlString, QLabel *label);
    void loadSquareIcon(const QString &urlString, QListWidgetItem *item);
    void addTrackItem(QListWidget *list, const QJsonObject &obj);
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif
