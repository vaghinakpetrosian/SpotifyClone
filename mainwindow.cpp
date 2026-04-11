#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QTime>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QIcon>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QInputDialog>
#include <QStringList>
#include <QSize>
#include <QVariant>

static QString formatSongDuration(int seconds)
{
    if (seconds <= 0)
    {
        return "";
    }
    return QString("%1:%2").arg(seconds / 60).arg(seconds % 60, 2, 10, QChar('0'));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->currentUserId = 0;
    this->currentTrackId = 0;
    this->isShuffle = false;
    this->repeatState = 0;
    this->customRepeatN = 0;
    this->customRepeatStartRow = -1;
    this->previousPageIndex = 0;
    this->currentLyricIndex = -1;
    this->currentPlayingList = nullptr;

    QWidget *central = new QWidget(this);
    central->setObjectName("CentralWidget");
    QVBoxLayout *appLayout = new QVBoxLayout(central);

    // Top Bar
    QWidget *topBar = new QWidget();
    topBar->setFixedHeight(60);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);

    this->usernameInput = new QLineEdit();
    this->usernameInput->setPlaceholderText("Enter username...");
    this->usernameInput->setFixedWidth(150);

    this->btnLogin = new QPushButton("Sign In");
    this->btnLogin->setObjectName("ActionBtn");

    this->currentUserLabel = new QLabel("Not logged in");
    this->currentUserLabel->setStyleSheet("color: #b3b3b3; font-style: italic;");

    this->searchInput = new QLineEdit();
    this->searchInput->setPlaceholderText("Search tracks or artists...");
    this->searchInput->setFixedWidth(200);

    this->btnSearch = new QPushButton("🔍");
    this->btnSearch->setStyleSheet("background-color: #282828; color: white; border-radius: 3px; padding: 5px;");

    topLayout->addWidget(new QLabel("👤"));
    topLayout->addWidget(this->usernameInput);
    topLayout->addWidget(this->btnLogin);
    topLayout->addWidget(this->currentUserLabel);
    topLayout->addStretch();
    topLayout->addWidget(this->searchInput);
    topLayout->addWidget(this->btnSearch);

    // Main Layout
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(8, 0, 8, 8);
    mainLayout->setSpacing(8);

    // Sidebar
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(180);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    QPushButton *btnHome = new QPushButton("My Vibe");
    QPushButton *btnArtists = new QPushButton("Find Artists");
    this->btnFollowing = new QPushButton("Following");
    QPushButton *btnLibrary = new QPushButton("Liked Songs");

    sideLayout->addWidget(new QLabel("SPOTIFY CLONE"));
    sideLayout->addWidget(btnHome);
    sideLayout->addWidget(btnArtists);
    sideLayout->addWidget(this->btnFollowing);
    sideLayout->addWidget(btnLibrary);
    sideLayout->addStretch();

    this->stackedWidget = new QStackedWidget();

    // Home
    QWidget *homePage = new QWidget();
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    this->recList = new QListWidget();
    this->btnLikeRecs = new QPushButton("♥ Like / Unlike Selected");
    this->btnLikeRecs->setObjectName("ActionBtn");
    homeLayout->addWidget(new QLabel("Recommended for You"));
    homeLayout->addWidget(this->recList);
    homeLayout->addWidget(this->btnLikeRecs);

    // Artists
    QWidget *artistPage = new QWidget();
    QVBoxLayout *artistLayout = new QVBoxLayout(artistPage);
    this->artistList = new QListWidget();
    this->artistList->setViewMode(QListView::IconMode);
    this->artistList->setIconSize(QSize(150, 150));
    this->artistList->setResizeMode(QListView::Adjust);
    this->artistList->setMovement(QListView::Static);
    this->artistList->setWordWrap(true);
    this->artistList->setSpacing(20);
    this->btnFollow = new QPushButton("➕ Follow Selected Artist");
    this->btnFollow->setObjectName("ActionBtn");
    artistLayout->addWidget(new QLabel("Artists You Might Like"));
    artistLayout->addWidget(this->artistList);
    artistLayout->addWidget(this->btnFollow);

    // Liked Songs
    QWidget *likesPage = new QWidget();
    QVBoxLayout *likesLayout = new QVBoxLayout(likesPage);
    this->likesFolderList = new QListWidget();
    this->btnRemoveLike = new QPushButton("❌ Remove Selected from Liked");
    this->btnRemoveLike->setObjectName("ActionBtn");
    likesLayout->addWidget(new QLabel("Your Liked Songs"));
    likesLayout->addWidget(this->likesFolderList);
    likesLayout->addWidget(this->btnRemoveLike);

    // Following
    QWidget *followingPage = new QWidget();
    QVBoxLayout *followingLayout = new QVBoxLayout(followingPage);
    this->followedArtistList = new QListWidget();
    this->followedArtistList->setViewMode(QListView::IconMode);
    this->followedArtistList->setIconSize(QSize(150, 150));
    this->followedArtistList->setResizeMode(QListView::Adjust);
    this->followedArtistList->setMovement(QListView::Static);
    this->followedArtistList->setWordWrap(true);
    this->followedArtistList->setSpacing(20);
    this->btnUnfollow = new QPushButton("❌ Unfollow Selected Artist");
    this->btnUnfollow->setObjectName("ActionBtn");
    followingLayout->addWidget(new QLabel("Artists You Follow"));
    followingLayout->addWidget(this->followedArtistList);
    followingLayout->addWidget(this->btnUnfollow);

    // Search Results
    QWidget *searchPage = new QWidget();
    QVBoxLayout *searchLayout = new QVBoxLayout(searchPage);
    this->searchList = new QListWidget();
    this->btnLikeSearch = new QPushButton("♥ Like / Unlike Selected");
    this->btnLikeSearch->setObjectName("ActionBtn");
    searchLayout->addWidget(new QLabel("Search Results"));
    searchLayout->addWidget(this->searchList);
    searchLayout->addWidget(this->btnLikeSearch);

    // Artist Profile Page
    this->artistProfilePage = new QWidget();
    QVBoxLayout *profileLayout = new QVBoxLayout(this->artistProfilePage);
    QHBoxLayout *profileTopLayout = new QHBoxLayout();
    profileTopLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    this->btnBackFromProfile = new QPushButton("← Back");
    this->btnBackFromProfile->setFixedWidth(80);
    this->btnBackFromProfile->setObjectName("ActionBtn");

    this->artistProfileImageLabel = new QLabel();
    this->artistProfileImageLabel->setFixedSize(100, 100);
    this->artistProfileImageLabel->setStyleSheet("background-color: transparent; border-radius: 50px;");

    this->artistProfileNameLabel = new QLabel("Artist Name");
    this->artistProfileNameLabel->setStyleSheet("font-size: 36px; font-weight: bold; margin-left: 15px;");

    profileTopLayout->addWidget(this->btnBackFromProfile);
    profileTopLayout->addSpacing(20);
    profileTopLayout->addWidget(this->artistProfileImageLabel);
    profileTopLayout->addWidget(this->artistProfileNameLabel);
    profileTopLayout->addStretch();

    this->artistProfileTrackList = new QListWidget();

    profileLayout->addLayout(profileTopLayout);
    profileLayout->addSpacing(15);
    profileLayout->addWidget(this->artistProfileTrackList);

    // Now Playing / Lyrics Page
    this->nowPlayingPage = new QWidget();
    this->nowPlayingPage->setObjectName("NowPlayingPage");
    this->nowPlayingPage->setStyleSheet("QWidget#NowPlayingPage { background-color: #2a2a2a; border-radius: 8px; }");
    QVBoxLayout *npMainLayout = new QVBoxLayout(this->nowPlayingPage);
    npMainLayout->setContentsMargins(30, 20, 30, 20);

    QHBoxLayout *npTopLayout = new QHBoxLayout();
    this->btnBackFromNowPlaying = new QPushButton("← Back");
    this->btnBackFromNowPlaying->setFixedWidth(80);
    this->btnBackFromNowPlaying->setObjectName("ActionBtn");
    npTopLayout->addWidget(this->btnBackFromNowPlaying);
    npTopLayout->addStretch();
    npMainLayout->addLayout(npTopLayout);

    QHBoxLayout *npContentLayout = new QHBoxLayout();

    QVBoxLayout *npLeftLayout = new QVBoxLayout();
    npLeftLayout->setAlignment(Qt::AlignVCenter);

    this->nowPlayingCoverLabel = new QLabel();
    this->nowPlayingCoverLabel->setFixedSize(400, 400); // Changed to 400x400
    this->nowPlayingCoverLabel->setStyleSheet("background-color: #121212; border-radius: 8px;");

    this->nowPlayingTitleLabel = new QLabel("No track playing");
    this->nowPlayingTitleLabel->setStyleSheet("font-size: 28px; font-weight: bold; margin-top: 20px;");
    this->nowPlayingTitleLabel->setAlignment(Qt::AlignCenter);

    this->nowPlayingArtistLabel = new QLabel("-");
    this->nowPlayingArtistLabel->setStyleSheet("font-size: 18px; color: #b3b3b3;");
    this->nowPlayingArtistLabel->setAlignment(Qt::AlignCenter);

    this->btnNowPlayingLike = new QPushButton("♡");
    this->btnNowPlayingLike->setFixedSize(45, 45);
    this->btnNowPlayingLike->setStyleSheet("background: transparent; color: #b3b3b3; font-size: 28px; border: none; padding: 0px;");

    this->nowPlayingSlider = new QSlider(Qt::Horizontal);
    this->nowPlayingTimeLabel = new QLabel("00:00 / 00:00");
    this->nowPlayingTimeLabel->setStyleSheet("color: #b3b3b3; font-size: 13px;");

    QHBoxLayout *npControlsLayout = new QHBoxLayout();
    npControlsLayout->setAlignment(Qt::AlignCenter);
    npControlsLayout->setSpacing(20);

    this->btnNowPlayingShuffle = new QPushButton();
    this->btnNowPlayingPrev = new QPushButton();
    this->btnNowPlayingPlayPause = new QPushButton();
    this->btnNowPlayingNext = new QPushButton();
    this->btnNowPlayingRepeat = new QPushButton();

    this->btnNowPlayingShuffle->setIcon(QIcon("/home/hnin/SpotifyClone/icons/shuffle.png"));
    this->btnNowPlayingPrev->setIcon(QIcon("/home/hnin/SpotifyClone/icons/prev.png"));
    this->btnNowPlayingPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/play.png"));
    this->btnNowPlayingNext->setIcon(QIcon("/home/hnin/SpotifyClone/icons/next.png"));
    this->btnNowPlayingRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));

    int npIconSize = 24;
    npControlsLayout->addWidget(this->btnNowPlayingLike);
    this->btnNowPlayingShuffle->setIconSize(QSize(npIconSize, npIconSize));
    this->btnNowPlayingPrev->setIconSize(QSize(npIconSize, npIconSize));
    this->btnNowPlayingPlayPause->setIconSize(QSize(npIconSize, npIconSize));
    this->btnNowPlayingNext->setIconSize(QSize(npIconSize, npIconSize));
    this->btnNowPlayingRepeat->setIconSize(QSize(npIconSize, npIconSize));

    QString controlStyle = "QPushButton { background-color: transparent; border: none; padding: 5px; border-radius: 5px; } "
                           "QPushButton:hover { background-color: rgba(255,255,255,0.1); }";

    this->btnNowPlayingShuffle->setStyleSheet(controlStyle);
    this->btnNowPlayingPrev->setStyleSheet(controlStyle);
    this->btnNowPlayingNext->setStyleSheet(controlStyle);
    this->btnNowPlayingRepeat->setStyleSheet(controlStyle);

    this->btnNowPlayingPlayPause->setFixedSize(50, 50);
    this->btnNowPlayingPlayPause->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 25px; } "
                                                "QPushButton:hover { background-color: rgba(255,255,255,0.1); transform: scale(1.05); }");

    npControlsLayout->addWidget(this->btnNowPlayingShuffle);
    npControlsLayout->addWidget(this->btnNowPlayingPrev);
    npControlsLayout->addWidget(this->btnNowPlayingPlayPause);
    npControlsLayout->addWidget(this->btnNowPlayingNext);
    npControlsLayout->addWidget(this->btnNowPlayingRepeat);

    QHBoxLayout *npProgressLayout = new QHBoxLayout();
    npProgressLayout->addWidget(this->nowPlayingSlider);
    npProgressLayout->addWidget(this->nowPlayingTimeLabel);

    npLeftLayout->addStretch();
    npLeftLayout->addWidget(this->nowPlayingCoverLabel, 0, Qt::AlignHCenter);
    npLeftLayout->addSpacing(25);
    npLeftLayout->addWidget(this->nowPlayingTitleLabel, 0, Qt::AlignHCenter);
    npLeftLayout->addWidget(this->nowPlayingArtistLabel, 0, Qt::AlignHCenter);
    npLeftLayout->addSpacing(20);
    npLeftLayout->addLayout(npControlsLayout);
    npLeftLayout->addLayout(npProgressLayout);
    npLeftLayout->addStretch();

    this->lyricsListWidget = new QListWidget();
    this->lyricsListWidget->setStyleSheet(
        "QListWidget { background: transparent; border: none; outline: 0; }"
        "QListWidget::item { padding: 10px; }"
        "QScrollBar:vertical { width: 0px; }"
        );
    this->lyricsListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    this->lyricsListWidget->setWordWrap(true);
    this->lyricsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    npContentLayout->addLayout(npLeftLayout, 1);
    npContentLayout->addSpacing(40);
    npContentLayout->addWidget(this->lyricsListWidget, 2);
    npMainLayout->addLayout(npContentLayout);
    this->stackedWidget->addWidget(homePage);
    this->stackedWidget->addWidget(artistPage);
    this->stackedWidget->addWidget(likesPage);
    this->stackedWidget->addWidget(followingPage);
    this->stackedWidget->addWidget(searchPage);
    this->stackedWidget->addWidget(this->artistProfilePage);
    this->stackedWidget->addWidget(this->nowPlayingPage);
    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(this->stackedWidget);

    // GLOBAL PLAYER BAR
    this->playerBarWidget = new QWidget();
    this->playerBarWidget->setFixedHeight(90);
    this->playerBarWidget->setStyleSheet("background-color: #000000; border-top: none;");
    QHBoxLayout *playerMainLayout = new QHBoxLayout(this->playerBarWidget);
    this->playerBarWidget->setObjectName("PlayerBarContainer");
    this->playerBarWidget->installEventFilter(this);

    // LEFT COLUMN
    QWidget *leftWidget = new QWidget();
    leftWidget->setFixedWidth(350);
    QHBoxLayout *leftLayout = new QHBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    this->currentCoverLabel = new QLabel();
    this->currentCoverLabel->setFixedSize(56, 56);
    this->currentCoverLabel->setStyleSheet("background-color: #282828; border-radius: 5px;");

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setAlignment(Qt::AlignVCenter);
    this->currentTitleLabel = new QLabel("No track playing");
    this->currentTitleLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    this->currentArtistLabel = new QLabel("-");
    this->currentArtistLabel->setStyleSheet("color: #b3b3b3; font-size: 12px;");

    this->currentTitleLabel->setMaximumWidth(180);
    this->currentArtistLabel->setMaximumWidth(180);

    infoLayout->addWidget(this->currentTitleLabel);
    infoLayout->addWidget(this->currentArtistLabel);

    this->btnLikePlaying = new QPushButton("♡");
    this->btnLikePlaying->setFixedSize(45, 45);
    this->btnLikePlaying->setStyleSheet("background: transparent; color: #b3b3b3; font-size: 28px; border: none; padding: 0px;");

    leftLayout->addWidget(this->currentCoverLabel);
    leftLayout->addSpacing(15);
    leftLayout->addLayout(infoLayout);
    leftLayout->addSpacing(5);
    leftLayout->addWidget(this->btnLikePlaying);
    leftLayout->addStretch();

    // CENTER COLUMN
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setAlignment(Qt::AlignCenter);
    controlsLayout->setSpacing(15);

    this->btnShuffle = new QPushButton();
    this->btnPrev = new QPushButton();
    this->btnPlayPause = new QPushButton();
    this->btnNext = new QPushButton();
    this->btnRepeat = new QPushButton();

    this->btnShuffle->setIcon(QIcon("/home/hnin/SpotifyClone/icons/shuffle.png"));
    this->btnPrev->setIcon(QIcon("/home/hnin/SpotifyClone/icons/prev.png"));
    this->btnPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/play.png"));
    this->btnNext->setIcon(QIcon("/home/hnin/SpotifyClone/icons/next.png"));
    this->btnRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));

    int iconSize = 24;
    this->btnShuffle->setIconSize(QSize(iconSize, iconSize));
    this->btnPrev->setIconSize(QSize(iconSize, iconSize));
    this->btnPlayPause->setIconSize(QSize(iconSize, iconSize));
    this->btnNext->setIconSize(QSize(iconSize, iconSize));
    this->btnRepeat->setIconSize(QSize(iconSize, iconSize));

    this->btnShuffle->setStyleSheet(controlStyle);
    this->btnPrev->setStyleSheet(controlStyle);
    this->btnNext->setStyleSheet(controlStyle);
    this->btnRepeat->setStyleSheet(controlStyle);
    this->btnPlayPause->setFixedSize(40, 40);
    this->btnPlayPause->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 20px; } "
                                      "QPushButton:hover { background-color: rgba(255,255,255,0.1); transform: scale(1.05); }");

    controlsLayout->addWidget(this->btnShuffle);
    controlsLayout->addWidget(this->btnPrev);
    controlsLayout->addWidget(this->btnPlayPause);
    controlsLayout->addWidget(this->btnNext);
    controlsLayout->addWidget(this->btnRepeat);

    QHBoxLayout *progressLayout = new QHBoxLayout();
    this->timeLabel = new QLabel("00:00 / 00:00");
    this->timeLabel->setStyleSheet("color: #b3b3b3; font-size: 12px;");
    this->progressSlider = new QSlider(Qt::Horizontal);
    this->progressSlider->setRange(0, 0);
    progressLayout->addWidget(this->progressSlider);
    progressLayout->addWidget(this->timeLabel);

    centerLayout->addLayout(controlsLayout);
    centerLayout->addLayout(progressLayout);

    // RIGHT COLUMN
    QWidget *rightWidget = new QWidget();
    rightWidget->setFixedWidth(200);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *volIcon = new QLabel("🔈");
    volIcon->setStyleSheet("color: #b3b3b3; font-size: 16px;");
    this->volumeSlider = new QSlider(Qt::Horizontal);
    this->volumeSlider->setRange(0, 100);
    this->volumeSlider->setValue(50);
    this->volumeSlider->setFixedWidth(100);

    rightLayout->addWidget(volIcon);
    rightLayout->addWidget(this->volumeSlider);

    playerMainLayout->addWidget(leftWidget);
    playerMainLayout->addWidget(centerWidget, 1);
    playerMainLayout->addWidget(rightWidget);

    appLayout->addWidget(topBar);
    appLayout->addLayout(mainLayout);
    appLayout->addWidget(this->playerBarWidget);
    appLayout->setContentsMargins(0, 0, 0, 0);

    QString spotifyTheme = R"(
        QWidget#CentralWidget {
            background-color: #000000;
        }
        QMainWindow {
            background-color: #000000;
            font-family: 'Segoe UI', Helvetica, Arial, sans-serif;
        }

        QLabel {
            color: #ffffff;
            font-size: 14px;
            font-weight: bold;
        }

        QLineEdit {
            background-color: #242424;
            color: white;
            border: none;
            border-radius: 15px;
            padding: 6px 15px;
            font-size: 13px;
        }
        QLineEdit:focus {
            background-color: #2a2a2a;
            border: 1px solid #727272;
        }

        QPushButton {
            background-color: transparent;
            color: #b3b3b3;
            text-align: left;
            padding: 10px;
            border: none;
            font-weight: bold;
            font-size: 14px;
            border-radius: 4px;
        }
        QPushButton:hover {
            color: #ffffff;
        }

        QPushButton#ActionBtn {
            background-color: #ffffff;
            color: #000000;
            border-radius: 18px;
            padding: 8px 16px;
            text-align: center;
        }
        QPushButton#ActionBtn:hover {
            background-color: #f0f0f0;
        }

        QStackedWidget {
            background-color: #121212;
            border-radius: 8px;
        }

        QListWidget {
            background-color: transparent;
            color: #ffffff;
            border: none;
            outline: 0;
            font-size: 14px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: 4px;
            margin-bottom: 2px;
        }
        QListWidget::item:hover {
            background-color: #2a2a2a;
        }
        QListWidget::item:selected {
            background-color: #3e3e3e;
            color: #1DB954;
        }

        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 12px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: rgba(255, 255, 255, 0.3);
            min-height: 30px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255, 255, 255, 0.5);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }

        QSlider::groove:horizontal {
            border-radius: 2px;
            height: 4px;
            background: #4d4d4d;
        }
        QSlider::sub-page:horizontal {
            background: #ffffff;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #ffffff;
            width: 12px;
            height: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::handle:horizontal:hover {
            background: #1DB954;
        }
    )";

    this->setStyleSheet(spotifyTheme);

    this->setCentralWidget(central);
    this->setMinimumSize(950, 650);

    this->apiManager = new ApiManager(this);
    this->imageManager = new QNetworkAccessManager(this);
    this->player = new QMediaPlayer(this);
    this->audioOutput = new QAudioOutput(this);
    this->player->setAudioOutput(this->audioOutput);
    this->audioOutput->setVolume(0.5f);

    connect(this->btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(this->btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(this->apiManager, &ApiManager::userLoggedIn, this, &MainWindow::onUserLoggedIn);
    connect(this->apiManager, &ApiManager::searchResultsReady, this, &MainWindow::updateSearchList);

    connect(btnHome, &QPushButton::clicked, [this]()
            {
                this->stackedWidget->setCurrentIndex(0);
                if(this->currentUserId > 0)
                {
                    this->apiManager->fetchRecommendations(this->currentUserId);
                }
            });

    connect(btnArtists, &QPushButton::clicked, [this]()
            {
                this->stackedWidget->setCurrentIndex(1);
                if(this->currentUserId > 0)
                {
                    this->apiManager->fetchArtists(this->currentUserId);
                }
            });

    connect(btnLibrary, &QPushButton::clicked, [this]()
            {
                this->stackedWidget->setCurrentIndex(2);
                if(this->currentUserId > 0)
                {
                    this->apiManager->fetchLikedSongs(this->currentUserId);
                }
            });

    connect(this->btnFollowing, &QPushButton::clicked, [this]()
            {
                this->stackedWidget->setCurrentIndex(3);
                if(this->currentUserId > 0)
                {
                    this->apiManager->fetchFollowedArtists(this->currentUserId);
                }
            });

    // Profile & Now Playing Connections
    connect(this->btnBackFromProfile, &QPushButton::clicked, [this]()
            {
                this->stackedWidget->setCurrentIndex(this->previousPageIndex);
            });

    connect(this->btnBackFromNowPlaying, &QPushButton::clicked, [this]()
            {
                this->stackedWidget->setCurrentIndex(this->previousPageIndex);
            });

    connect(this->artistList, &QListWidget::itemClicked, this, &MainWindow::onArtistClicked);
    connect(this->followedArtistList, &QListWidget::itemClicked, this, &MainWindow::onArtistClicked);
    connect(this->apiManager, &ApiManager::artistTracksReady, this, &MainWindow::updateArtistTracksList);

    connect(this->btnLikeRecs, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(this->btnLikeSearch, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(this->btnRemoveLike, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(this->btnFollow, &QPushButton::clicked, this, &MainWindow::onFollowClicked);
    connect(this->btnUnfollow, &QPushButton::clicked, this, &MainWindow::onUnfollowClicked);

    connect(this->apiManager, &ApiManager::recsReady, this, &MainWindow::updateRecsList);
    connect(this->apiManager, &ApiManager::likesReady, this, &MainWindow::updateLikesList);
    connect(this->apiManager, &ApiManager::artistsReady, this, &MainWindow::updateArtistList);
    connect(this->apiManager, &ApiManager::followedArtistsReady, this, &MainWindow::updateFollowedArtistList);

    connect(this->recList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);
    connect(this->likesFolderList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);
    connect(this->searchList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);
    connect(this->artistProfileTrackList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);

    connect(this->lyricsListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
            {
                if (item)
                {
                    QVariant timeData = item->data(Qt::UserRole);
                    if (timeData.isValid())
                    {
                        qint64 ms = timeData.toLongLong();
                        this->player->setPosition(ms);
                    }
                }
            });

    // Global and Separated Control Connections
    connect(this->btnPlayPause, &QPushButton::clicked, this, &MainWindow::togglePlayPause);
    connect(this->btnNowPlayingPlayPause, &QPushButton::clicked, this, &MainWindow::togglePlayPause);

    connect(this->btnNext, &QPushButton::clicked, this, &MainWindow::playNext);
    connect(this->btnNowPlayingNext, &QPushButton::clicked, this, &MainWindow::playNext);

    connect(this->btnPrev, &QPushButton::clicked, this, &MainWindow::playPrevious);
    connect(this->btnNowPlayingPrev, &QPushButton::clicked, this, &MainWindow::playPrevious);

    connect(this->btnShuffle, &QPushButton::clicked, this, &MainWindow::toggleShuffle);
    connect(this->btnNowPlayingShuffle, &QPushButton::clicked, this, &MainWindow::toggleShuffle);

    connect(this->btnRepeat, &QPushButton::clicked, this, &MainWindow::toggleRepeat);
    connect(this->btnNowPlayingRepeat, &QPushButton::clicked, this, &MainWindow::toggleRepeat);

    connect(this->btnLikePlaying, &QPushButton::clicked, this, &MainWindow::toggleLikePlayingTrack);
    connect(this->btnNowPlayingLike, &QPushButton::clicked, this, &MainWindow::toggleLikePlayingTrack);

    connect(this->volumeSlider, &QSlider::valueChanged, this, &MainWindow::changeVolume);

    connect(this->player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onPlayerStateChanged);
    connect(this->player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    connect(this->player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(this->player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(this->progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(this->nowPlayingSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);

    // Hide sidebar and top bar when entering full window mode!
    connect(this->stackedWidget, &QStackedWidget::currentChanged, this, [this, topBar, sidebar](int index)
            {
                if (index == 6)
                {
                    this->playerBarWidget->hide();
                    topBar->hide();
                    sidebar->hide();
                }
                else
                {
                    this->playerBarWidget->show();
                    topBar->show();
                    sidebar->show();
                }
            });
}

// User & Search
void MainWindow::onLoginClicked()
{
    QString username = this->usernameInput->text().trimmed();
    if (username.isEmpty())
    {
        return;
    }
    this->btnLogin->setText("...");
    this->apiManager->loginUser(username);
}

void MainWindow::onUserLoggedIn(int userId, const QString &username)
{
    this->currentUserId = userId;
    this->currentUserLabel->setText("Logged in as: " + username);
    this->currentUserLabel->setStyleSheet("color: #1DB954; font-weight: bold;");
    this->btnLogin->setText("Sign In");
    this->usernameInput->clear();

    // FETCH LIKED SONGS IMMEDIATELY TO WARM UP THE CACHE
    this->apiManager->fetchLikedSongs(this->currentUserId);

    if (this->stackedWidget->currentIndex() == 0)
    {
        this->apiManager->fetchRecommendations(this->currentUserId);
    }
    else if (this->stackedWidget->currentIndex() == 1)
    {
        this->apiManager->fetchArtists(this->currentUserId);
    }
    else if (this->stackedWidget->currentIndex() == 2)
    {
        this->apiManager->fetchLikedSongs(this->currentUserId);
    }
    else if (this->stackedWidget->currentIndex() == 3)
    {
        this->apiManager->fetchFollowedArtists(this->currentUserId);
    }
}

void MainWindow::onSearchClicked()
{
    QString query = this->searchInput->text().trimmed();
    if (query.isEmpty())
    {
        return;
    }
    this->stackedWidget->setCurrentIndex(4);
    this->apiManager->search(query);
}

// Custom Row Builder
void MainWindow::addTrackItem(QListWidget *list, const QJsonObject &obj)
{
    QListWidgetItem *item = new QListWidgetItem(list);
    item->setSizeHint(QSize(100, 64));

    int trackId = obj["id"].toInt();
    item->setData(Qt::UserRole, obj["file_url"].toString());
    item->setData(Qt::UserRole + 1, trackId);
    item->setData(Qt::UserRole + 2, obj["cover_url"].toString());
    item->setData(Qt::UserRole + 3, obj["title"].toString());
    item->setData(Qt::UserRole + 4, obj["artist"].toString());
    item->setData(Qt::UserRole + 5, obj["lyrics"].toString());

    QWidget *rowWidget = new QWidget();
    rowWidget->setStyleSheet("background-color: transparent;");

    QHBoxLayout *layout = new QHBoxLayout(rowWidget);
    layout->setContentsMargins(5, 4, 15, 4);

    QLabel *coverLabel = new QLabel();
    coverLabel->setFixedSize(56, 56);
    coverLabel->setStyleSheet("background-color: #282828; border-radius: 4px;");
    loadSquareImage(obj["cover_url"].toString(), coverLabel);

    // READ FROM CACHE TO DETERMINE LIKE STATUS
    bool isLiked = (list == this->likesFolderList) || this->likedTrackIds.contains(trackId);
    if (isLiked) {
        this->likedTrackIds.insert(trackId); // Ensure it's tracked locally
    }

    QString titleText = obj["title"].toString() + " - " + obj["artist"].toString();
    if (isLiked) {
        titleText += " ♥";
    }

    QLabel *textLabel = new QLabel(titleText);
    if (isLiked) {
        textLabel->setStyleSheet("color: #1DB954; font-size: 14px; font-weight: bold;");
    } else {
        textLabel->setStyleSheet("color: white; font-size: 14px;");
    }

    QLabel *durationLabel = new QLabel(formatSongDuration(obj["duration"].toInt()));
    durationLabel->setStyleSheet("color: #b3b3b3; font-size: 13px; font-weight: bold;");

    layout->addWidget(coverLabel);
    layout->addSpacing(10);
    layout->addWidget(textLabel);
    layout->addStretch();
    layout->addWidget(durationLabel);

    list->setItemWidget(item, rowWidget);
}

// Artist Profile UI Actions
void MainWindow::onArtistClicked(QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    this->previousPageIndex = this->stackedWidget->currentIndex();
    this->artistProfileTrackList->clear();

    int artistId = item->data(Qt::UserRole + 1).toInt();
    QString artistNameText = item->text().split("\n").first();
    this->artistProfileNameLabel->setText(artistNameText);

    QPixmap iconPixmap = item->icon().pixmap(100, 100);
    this->artistProfileImageLabel->setPixmap(iconPixmap);

    this->apiManager->fetchArtistTracks(artistId);
    this->stackedWidget->setCurrentIndex(5);
}

// Clean Track List Updates
void MainWindow::updateSearchList(const QJsonArray &tracks)
{
    this->searchList->clear();
    int n = tracks.size();
    for (int i = 0; i < n; ++i)
    {
        addTrackItem(this->searchList, tracks[i].toObject());
    }
}

void MainWindow::updateRecsList(const QJsonArray &tracks)
{
    this->recList->clear();
    int n = tracks.size();
    for (int i = 0; i < n; ++i)
    {
        addTrackItem(this->recList, tracks[i].toObject());
    }
}

void MainWindow::updateLikesList(const QJsonArray &tracks)
{
    this->likesFolderList->clear();
    int n = tracks.size();
    for (int i = 0; i < n; ++i)
    {
        QJsonObject obj = tracks[i].toObject();
        this->likedTrackIds.insert(obj["id"].toInt()); 
        addTrackItem(this->likesFolderList, obj);
    }
}

void MainWindow::updateArtistTracksList(const QJsonArray &tracks)
{
    this->artistProfileTrackList->clear();
    int n = tracks.size();
    for (int i = 0; i < n; ++i)
    {
        addTrackItem(this->artistProfileTrackList, tracks[i].toObject());
    }
}

// Artist Updates
void MainWindow::updateArtistList(const QJsonArray &artists)
{
    this->artistList->clear();
    int n = artists.size();
    for (int i = 0; i < n; ++i)
    {
        QJsonObject obj = artists[i].toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["name"].toString() + "\n" + obj["genre"].toString(), this->artistList);
        item->setTextAlignment(Qt::AlignCenter);
        item->setSizeHint(QSize(150, 220));
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        loadCircularImage(obj["image_url"].toString(), item);
    }
}

void MainWindow::updateFollowedArtistList(const QJsonArray &artists)
{
    this->followedArtistList->clear();
    int n = artists.size();
    for (int i = 0; i < n; ++i)
    {
        QJsonObject obj = artists[i].toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["name"].toString() + "\n" + obj["genre"].toString(), this->followedArtistList);
        item->setTextAlignment(Qt::AlignCenter);
        item->setSizeHint(QSize(150, 220));
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        loadCircularImage(obj["image_url"].toString(), item);
    }
}

// Image Loaders
void MainWindow::loadCircularImage(const QString &urlString, QListWidgetItem *item)
{
    if (urlString.isEmpty())
    {
        return;
    }
    QUrl url("http://localhost:8000/" + urlString);
    QNetworkRequest request(url);
    QNetworkReply *reply = this->imageManager->get(request);

    connect(reply, &QNetworkReply::finished, [reply, item]()
            {
                if (reply->error() == QNetworkReply::NoError)
                {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull())
                    {
                        int size = 150;
                        QPixmap scaled = pixmap.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                        QPixmap circular(size, size);
                        circular.fill(Qt::transparent);
                        QPainter painter(&circular);
                        painter.setRenderHint(QPainter::Antialiasing);
                        QPainterPath path;
                        path.addEllipse(0, 0, size, size);
                        painter.setClipPath(path);
                        painter.drawPixmap(0, 0, scaled);
                        painter.end();
                        item->setIcon(QIcon(circular));
                    }
                }
                reply->deleteLater();
            });
}

void MainWindow::loadSquareImage(const QString &urlString, QLabel *label)
{
    QPixmap placeholder(56, 56);
    placeholder.fill(QColor(40, 40, 40));
    QPainter painter(&placeholder);
    painter.setPen(QColor("#b3b3b3"));
    QFont font = painter.font();
    font.setPointSize(24);
    painter.setFont(font);
    painter.drawText(placeholder.rect(), Qt::AlignCenter, "🎵");
    painter.end();
    label->setPixmap(placeholder);

    if (urlString.isEmpty())
    {
        return;
    }
    QUrl url("http://localhost:8000/" + urlString);
    QNetworkRequest request(url);
    QNetworkReply *reply = this->imageManager->get(request);

    connect(reply, &QNetworkReply::finished, [reply, label, placeholder]()
            {
                if (reply->error() == QNetworkReply::NoError)
                {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull())
                    {
                        QPixmap scaled = pixmap.scaled(56, 56, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                        label->setPixmap(scaled);
                    }
                }
                reply->deleteLater();
            });
}

void MainWindow::loadSquareIcon(const QString &urlString, QListWidgetItem *item)
{
    if (urlString.isEmpty())
    {
        return;
    }
    QPixmap placeholder(56, 56);
    placeholder.fill(QColor(40, 40, 40));
    item->setIcon(QIcon(placeholder));
    QUrl url("http://localhost:8000/" + urlString);
    QNetworkRequest request(url);
    QNetworkReply *reply = this->imageManager->get(request);

    connect(reply, &QNetworkReply::finished, [reply, item]()
            {
                if (reply->error() == QNetworkReply::NoError)
                {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull())
                    {
                        QPixmap scaled = pixmap.scaled(56, 56, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                        item->setIcon(QIcon(scaled));
                    }
                }
                reply->deleteLater();
            });
}

// Audio Controls
QListWidget* MainWindow::getActiveTrackList()
{
    int idx = this->stackedWidget->currentIndex();
    if (idx == 2)
    {
        return this->likesFolderList;
    }
    if (idx == 4)
    {
        return this->searchList;
    }
    if (idx == 5)
    {
        return this->artistProfileTrackList;
    }
    return this->recList;
}

void MainWindow::playTrack(QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    this->currentPlayingList = item->listWidget();

    this->currentTrackId = item->data(Qt::UserRole + 1).toInt();
    this->currentCoverUrl = item->data(Qt::UserRole + 2).toString();
    this->currentTitleLabel->setText(item->data(Qt::UserRole + 3).toString());
    this->currentArtistLabel->setText(item->data(Qt::UserRole + 4).toString());
    this->currentLyrics = item->data(Qt::UserRole + 5).toString();

    loadSquareImage(this->currentCoverUrl, this->currentCoverLabel);
    QString fileUrl = "http://localhost:8000/" + item->data(Qt::UserRole).toString();
    this->player->setSource(QUrl(fileUrl));
    this->player->play();

    // 2. Keep the green hearts perfectly synced in both places
    bool isLiked = this->likedTrackIds.contains(this->currentTrackId);
    auto setLikeBtnState = [](QPushButton* btn, bool liked) {
        if (liked) {
            btn->setText("♥");
            btn->setStyleSheet("background: transparent; color: #1DB954; font-size: 28px; border: none; padding: 0px;");
        } else {
            btn->setText("♡");
            btn->setStyleSheet("background: transparent; color: #b3b3b3; font-size: 28px; border: none; padding: 0px;");
        }
    };
    setLikeBtnState(this->btnLikePlaying, isLiked);
    setLikeBtnState(this->btnNowPlayingLike, isLiked);

    // 3. UI REFRESH FIX
    if (this->stackedWidget->currentIndex() == 6)
    {
        openNowPlayingWindow();
    }
}

void MainWindow::togglePlayPause()
{
    if (this->player->playbackState() == QMediaPlayer::PlayingState)
    {
        this->player->pause();
    }
    else
    {
        this->player->play();
    }
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState)
    {
        this->btnPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/pause.png"));
        this->btnPlayPause->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 20px; } "
                                          "QPushButton:hover { background-color: rgba(255,255,255,0.1); transform: scale(1.05); }");

        this->btnNowPlayingPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/pause.png"));
        this->btnNowPlayingPlayPause->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 25px; } "
                                                    "QPushButton:hover { background-color: rgba(255,255,255,0.1); transform: scale(1.05); }");
    }
    else
    {
        this->btnPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/play.png"));
        this->btnPlayPause->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 20px; } "
                                          "QPushButton:hover { background-color: rgba(255,255,255,0.1); transform: scale(1.05); }");

        this->btnNowPlayingPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/play.png"));
        this->btnNowPlayingPlayPause->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 25px; } "
                                                    "QPushButton:hover { background-color: rgba(255,255,255,0.1); transform: scale(1.05); }");
    }
}

