#include "MiniPlayer.hpp"
#include "src/applicationui.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include <src/PlayerPage/PlayerPage.hpp>
#include "src/utils/UIUtils.hpp"
#include "PlaylistSheet.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/LabelAutoSizeProperties>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/DockLayout>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/UIConfig>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/ImageView>
#include <bb/cascades/ImageButton>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/Color>
#include <bb/cascades/Divider>
#include <bb/cascades/NavigationFocusPolicy>
#include <bb/cascades/TouchEvent>
#include <bb/cascades/TouchType>

#include <bb/cascades/ContainerNavigation>
#include <bb/cascades/NavigationFocusPolicy>
#include <bb/cascades/TrackpadHandler>
#include <bb/cascades/TrackpadEvent>
#include <bb/cascades/TrackpadEventType>
#include <bb/cascades/Shortcut>

#include <bb/multimedia/MediaState>

float MiniPlayer::videoHeight = 135;
float MiniPlayer::videoWidth = 180;

using namespace bb::cascades;
using namespace bb::multimedia;

MiniPlayer::MiniPlayer(NavigationPane *navigationPane) :
        navigationPane(navigationPane)
{
    playerContext = ApplicationUI::playerContext;

    Container *playerContainer = new Container();
    StackLayout *contentLayout = new StackLayout();
    contentLayout->setOrientation(LayoutOrientation::LeftToRight);

    uiConfig = this->ui();
    this->setVisible(playerContext->containsVideo());
    playerContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    playerContainer->setPreferredHeight(videoHeight);
    playerContainer->setLayout(contentLayout);

    Container *goToVideoClickableContainer = new Container();
    ContainerNavigation *containerNavigation = goToVideoClickableContainer->navigation();
    containerNavigation->setFocusPolicy(NavigationFocusPolicy::Focusable);
    Shortcut *enterShortcut = Shortcut::create().key("Enter");
    goToVideoClickableContainer->addShortcut(enterShortcut);
    goToVideoClickableContainer->setLayout(
            StackLayout::create().orientation(LayoutOrientation::LeftToRight));

    Container *videoContainer = new Container();
    videoContainer->setLayout(new DockLayout());
    videoContainer->setMinWidth(videoWidth);
    videoContainer->setMaxWidth(videoWidth);
    videoContainer->setMinHeight(videoHeight);
    videoContainer->setMaxHeight(videoHeight);
    videoContainer->setLeftPadding(uiConfig->du(1));
    videoContainer->setTopPadding(uiConfig->du(1));
    videoContainer->setRightPadding(uiConfig->du(1));
    videoContainer->setBottomPadding(uiConfig->du(1));
    foreignWindowControl = ForeignWindowControl::create().updatedProperties(
            WindowProperty::Size | WindowProperty::Position | WindowProperty::Visible);
    videoContainer->add(foreignWindowControl);
    backgroundImage = ImageView::create();
    backgroundImage->setVerticalAlignment(VerticalAlignment::Center);
    backgroundImage->setHorizontalAlignment(HorizontalAlignment::Center);
    backgroundImage->setImageSource(QString("asset:///images/player_background_mini.png"));
    videoContainer->add(backgroundImage);
    goToVideoClickableContainer->add(videoContainer);

    Container *titleContainer = new Container();
    titleContainer->setLeftMargin(uiConfig->du(1));
    titleContainer->setLeftPadding(uiConfig->du(1));
    titleContainer->setTopPadding(uiConfig->du(1));
    titleContainer->setRightPadding(uiConfig->du(1));
    titleContainer->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));
    title = Label::create().text("");
    title->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    title->setMultiline(true);
    title->autoSize()->setMaxLineCount(2);
    titleContainer->add(title);
    goToVideoClickableContainer->add(titleContainer);

    playerContainer->add(goToVideoClickableContainer);

    Container *playButtonContainer = new Container();
    playButtonContainer->setMinWidth(64);
    playButtonContainer->setMaxWidth(64);
    playButtonContainer->setMinHeight(64);
    playButtonContainer->setMaxHeight(64);
    playButtonContainer->setVerticalAlignment(VerticalAlignment::Center);
    playButton = ImageButton::create();
    playButtonContainer->add(playButton);
    playerContainer->add(playButtonContainer);

    Container *stopButtonContainer = new Container();
    stopButtonContainer->setLeftMargin(uiConfig->du(3));
    stopButtonContainer->setMinWidth(64);
    stopButtonContainer->setMaxWidth(64);
    stopButtonContainer->setMinHeight(64);
    stopButtonContainer->setMaxHeight(64);
    stopButtonContainer->setVerticalAlignment(VerticalAlignment::Center);
    ImageButton *stopButton = ImageButton::create().defaultImage(
            QString("asset:///images/ic_stop.png")).pressedImage(
            QString("asset:///images/ic_stop.png"));
    stopButtonContainer->add(stopButton);
    playerContainer->add(stopButtonContainer);

    playlistButtonContainer = new Container();
    playlistButtonContainer->setLeftMargin(uiConfig->du(3));
    playlistButtonContainer->setMinWidth(64);
    playlistButtonContainer->setMaxWidth(64);
    playlistButtonContainer->setMinHeight(64);
    playlistButtonContainer->setMaxHeight(64);
    playlistButtonContainer->setVerticalAlignment(VerticalAlignment::Center);
    playlistButtonContainer->setVisible(playerContext->getPlaylistId() > 0);
    ImageButton *playlistButton = ImageButton::create().defaultImage(
            QString("asset:///images/ic_view_list.png")).pressedImage(
            QString("asset:///images/ic_view_list.png"));
    playlistButtonContainer->add(playlistButton);
    playerContainer->add(playlistButtonContainer);

    this->add(playerContainer);
    this->add(Divider::create());
    if (playerContext->containsVideo()) {
        setVideo();
    } else {
        needHack = false;
    }

    QObject::connect(foreignWindowControl, SIGNAL(boundToWindowChanged(bool)), this,
            SLOT(onForeignWindowBoundingChanged(bool)));
    QObject::connect(foreignWindowControl, SIGNAL(controlFrameChanged(const QRectF &)), this,
            SLOT(onControlFrameChanged(const QRectF &)));
    QObject::connect(goToVideoClickableContainer, SIGNAL(touch(bb::cascades::TouchEvent *)), this,
            SLOT(onGoToVideoTouch(bb::cascades::TouchEvent *)));
    QObject::connect(playerContext, SIGNAL(metadataChanged()), this, SLOT(onMetadataChanged()));
    QObject::connect(playerContext, SIGNAL(miniPlayerStopped()), this, SLOT(onMiniPlayerStopped()));
    QObject::connect(playerContext, SIGNAL(playlistChanged(int)), this, SLOT(onPlaylistChanged(int)));
    QObject::connect(playerContext, SIGNAL(mediaStateChanged(bb::multimedia::MediaState::Type)),
            this, SLOT(onMediaStateChanged(bb::multimedia::MediaState::Type)));
    QObject::connect(playButton, SIGNAL(clicked()), this, SLOT(onPlayButtonClicked()));
    QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    QObject::connect(playlistButton, SIGNAL(clicked()), this, SLOT(onPlaylistButtonClicked()));
    TrackpadHandler *goToVideoClickableContainerTrackpadHandler =
            new bb::cascades::TrackpadHandler();
    QObject::connect(goToVideoClickableContainerTrackpadHandler,
            SIGNAL(trackpad(bb::cascades::TrackpadEvent*)), this,
            SLOT(onGoToVideoClickableContainerTrackpadEvent(bb::cascades::TrackpadEvent*)));
    goToVideoClickableContainer->addEventHandler(goToVideoClickableContainerTrackpadHandler);
    QObject::connect(enterShortcut, SIGNAL(triggered()), this, SLOT(onEnterKey()));

    if (playerContext->containsVideo()) {
        onMetadataChanged();
        onMediaStateChanged(playerContext->getMediaState());
    }
}

