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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    currentUserId = 0;
    isShuffle = false;
    repeatState = 0;

    QWidget *central = new QWidget(this);
    QVBoxLayout *appLayout = new QVBoxLayout(central);

    //Top Bar
    QWidget *topBar = new QWidget();
    topBar->setFixedHeight(60);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);

    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");
    usernameInput->setFixedWidth(150);
    usernameInput->setStyleSheet("background-color: white; color: black; border-radius: 3px; padding: 5px;");

    btnLogin = new QPushButton("Sign In");
    btnLogin->setStyleSheet("background-color: #1DB954; color: white; border-radius: 3px; padding: 5px;");

    currentUserLabel = new QLabel("Not logged in");
    currentUserLabel->setStyleSheet("color: #b3b3b3; font-style: italic;");

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search tracks or artists...");
    searchInput->setFixedWidth(200);
    searchInput->setStyleSheet("background-color: white; color: black; border-radius: 3px; padding: 5px;");

    btnSearch = new QPushButton("🔍");
    btnSearch->setStyleSheet("background-color: #282828; color: white; border-radius: 3px; padding: 5px;");

    topLayout->addWidget(new QLabel("👤"));
    topLayout->addWidget(usernameInput);
    topLayout->addWidget(btnLogin);
    topLayout->addWidget(currentUserLabel);
    topLayout->addStretch();
    topLayout->addWidget(searchInput);
    topLayout->addWidget(btnSearch);

    //Main Layout
    QHBoxLayout *mainLayout = new QHBoxLayout();

    //Sidebar
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(180);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    QPushButton *btnHome = new QPushButton("🏠 Home");
    QPushButton *btnArtists = new QPushButton("🎤 Find Artists");
    btnFollowing = new QPushButton("👤 Following");
    QPushButton *btnLibrary = new QPushButton("♥ Liked Songs");

    sideLayout->addWidget(new QLabel("SPOTIFY CLONE"));
    sideLayout->addWidget(btnHome);
    sideLayout->addWidget(btnArtists);
    sideLayout->addWidget(btnFollowing);
    sideLayout->addWidget(btnLibrary);
    sideLayout->addStretch();

    stackedWidget = new QStackedWidget();

    //Home
    QWidget *homePage = new QWidget();
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    recList = new QListWidget();
    btnLikeRecs = new QPushButton("♥ Like / Unlike Selected");
    btnLikeRecs->setStyleSheet("background-color: #1DB954; color: white; padding: 10px; border-radius: 5px;");
    homeLayout->addWidget(new QLabel("Recommended for You"));
    homeLayout->addWidget(recList);
    homeLayout->addWidget(btnLikeRecs);

    //Artists
    QWidget *artistPage = new QWidget();
    QVBoxLayout *artistLayout = new QVBoxLayout(artistPage);
    artistList = new QListWidget();
    artistList->setViewMode(QListView::IconMode);
    artistList->setIconSize(QSize(150, 150));
    artistList->setResizeMode(QListView::Adjust);
    artistList->setSpacing(20);
    artistList->setMovement(QListView::Static);
    artistList->setWordWrap(true);
    btnFollow = new QPushButton("➕ Follow Selected Artist");
    btnFollow->setStyleSheet("background-color: #1DB954; color: white; padding: 10px; border-radius: 5px;");
    artistLayout->addWidget(new QLabel("Artists You Might Like"));
    artistLayout->addWidget(artistList);
    artistLayout->addWidget(btnFollow);

    //Liked Songs
    QWidget *likesPage = new QWidget();
    QVBoxLayout *likesLayout = new QVBoxLayout(likesPage);
    likesFolderList = new QListWidget();
    btnRemoveLike = new QPushButton("❌ Remove Selected from Liked");
    btnRemoveLike->setStyleSheet("background-color: #E22134; color: white; padding: 10px; border-radius: 5px;");
    likesLayout->addWidget(new QLabel("Your Liked Songs"));
    likesLayout->addWidget(likesFolderList);
    likesLayout->addWidget(btnRemoveLike);

    //Following
    QWidget *followingPage = new QWidget();
    QVBoxLayout *followingLayout = new QVBoxLayout(followingPage);
    followedArtistList = new QListWidget();
    followedArtistList->setViewMode(QListView::IconMode);
    followedArtistList->setIconSize(QSize(150, 150));
    followedArtistList->setResizeMode(QListView::Adjust);
    followedArtistList->setSpacing(20);
    followedArtistList->setMovement(QListView::Static);
    followedArtistList->setWordWrap(true);
    btnUnfollow = new QPushButton("❌ Unfollow Selected Artist");
    btnUnfollow->setStyleSheet("background-color: #E22134; color: white; padding: 10px; border-radius: 5px;");
    followingLayout->addWidget(new QLabel("Artists You Follow"));
    followingLayout->addWidget(followedArtistList);
    followingLayout->addWidget(btnUnfollow);

    //Search Results
    QWidget *searchPage = new QWidget();
    QVBoxLayout *searchLayout = new QVBoxLayout(searchPage);
    searchList = new QListWidget();
    btnLikeSearch = new QPushButton("♥ Like / Unlike Selected");
    btnLikeSearch->setStyleSheet("background-color: #1DB954; color: white; padding: 10px; border-radius: 5px;");
    searchLayout->addWidget(new QLabel("Search Results"));
    searchLayout->addWidget(searchList);
    searchLayout->addWidget(btnLikeSearch);

    stackedWidget->addWidget(homePage);
    stackedWidget->addWidget(artistPage);
    stackedWidget->addWidget(likesPage);
    stackedWidget->addWidget(followingPage);
    stackedWidget->addWidget(searchPage);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget);

    //NEW PLAYER BAR REDESIGN
    QWidget *playerBar = new QWidget();
    playerBar->setFixedHeight(90);
    playerBar->setStyleSheet("background-color: #181818; border-top: 1px solid #282828;");
    QHBoxLayout *playerMainLayout = new QHBoxLayout(playerBar);

    //LEFT COLUMN
    QWidget *leftWidget = new QWidget();
    leftWidget->setFixedWidth(250);
    QHBoxLayout *leftLayout = new QHBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    currentCoverLabel = new QLabel();
    currentCoverLabel->setFixedSize(56, 56);
    currentCoverLabel->setStyleSheet("background-color: #282828; border-radius: 5px;");

    QVBoxLayout *infoLayout = new QVBoxLayout();
    currentTitleLabel = new QLabel("No track playing");
    currentTitleLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    currentArtistLabel = new QLabel("-");
    currentArtistLabel->setStyleSheet("color: #b3b3b3; font-size: 12px;");
    infoLayout->addWidget(currentTitleLabel);
    infoLayout->addWidget(currentArtistLabel);
    infoLayout->addStretch();

    leftLayout->addWidget(currentCoverLabel);
    leftLayout->addLayout(infoLayout);

    //CENTER COLUMN
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setAlignment(Qt::AlignCenter);
    controlsLayout->setSpacing(15);

    btnShuffle = new QPushButton();
    btnPrev = new QPushButton();
    btnPlayPause = new QPushButton();
    btnNext = new QPushButton();
    btnRepeat = new QPushButton();
    btnShuffle->setIcon(QIcon(":/SpotifyClone/icons/shuffle.png"));
    btnPrev->setIcon(QIcon(":/SpotifyClone/icons/prev.png"));
    btnPlayPause->setIcon(QIcon(":/SpotifyClone/icons/play.png"));
    btnNext->setIcon(QIcon(":/SpotifyClone/icons/next.png"));
    btnRepeat->setIcon(QIcon(":/SpotifyClone/icons/repeat.png"));

    int iconSize = 24;
    btnShuffle->setIconSize(QSize(iconSize, iconSize));
    btnPrev->setIconSize(QSize(iconSize, iconSize));
    btnPlayPause->setIconSize(QSize(iconSize, iconSize));
    btnNext->setIconSize(QSize(iconSize, iconSize));
    btnRepeat->setIconSize(QSize(iconSize, iconSize));
    QString controlStyle = "QPushButton { background-color: black; border: none; padding: 5px; border-radius: 5px; } "
                           "QPushButton:hover { background-color: #282828; }";

    btnShuffle->setStyleSheet(controlStyle);
    btnPrev->setStyleSheet(controlStyle);
    btnNext->setStyleSheet(controlStyle);
    btnRepeat->setStyleSheet(controlStyle);
    btnPlayPause->setFixedSize(40, 40);
    btnPlayPause->setStyleSheet("QPushButton { background-color: black; border: none; border-radius: 20px; } "
                                "QPushButton:hover { background-color: #282828; }");

    controlsLayout->addWidget(btnShuffle);
    controlsLayout->addWidget(btnPrev);
    controlsLayout->addWidget(btnPlayPause);
    controlsLayout->addWidget(btnNext);
    controlsLayout->addWidget(btnRepeat);

    QHBoxLayout *progressLayout = new QHBoxLayout();
    timeLabel = new QLabel("00:00 / 00:00");
    timeLabel->setStyleSheet("color: #b3b3b3; font-size: 12px;");
    progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setRange(0, 0);
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(timeLabel);

    centerLayout->addLayout(controlsLayout);
    centerLayout->addLayout(progressLayout);

    //RIGHT COLUMN
    QWidget *rightWidget = new QWidget();
    rightWidget->setFixedWidth(200);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *volIcon = new QLabel("🔈");
    volIcon->setStyleSheet("color: #b3b3b3; font-size: 16px;");
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(100);

    rightLayout->addWidget(volIcon);
    rightLayout->addWidget(volumeSlider);

    playerMainLayout->addWidget(leftWidget);
    playerMainLayout->addWidget(centerWidget, 1);
    playerMainLayout->addWidget(rightWidget);

    appLayout->addWidget(topBar);
    appLayout->addLayout(mainLayout);
    appLayout->addWidget(playerBar);
    appLayout->setContentsMargins(0,0,0,0);

    this->setStyleSheet("QMainWindow { background-color: #121212; color: white; }"
                        "QPushButton { background-color: transparent; color: #b3b3b3; text-align: left; padding: 10px; border: none; font-weight: bold; }"
                        "QPushButton:hover { color: white; }"
                        "QListWidget { background-color: #181818; color: white; border: none; border-radius: 10px; padding: 5px; outline: 0; }"
                        "QListWidget::item:selected { background-color: #282828; color: #1DB954; }"
                        "QLabel { color: white; font-size: 16px; font-weight: bold; }");

    setCentralWidget(central);
    resize(950, 650);

    apiManager = new ApiManager(this);
    imageManager = new QNetworkAccessManager(this);
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5f);

    connect(btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(apiManager, &ApiManager::userLoggedIn, this, &MainWindow::onUserLoggedIn);
    connect(apiManager, &ApiManager::searchResultsReady, this, &MainWindow::updateSearchList);

    connect(btnHome, &QPushButton::clicked, [this](){
        stackedWidget->setCurrentIndex(0);
        if(currentUserId > 0) apiManager->fetchRecommendations(currentUserId);
    });
    connect(btnArtists, &QPushButton::clicked, [this](){
        stackedWidget->setCurrentIndex(1);
        if(currentUserId > 0) apiManager->fetchArtists(currentUserId);
    });
    connect(btnLibrary, &QPushButton::clicked, [this](){
        stackedWidget->setCurrentIndex(2);
        if(currentUserId > 0) apiManager->fetchLikedSongs(currentUserId);
    });
    connect(btnFollowing, &QPushButton::clicked, [this](){
        stackedWidget->setCurrentIndex(3);
        if(currentUserId > 0) apiManager->fetchFollowedArtists(currentUserId);
    });

    connect(btnLikeRecs, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(btnLikeSearch, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(btnRemoveLike, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(btnFollow, &QPushButton::clicked, this, &MainWindow::onFollowClicked);
    connect(btnUnfollow, &QPushButton::clicked, this, &MainWindow::onUnfollowClicked);

    connect(apiManager, &ApiManager::recsReady, this, &MainWindow::updateRecsList);
    connect(apiManager, &ApiManager::likesReady, this, &MainWindow::updateLikesList);
    connect(apiManager, &ApiManager::artistsReady, this, &MainWindow::updateArtistList);
    connect(apiManager, &ApiManager::followedArtistsReady, this, &MainWindow::updateFollowedArtistList);

    connect(recList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);
    connect(likesFolderList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);
    connect(searchList, &QListWidget::itemDoubleClicked, this, &MainWindow::playTrack);
    connect(btnPlayPause, &QPushButton::clicked, this, &MainWindow::togglePlayPause);
    connect(btnNext, &QPushButton::clicked, this, &MainWindow::playNext);
    connect(btnPrev, &QPushButton::clicked, this, &MainWindow::playPrevious);
    connect(btnShuffle, &QPushButton::clicked, this, &MainWindow::toggleShuffle);
    connect(btnRepeat, &QPushButton::clicked, this, &MainWindow::toggleRepeat);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::changeVolume);

    connect(player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onPlayerStateChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
}

//User & Search
void MainWindow::onLoginClicked() {
    QString username = usernameInput->text().trimmed();
    if (username.isEmpty()) return;
    btnLogin->setText("...");
    apiManager->loginUser(username);
}

void MainWindow::onUserLoggedIn(int userId, const QString &username) {
    currentUserId = userId;
    currentUserLabel->setText("Logged in as: " + username);
    currentUserLabel->setStyleSheet("color: #1DB954; font-weight: bold;");
    btnLogin->setText("Sign In");
    usernameInput->clear();

    if (stackedWidget->currentIndex() == 0) apiManager->fetchRecommendations(currentUserId);
    else if (stackedWidget->currentIndex() == 1) apiManager->fetchArtists(currentUserId);
    else if (stackedWidget->currentIndex() == 2) apiManager->fetchLikedSongs(currentUserId);
    else if (stackedWidget->currentIndex() == 3) apiManager->fetchFollowedArtists(currentUserId);
}

void MainWindow::onSearchClicked() {
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) return;
    stackedWidget->setCurrentIndex(4);
    apiManager->search(query);
}

//List Updates
void MainWindow::updateSearchList(const QJsonArray &tracks) {
    searchList->clear();
    for (const QJsonValue &v : tracks) {
        QJsonObject obj = v.toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["title"].toString() + " - " + obj["artist"].toString(), searchList);
        item->setData(Qt::UserRole, obj["file_url"].toString());
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        item->setData(Qt::UserRole + 2, obj["cover_url"].toString());
        item->setData(Qt::UserRole + 3, obj["title"].toString());
        item->setData(Qt::UserRole + 4, obj["artist"].toString());
    }
}

void MainWindow::updateRecsList(const QJsonArray &tracks) {
    recList->clear();
    for (const QJsonValue &v : tracks) {
        QJsonObject obj = v.toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["title"].toString() + " - " + obj["artist"].toString(), recList);
        item->setData(Qt::UserRole, obj["file_url"].toString());
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        item->setData(Qt::UserRole + 2, obj["cover_url"].toString());
        item->setData(Qt::UserRole + 3, obj["title"].toString());
        item->setData(Qt::UserRole + 4, obj["artist"].toString());
    }
}