void MainWindow::toggleShuffle()
{
    this->isShuffle = !this->isShuffle;

    QString activeStyle = "QPushButton { background-color: rgba(29, 185, 84, 0.2); border: 1px solid #1DB954; padding: 4px; border-radius: 5px; }";
    QString inactiveStyle = "QPushButton { background-color: transparent; border: none; padding: 5px; border-radius: 5px; } QPushButton:hover { background-color: rgba(255,255,255,0.1); }";

    if (this->isShuffle)
    {
        this->btnShuffle->setStyleSheet(activeStyle);
        this->btnNowPlayingShuffle->setStyleSheet(activeStyle);
    }
    else
    {
        this->btnShuffle->setStyleSheet(inactiveStyle);
        this->btnNowPlayingShuffle->setStyleSheet(inactiveStyle);
    }
}

void MainWindow::toggleRepeat()
{
    this->repeatState = (this->repeatState + 1) % 4;

    QString activeStyle = "QPushButton { background-color: rgba(29, 185, 84, 0.2); border: 1px solid #1DB954; padding: 4px; border-radius: 5px; }";
    QString inactiveStyle = "QPushButton { background-color: transparent; border: none; padding: 5px; border-radius: 5px; } QPushButton:hover { background-color: rgba(255,255,255,0.1); }";
    QString activeSequenceStyle = "QPushButton { background-color: rgba(29, 185, 84, 0.2); border: 1px solid #1DB954; padding: 4px; border-radius: 5px; color: #1DB954; font-weight: bold; }";

    if (this->repeatState == 0)
    {
        this->btnRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));
        this->btnRepeat->setStyleSheet(inactiveStyle);
        this->btnRepeat->setText("");

        this->btnNowPlayingRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));
        this->btnNowPlayingRepeat->setStyleSheet(inactiveStyle);
        this->btnNowPlayingRepeat->setText("");

        this->customRepeatStartRow = -1;
    }
    else if (this->repeatState == 1)
    {
        this->btnRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));
        this->btnRepeat->setStyleSheet(activeStyle);

        this->btnNowPlayingRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));
        this->btnNowPlayingRepeat->setStyleSheet(activeStyle);
    }
    else if (this->repeatState == 2)
    {
        this->btnRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat_one.png"));
        this->btnRepeat->setStyleSheet(activeStyle);

        this->btnNowPlayingRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat_one.png"));
        this->btnNowPlayingRepeat->setStyleSheet(activeStyle);
    }
    else if (this->repeatState == 3)
    {
        bool ok;
        int n = QInputDialog::getInt(this, "Repeat Sequence",
                                     "How many songs to loop from the current track?", 2, 2, 100, 1, &ok);
        if (ok)
        {
            this->customRepeatN = n;
            QListWidget *list = this->currentPlayingList;
            this->customRepeatStartRow = list ? list->currentRow() : 0;
            if (this->customRepeatStartRow == -1)
            {
                this->customRepeatStartRow = 0;
            }
            this->btnRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));
            this->btnRepeat->setStyleSheet(activeSequenceStyle);
            this->btnRepeat->setText(" " + QString::number(n));

            this->btnNowPlayingRepeat->setIcon(QIcon("/home/hnin/SpotifyClone/icons/repeat.png"));
            this->btnNowPlayingRepeat->setStyleSheet(activeSequenceStyle);
            this->btnNowPlayingRepeat->setText(" " + QString::number(n));
        }
        else
        {
            this->repeatState = -1;
            toggleRepeat();
            return;
        }
    }
}