void MiniPlayer::onForeignWindowBoundingChanged(bool isBound)
{
    foreignWindowControl->setVisible(isBound);
}

void MiniPlayer::onControlFrameChanged(const QRectF & rect)
{
    if (rect.width() == 0 || rect.height() == 0 || !playerContext
            || !playerContext->getWindowHandle()) {
        return;
    }

    setPositionAndSize(playerContext->getWindowHandle(), rect.x(), rect.y(), rect.width(),
            rect.height());

    if (needHack) {
        needHack = false;
        playerContext->resizeVideo(videoWidth, videoHeight);
    }
}

void MiniPlayer::setPositionAndSize(screen_window_t windowHandle, float x, float y, float width,
        float height)
{
    if (!windowHandle) {
        return;
    }

    int positionParam[2];
    positionParam[0] = x;
    positionParam[1] = y;

    int result = -1;
    result = screen_set_window_property_iv(windowHandle, SCREEN_PROPERTY_POSITION, positionParam);

    if (result != 0) {
        fprintf(stderr, "ERROR : screen_set_window_property_iv : SCREEN_PROPERTY_POSITION = %d \n",
                result);
    }

    int sizeParam[2];
    sizeParam[0] = width;
    sizeParam[1] = height;

    result = -1;
    result = screen_set_window_property_iv(windowHandle, SCREEN_PROPERTY_SIZE, sizeParam);

    if (result != 0) {
        fprintf(stderr, "ERROR : screen_set_window_property_iv : SCREEN_PROPERTY_SIZE = %d \n",
                result);
    }
}