void MainWindow::updateLikesList(const QJsonArray &tracks) {
    likesFolderList->clear();
    for (const QJsonValue &v : tracks) {
        QJsonObject obj = v.toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["title"].toString() + " - " + obj["artist"].toString(), likesFolderList);
        item->setData(Qt::UserRole, obj["file_url"].toString());
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        item->setData(Qt::UserRole + 2, obj["cover_url"].toString());
        item->setData(Qt::UserRole + 3, obj["title"].toString());
        item->setData(Qt::UserRole + 4, obj["artist"].toString());
    }
}

void MainWindow::updateArtistList(const QJsonArray &artists) {
    artistList->clear();
    for (const QJsonValue &v : artists) {
        QJsonObject obj = v.toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["name"].toString() + "\n" + obj["genre"].toString(), artistList);
        item->setTextAlignment(Qt::AlignCenter);
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        loadCircularImage(obj["image_url"].toString(), item);
    }
}

void MainWindow::updateFollowedArtistList(const QJsonArray &artists) {
    followedArtistList->clear();
    for (const QJsonValue &v : artists) {
        QJsonObject obj = v.toObject();
        QListWidgetItem *item = new QListWidgetItem(obj["name"].toString() + "\n" + obj["genre"].toString(), followedArtistList);
        item->setTextAlignment(Qt::AlignCenter);
        item->setData(Qt::UserRole + 1, obj["id"].toInt());
        loadCircularImage(obj["image_url"].toString(), item);
    }
}