void MainWindow::playNext()
{
    // Fix: Use the remembered list, not the active screen!
    QListWidget *list = this->currentPlayingList;

    if (!list || list->count() == 0)
    {
        return;
    }

    int currentRow = list->currentRow();
    int nextRow;

    if (this->repeatState == 3 && this->customRepeatStartRow != -1)
    {
        nextRow = currentRow + 1;
        if (nextRow >= this->customRepeatStartRow + this->customRepeatN || nextRow >= list->count())
        {
            nextRow = this->customRepeatStartRow;
        }
    }
    else if (this->isShuffle)
    {
        nextRow = QRandomGenerator::global()->bounded(list->count());
    }
    else
    {
        nextRow = (currentRow + 1) % list->count();
    }

    list->setCurrentRow(nextRow);
    playTrack(list->item(nextRow));
}

void MainWindow::playPrevious()
{
    // Fix: Use the remembered list, not the active screen!
    QListWidget *list = this->currentPlayingList;

    if (!list || list->count() == 0)
    {
        return;
    }

    int currentRow = list->currentRow();
    int prevRow;

    if (this->repeatState == 3 && this->customRepeatStartRow != -1)
    {
        prevRow = currentRow - 1;
        if (prevRow < this->customRepeatStartRow)
        {
            prevRow = qMin(this->customRepeatStartRow + this->customRepeatN - 1, list->count() - 1);
        }
    }
    else
    {
        prevRow = (currentRow - 1 >= 0) ? (currentRow - 1) : (list->count() - 1);
    }

    list->setCurrentRow(prevRow);
    playTrack(list->item(prevRow));
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        // Fix: Use the remembered list!
        QListWidget *list = this->currentPlayingList;

        if (!list)
        {
            return;
        }

        if (this->repeatState == 2)
        {
            this->player->setPosition(0);
            this->player->play();
        }
        else
        {
            if (list->currentRow() == list->count() - 1 && this->repeatState == 0 && !this->isShuffle)
            {
                this->player->stop();
                this->btnPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/play.png"));
                this->btnNowPlayingPlayPause->setIcon(QIcon("/home/hnin/SpotifyClone/icons/play.png"));
            }
            else
            {
                playNext();
            }
        }
    }
}