void MiniPlayer::setVideo()
{
    if (!playerContext->containsVideo()) {
        return;
    }

    playerContext->setWindowToForeignWindowControl(foreignWindowControl);
    playerContext->resetScalingMethod();
    needHack = true;
    playerContext->resizeVideo(videoWidth + 10, videoHeight);
    this->setVisible(true);
    playerContext->setNextEnabled(false);
    playerContext->setPrevEnabled(false);
    onMediaStateChanged(playerContext->getMediaState());
}

void MiniPlayer::onMetadataChanged()
{
    title->setText(playerContext->getVideoMetadata().video.title);
    if (playerContext->isAudioOnly()) {
        backgroundImage->setVisible(true);
        foreignWindowControl->setVisible(false);
    } else {
        backgroundImage->setVisible(false);
        foreignWindowControl->setVisible(true);
    }
}

void MiniPlayer::onMediaStateChanged(bb::multimedia::MediaState::Type state)
{
    switch (state) {
        case MediaState::Paused:
        case MediaState::Stopped:
            playButton->setDefaultImageSource(QString("asset:///images/ic_play.png"));
            playButton->setPressedImageSource(QString("asset:///images/ic_play.png"));
            break;
        case bb::multimedia::MediaState::Started:
            playButton->setDefaultImageSource(QString("asset:///images/ic_pause.png"));
            playButton->setPressedImageSource(QString("asset:///images/ic_pause.png"));
            break;
    }
}

void MiniPlayer::onGoToVideoTouch(bb::cascades::TouchEvent *e)
{
    if (e->touchType() != bb::cascades::TouchType::Up) {
        return;
    }

    openInNewWindow();
}

void MiniPlayer::onPlayButtonClicked()
{
    switch (playerContext->getMediaState()) {
        case bb::multimedia::MediaState::Paused:
        case bb::multimedia::MediaState::Stopped:
        case bb::multimedia::MediaState::Prepared: {
            bb::multimedia::MediaError::Type error = playerContext->play();

            if (error == bb::multimedia::MediaError::None) {
                playerContext->acquire();
                playButton->setDefaultImageSource(QString("asset:///images/ic_pause.png"));
                playButton->setPressedImageSource(QString("asset:///images/ic_pause.png"));
            } else {
                UIUtils::toastError("Source unavailable");
            }
        }
            break;
        case bb::multimedia::MediaState::Started:
            playerContext->pause();
            playButton->setDefaultImageSource(QString("asset:///images/ic_play.png"));
            playButton->setPressedImageSource(QString("asset:///images/ic_play.png"));
            break;
    }
}

void MiniPlayer::onStopButtonClicked()
{
    playerContext->stopMiniPlayer();
}

void MiniPlayer::onPlaylistButtonClicked()
{
    PlaylistSheet *sheet = new PlaylistSheet(navigationPane);
}

void MiniPlayer::onMiniPlayerStopped()
{
    this->setVisible(false);
}

void MiniPlayer::onPlaylistChanged(int playlistId)
{
    playlistButtonContainer->setVisible(playlistId > 0);
}

void MiniPlayer::onGoToVideoClickableContainerTrackpadEvent(bb::cascades::TrackpadEvent *e)
{
    if (e->trackpadEventType() == bb::cascades::TrackpadEventType::Press) {
        openInNewWindow();
    }
}

void MiniPlayer::onEnterKey()
{
    openInNewWindow();
}

void MiniPlayer::openInNewWindow()
{
    navigationPane->push(new PlayerPage(navigationPane));
}