//Image Loaders
void MainWindow::loadCircularImage(const QString &urlString, QListWidgetItem *item) {
    if (urlString.isEmpty()) return;
    QUrl url("http://localhost:8000/" + urlString);
    QNetworkRequest request(url);
    QNetworkReply *reply = imageManager->get(request);
    connect(reply, &QNetworkReply::finished, [reply, item]() {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            if (!pixmap.isNull()) {
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

void MainWindow::loadSquareImage(const QString &urlString, QLabel *label) {
    if (urlString.isEmpty()) return;
    QUrl url("http://localhost:8000/" + urlString);
    QNetworkRequest request(url);
    QNetworkReply *reply = imageManager->get(request);
    connect(reply, &QNetworkReply::finished, [reply, label]() {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            if (!pixmap.isNull()) {
                QPixmap scaled = pixmap.scaled(56, 56, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                label->setPixmap(scaled);
            }
        }
        reply->deleteLater();
    });
}

//Audio Controls
QListWidget* MainWindow::getActiveTrackList() {
    int idx = stackedWidget->currentIndex();
    if (idx == 2) return likesFolderList;
    if (idx == 4) return searchList;
    return recList;
}

void MainWindow::playTrack(QListWidgetItem *item) {
    if (!item) return;
    currentTitleLabel->setText(item->data(Qt::UserRole + 3).toString());
    currentArtistLabel->setText(item->data(Qt::UserRole + 4).toString());
    loadSquareImage(item->data(Qt::UserRole + 2).toString(), currentCoverLabel);
    QString fileUrl = "http://localhost:8000/" + item->data(Qt::UserRole).toString();
    player->setSource(QUrl(fileUrl));
    player->play();
}

void MainWindow::togglePlayPause() {
    if (player->playbackState() == QMediaPlayer::PlayingState) player->pause();
    else player->play();
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlayingState) {
        btnPlayPause->setIcon(QIcon(":/SpotifyClone/icons/pause.png"));
        btnPlayPause->setStyleSheet("QPushButton { background-color: black; border: none; border-radius: 20px; } "
                                    "QPushButton:hover { background-color: #282828; }");
    } else {
        btnPlayPause->setIcon(QIcon(":/SpotifyClone/icons/play.png"));
        btnPlayPause->setStyleSheet("QPushButton { background-color: black; border: none; border-radius: 20px; } "
                                    "QPushButton:hover { background-color: #282828; }");
    }
}

void MainWindow::toggleShuffle() {
    isShuffle = !isShuffle;

    if (isShuffle) {
        btnShuffle->setStyleSheet("QPushButton { font-size: 20px; color: white; background-color: black; border: 1px solid white; padding: 4px; border-radius: 5px; }");
    } else {
        btnShuffle->setStyleSheet("QPushButton { font-size: 20px; color: white; background-color: black; border: none; padding: 5px; border-radius: 5px; } "
                                  "QPushButton:hover { background-color: #282828; }");
    }
}

void MainWindow::toggleRepeat() {
    repeatState = (repeatState + 1) % 3;

    if (repeatState == 0) {
        btnRepeat->setIcon(QIcon(":/SpotifyClone/icons/repeat.png"));
        btnRepeat->setStyleSheet("QPushButton { background-color: black; border: none; padding: 5px; border-radius: 5px; } "
                                 "QPushButton:hover { background-color: #282828; }");
    }
    else if (repeatState == 1) {
        btnRepeat->setIcon(QIcon(":/SpotifyClone/icons/repeat.png"));
        btnRepeat->setStyleSheet("QPushButton { background-color: black; border: 1px solid white; padding: 4px; border-radius: 5px; }");
    }
    else if (repeatState == 2) {
        btnRepeat->setIcon(QIcon(":/SpotifyClone/icons/repeat_one.png"));
        btnRepeat->setStyleSheet("QPushButton { background-color: black; border: 1px solid white; padding: 4px; border-radius: 5px; }");
    }
}

void MainWindow::playNext() {
    QListWidget *list = getActiveTrackList();
    if (!list || list->count() == 0) return;

    int currentRow = list->currentRow();
    int nextRow;

    if (isShuffle) {
        nextRow = QRandomGenerator::global()->bounded(list->count());
    } else {
        nextRow = (currentRow + 1) % list->count();
    }

    list->setCurrentRow(nextRow);
    playTrack(list->item(nextRow));
}

void MainWindow::playPrevious() {
    QListWidget *list = getActiveTrackList();
    if (!list || list->count() == 0) return;

    int currentRow = list->currentRow();
    int prevRow = (currentRow - 1 >= 0) ? (currentRow - 1) : (list->count() - 1);

    list->setCurrentRow(prevRow);
    playTrack(list->item(prevRow));
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        QListWidget *list = getActiveTrackList();
        if (!list) return;

        if (repeatState == 2) {
            player->setPosition(0);
            player->play();
        } else if (isShuffle) {
            playNext();
        } else {
            if (list->currentRow() == list->count() - 1) {
                if (repeatState == 1) playNext();
                else {
                    player->stop();
                    btnPlayPause->setText("▶");
                }
            } else {
                playNext();
            }
        }
    }
}

void MainWindow::changeVolume(int value) {
    audioOutput->setVolume(value / 100.0f);
}

void MainWindow::updatePosition(qint64 position) {
    if (!progressSlider->isSliderDown()) progressSlider->setValue(position);
    qint64 duration = player->duration();
    int cM = (position / 60000) % 60, cS = (position / 1000) % 60;
    int tM = (duration / 60000) % 60, tS = (duration / 1000) % 60;
    timeLabel->setText(QString("%1:%2 / %3:%4").arg(cM, 2, 10, QChar('0')).arg(cS, 2, 10, QChar('0')).arg(tM, 2, 10, QChar('0')).arg(tS, 2, 10, QChar('0')));
}

void MainWindow::updateDuration(qint64 duration) {
    progressSlider->setMaximum(duration);
}

void MainWindow::setPosition(int position) {
    player->setPosition(position);
}

//Like / Follow Actions
void MainWindow::onLikeClicked() {
    if (currentUserId == 0) return;
    QListWidget *activeList = getActiveTrackList();
    QListWidgetItem *item = activeList->currentItem();
    if (!item) return;

    int trackId = item->data(Qt::UserRole + 1).toInt();
    apiManager->sendLike(currentUserId, trackId);

    if (stackedWidget->currentIndex() == 2) {
        delete item;
    } else {
        if (item->text().contains("♥")) {
            item->setText(item->text().remove(" ♥"));
            item->setForeground(QBrush(QColor(255, 255, 255)));
        } else {
            item->setText(item->text() + " ♥");
            item->setForeground(QBrush(QColor(29, 185, 84)));
        }
    }
}

void MainWindow::onFollowClicked() {
    if (currentUserId == 0) return;
    QListWidgetItem *item = artistList->currentItem();
    if (!item) return;
    apiManager->sendFollow(currentUserId, item->data(Qt::UserRole + 1).toInt());
    delete item;
}

void MainWindow::onUnfollowClicked() {
    if (currentUserId == 0) return;
    QListWidgetItem *item = followedArtistList->currentItem();
    if (!item) return;
    apiManager->sendFollow(currentUserId, item->data(Qt::UserRole + 1).toInt());
    delete item;
}

MainWindow::~MainWindow() {}