void MainWindow::changeVolume(int value)
{
    this->audioOutput->setVolume(value / 100.0f);
}

void MainWindow::updatePosition(qint64 position)
{
    if (!this->progressSlider->isSliderDown())
    {
        this->progressSlider->setValue(position);
    }
    if (!this->nowPlayingSlider->isSliderDown())
    {
        this->nowPlayingSlider->setValue(position);
    }

    qint64 duration = this->player->duration();
    int cM = (position / 60000) % 60, cS = (position / 1000) % 60;
    int tM = (duration / 60000) % 60, tS = (duration / 1000) % 60;

    QString newTimeStr = QString("%1:%2 / %3:%4").arg(cM, 2, 10, QChar('0')).arg(cS, 2, 10, QChar('0')).arg(tM, 2, 10, QChar('0')).arg(tS, 2, 10, QChar('0'));
    this->timeLabel->setText(newTimeStr);
    this->nowPlayingTimeLabel->setText(newTimeStr);

    if (this->stackedWidget->currentIndex() == 6)
    {
        int newIndex = -1;
        int count = this->lyricsListWidget->count();

        for (int i = 0; i < count; ++i)
        {
            qint64 itemTime = this->lyricsListWidget->item(i)->data(Qt::UserRole).toLongLong();
            if (position >= itemTime)
            {
                newIndex = i;
            }
            else
            {
                break;
            }
        }

        if (newIndex != this->currentLyricIndex && newIndex != -1)
        {
            if (this->currentLyricIndex != -1 && this->currentLyricIndex < count)
            {
                QListWidgetItem *oldItem = this->lyricsListWidget->item(this->currentLyricIndex);
                if (oldItem)
                {
                    oldItem->setForeground(QColor(255, 255, 255, 100));
                    QFont f = oldItem->font();
                    f.setPointSize(24);
                    oldItem->setFont(f);
                }
            }

            QListWidgetItem *newItem = this->lyricsListWidget->item(newIndex);
            if (newItem)
            {
                newItem->setForeground(QColor(255, 255, 255, 255));
                QFont f = newItem->font();
                f.setPointSize(36);
                newItem->setFont(f);

                this->lyricsListWidget->scrollToItem(newItem, QAbstractItemView::PositionAtCenter);
            }
            this->currentLyricIndex = newIndex;
        }
    }
}

