#ifndef PLAYERPAGE_HPP_
#define PLAYERPAGE_HPP_

#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/utils/BasePage.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/settings/AppSettings.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/Slider>
#include <bb/cascades/Label>
#include <bb/cascades/ListView>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/ImageButton>

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/MediaState>
#include <bb/multimedia/NowPlayingConnection>
#include <QStringList>
#include <bb/cascades/TouchEvent>
#include <bb/cascades/TrackpadEvent>
#include <bb/system/SystemUiResult>
#include <bb/cascades/DoubleTapEvent>

class PlayerPage: public BasePage
{
Q_OBJECT
private slots:
    void onForeignWindowBoundingChanged(bool);
    void onCcForeignWindowBoundingChanged(bool isBound);
    void onOrientationChanged();
    void onTouch(bb::cascades::TouchEvent *);
    void onMediaStateChanged(bb::multimedia::MediaState::Type);
    void onPlayActionItemClick();
    void onNextActionItemClick();
    void onPreviousActionItemClick();
    void onQualityActionItemClick();
    void onScalingActionItemClick();
    void onCcActionItemClick();
    void onTimecodeActionItemClick();
    void onDownloadActionItemClick();
    void onMetadataReceived(VideoMetadata, StorageData);
    void onPlayerPositionChanged(unsigned int);
    void onNpcPrev();
    void onNpcNext();
    void onProgressSliderValueChanged(float);
    void onProgressSliderImmediateValueChanged(float);
    void onProgressSliderTouch(bb::cascades::TouchEvent *);
    void onProgressSliderTrackpadEvent(bb::cascades::TrackpadEvent*);
    void onProgressSliderFocusedChanged(bool);
    void onCloseInfoContainerClick();
    void onBackward10ButtonClick();
    void onForward10ButtonClick();
    void onCloseButtonTrackpadEvent(bb::cascades::TrackpadEvent*);
    void onUpnextVideoListItemClick(QVariantList);
    void onQualityDialogFinished(bb::system::SystemUiResult::Type);
    void onScalingDialogFinished(bb::system::SystemUiResult::Type);
    void onCcDialogFinished(bb::system::SystemUiResult::Type);
    void onDownloadDialogFinished(bb::system::SystemUiResult::Type);
    void onChannelActionItemClick(QVariantList);
    void onChannelDataReceived(ChannelPageData);
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath);
    void onVideoChannelActionItemClick();
    void onOpenVideoInBrowserActionItemClick();
    void onCopyVideoLinkActionItemClick();
    void onAddToFavoritesActionItemClick();
    void onAddToWatchLaterActionItemClick();
    void addToHistory();
    void onBrowserActionItemClick();
    void onDoubleTappedHandler(bb::cascades::DoubleTapEvent*);
    void onLoopScalingShortcut();
    void onAudioOnlyShortcut();
    void onVideoShortcut();
    void onRepeatActionItemClick();
    void onPlaylistActionItemClick();
    void onPlaylistChanged(int);
    void onPlaylistVideoAdded(PlaylistVideoModel* video);
    void onPlaylistVideoDeleted(QString videoId, PlaylistListItemModel::Type playlistType);
    void onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType);
private:
    void init(VideoMetadata videoMetadata, StorageData storageData,
            bb::cascades::NavigationPane *navigationPane, bool audioOnly);
    VideoMetadata videoMetadata;
    StorageData storageData;
    AppSettings *appSettings;

    bb::cascades::ForeignWindowControl *foreignWindowControl;
    bb::cascades::ForeignWindowControl *ccForeignWindowControl;
    bb::cascades::Container *infoContainer;
    bb::cascades::Container *audioBackground;
    CustomListView *upNextListView;
    bb::cascades::Slider *progressSlider;
    bb::cascades::ImageButton *forward10Button;
    bb::cascades::ImageButton *backward10Button;
    bb::cascades::ActionItem *playlistActionItem;
    bb::cascades::ActionItem *qualityActionItem;
    bb::cascades::ActionItem *scalingActionItem;
    bb::cascades::ActionItem *nextActionItem;
    bb::cascades::ActionItem *previousActionItem;
    bb::cascades::ActionItem *downloadActionItem;
    bb::cascades::ActionItem *playActionItem;
    bb::cascades::ActionItem *repeatActionItem;
    bb::cascades::ActionItem *ccActionItem;
    bb::cascades::ActionItem *timecodeActionItem;
    bb::cascades::ActionItem *addToFavoritesActionItem;
    bb::cascades::ActionItem *addToWatchLaterActionItem;
    bb::cascades::InvokeActionItem *shareVideoActionItem;
    bb::cascades::Label *passedTime;
    bb::cascades::Label *remainingTime;
    bb::cascades::Label *title;
    bb::cascades::Label *subtitle;
    bb::cascades::ImageButton *closeButton;
    bb::cascades::Container *subtitleContainer;

    bool actionBarsVisible;
    bool movingInSlider;
    bool manualSeeking;
    bool trackpadFocusInSlider;
    bool sliderDoubleTap;
    bool isLiveStream;
    bool autoplay;
    bool alreadyPlaying;
    int duration;
    QString quality;
    QString nextVideoId;
    QString prevVideoId;
    void resizeVideo();
    void playVideo();
    void setInfos();
    void toggleInfosVisibility();
    void hideInfos();
    void showInfos();
    void setAudioOnly(bool audioOnly);
    void changeQuality(QString newQuality, QString url);
    int getIndexOfDefaultQuality();
    QString getScalingMethodString(bb::cascades::ScalingMethod::Type type);
    void adjustInfoScreen();
    void updateScaling(bb::cascades::ScalingMethod::Type newMethod);
    static QStringList watched;
    void updateRepeatActionItem();
    QString getCcPath(QString languageCode);
    void setPrevVideoId();
    void setNextVideoId();
    void setProgressSliderEnabled(bool value);
public:
    PlayerPage(VideoMetadata videoMetadata, StorageData storageData,
            bb::cascades::NavigationPane *navigationPane, bool audioOnly, bool isPlaylistPlaying =
                    false);
    PlayerPage(bb::cascades::NavigationPane *navigationPane);

    virtual ~PlayerPage()
    {
        playerContext->setCcForeignWindowControl(0);
    }
    virtual void playVideoFromOutside(QString url);
    virtual void playVideoFromPlaylist(QString url);
};

class PlayerPageSearchListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~PlayerPageSearchListItemActionSetBuilder()
    {
    }

    virtual void buildActionSet(const VideoListItemModel* item, SearchListItem *listItem)
    {
        listItem->addContinueActionItem();
        if (!item->isLiveStream()) {
            listItem->addPlayAudioOnlyActionItem();
        }
        listItem->addChannelActionItem();
        buildCommonVideoActionSet(item, listItem);
    }
};

#endif /* PLAYERPAGE_HPP_ */