void MainWindow::updateDuration(qint64 duration)
{
    this->progressSlider->setMaximum(duration);
    this->nowPlayingSlider->setMaximum(duration);
}

void MainWindow::setPosition(int position)
{
    this->player->setPosition(position);
}

void MainWindow::toggleLikePlayingTrack()
{
    if (this->currentUserId == 0 || this->currentTrackId == 0)
    {
        return;
    }

    this->apiManager->sendLike(this->currentUserId, this->currentTrackId);

    bool isCurrentlyLiked = this->likedTrackIds.contains(this->currentTrackId);
    bool newState = !isCurrentlyLiked;

    if (newState) {
        this->likedTrackIds.insert(this->currentTrackId);
    } else {
        this->likedTrackIds.remove(this->currentTrackId);
    }

    auto setLikeBtnState = [](QPushButton* btn, bool liked) {
        if (liked) {
            btn->setText("♥");
            btn->setStyleSheet("background: transparent; color: #1DB954; font-size: 28px; border: none; padding: 0px;");
        } else {
            btn->setText("♡");
            btn->setStyleSheet("background: transparent; color: #b3b3b3; font-size: 28px; border: none; padding: 0px;");
        }
    };

    setLikeBtnState(this->btnLikePlaying, newState);
    setLikeBtnState(this->btnNowPlayingLike, newState);
}

void MainWindow::onLikeClicked()
{
    if (this->currentUserId == 0)
    {
        return;
    }
    QListWidget *activeList = getActiveTrackList();
    QListWidgetItem *item = activeList->currentItem();
    if (!item)
    {
        return;
    }

    int trackId = item->data(Qt::UserRole + 1).toInt();
    this->apiManager->sendLike(this->currentUserId, trackId);

    if (this->stackedWidget->currentIndex() == 2)
    {
        this->likedTrackIds.remove(trackId);
        delete item;
    }
    else
    {
        QWidget *rowWidget = activeList->itemWidget(item);
        if (rowWidget)
        {
            QLabel *textLabel = rowWidget->findChild<QLabel*>();
            if (textLabel)
            {
                if (textLabel->text().contains("♥"))
                {
                    this->likedTrackIds.remove(trackId);
                    textLabel->setText(textLabel->text().remove(" ♥"));
                    textLabel->setStyleSheet("color: white; font-size: 14px;");
                }
                else
                {
                    this->likedTrackIds.insert(trackId);
                    textLabel->setText(textLabel->text() + " ♥");
                    textLabel->setStyleSheet("color: #1DB954; font-size: 14px; font-weight: bold;");
                }
            }
        }
    }

    if (this->currentTrackId == trackId) {
        bool isCurrentlyLiked = this->likedTrackIds.contains(trackId);
        auto setLikeBtnState = [](QPushButton* btn, bool liked) {
            if (liked) {
                btn->setText("♥");
                btn->setStyleSheet("background: transparent; color: #1DB954; font-size: 28px; border: none; padding: 0px;");
            } else {
                btn->setText("♡");
                btn->setStyleSheet("background: transparent; color: #b3b3b3; font-size: 28px; border: none; padding: 0px;");
            }
        };
        setLikeBtnState(this->btnLikePlaying, isCurrentlyLiked);
        setLikeBtnState(this->btnNowPlayingLike, isCurrentlyLiked);
    }
}

void MainWindow::onFollowClicked()
{
    if (this->currentUserId == 0)
    {
        return;
    }
    QListWidgetItem *item = this->artistList->currentItem();
    if (!item)
    {
        return;
    }
    this->apiManager->sendFollow(this->currentUserId, item->data(Qt::UserRole + 1).toInt());
    delete item;
}

void MainWindow::onUnfollowClicked()
{
    if (this->currentUserId == 0)
    {
        return;
    }
    QListWidgetItem *item = this->followedArtistList->currentItem();
    if (!item)
    {
        return;
    }
    this->apiManager->sendFollow(this->currentUserId, item->data(Qt::UserRole + 1).toInt());
    delete item;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched->objectName() == "PlayerBarContainer" && event->type() == QEvent::MouseButtonRelease)
    {
        openNowPlayingWindow();
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::openNowPlayingWindow()
{
    if (this->currentTitleLabel->text() == "No track playing")
    {
        return;
    }

    if (this->stackedWidget->currentIndex() != 6)
    {
        this->previousPageIndex = this->stackedWidget->currentIndex();
    }

    this->stackedWidget->setCurrentIndex(6);

    this->nowPlayingTitleLabel->setText(this->currentTitleLabel->text());
    this->nowPlayingArtistLabel->setText(this->currentArtistLabel->text());

    this->nowPlayingSlider->setRange(0, this->player->duration());
    this->nowPlayingSlider->setValue(this->player->position());

    this->lyricsListWidget->clear();
    this->currentLyricIndex = -1;

    QString safeLyrics = this->currentLyrics;
    safeLyrics.replace("\\n", "\n");
    safeLyrics.replace("\r", "");

    QStringList lines = safeLyrics.split("\n", Qt::SkipEmptyParts);

    int lineCount = lines.size();
    for (int i = 0; i < lineCount; ++i)
    {
        QString line = lines[i].trimmed();
        int bracketClose = line.indexOf(']');
        if (line.startsWith('[') && bracketClose > 0)
        {
            QString timeStr = line.mid(1, bracketClose - 1);
            QString text = line.mid(bracketClose + 1).trimmed();

            if (text.isEmpty())
            {
                text = " ";
            }

            QStringList timeParts = timeStr.split(':');
            if (timeParts.size() == 2)
            {
                int min = timeParts[0].toInt();

                QStringList secParts = timeParts[1].split('.');
                int sec = secParts[0].toInt();

                int msec = 0;
                if (secParts.size() > 1)
                {
                    msec = secParts[1].leftJustified(3, '0').toInt();
                }

                qint64 ms = (min * 60000) + (sec * 1000) + msec;

                QListWidgetItem *lItem = new QListWidgetItem(text, this->lyricsListWidget);
                lItem->setTextAlignment(Qt::AlignCenter);
                lItem->setData(Qt::UserRole, ms);

                lItem->setForeground(QColor(255, 255, 255, 100));
                QFont f = lItem->font();
                f.setPointSize(24);
                f.setBold(true);
                lItem->setFont(f);
            }
        }
    }

    if (this->lyricsListWidget->count() == 0)
    {
        QListWidgetItem *emptyItem = new QListWidgetItem("No lyrics available", this->lyricsListWidget);
        emptyItem->setTextAlignment(Qt::AlignCenter);
        emptyItem->setForeground(QColor(255, 255, 255, 100));
        QFont f = emptyItem->font();
        f.setPointSize(24);
        emptyItem->setFont(f);
    }

    if (!this->currentCoverUrl.isEmpty())
    {
        QUrl url("http://localhost:8000/" + this->currentCoverUrl);
        QNetworkRequest req(url);
        QNetworkReply *reply = this->imageManager->get(req);

        connect(reply, &QNetworkReply::finished, [this, reply]()
                {
                    if (reply->error() == QNetworkReply::NoError)
                    {
                        QPixmap pm;
                        pm.loadFromData(reply->readAll());
                        if (!pm.isNull())
                        {
                            this->nowPlayingCoverLabel->setPixmap(pm.scaled(400, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
                            QImage avg = pm.toImage().scaled(1, 1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                            QColor bgColor = avg.pixelColor(0,0).darker(180);
                            this->nowPlayingPage->setStyleSheet(QString("QWidget#NowPlayingPage { background-color: %1; border-radius: 8px; }").arg(bgColor.name()));
                        }
                    }
                    reply->deleteLater();
                });
    }
}

MainWindow::~MainWindow()
{
}
