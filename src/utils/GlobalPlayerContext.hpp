#ifndef GLOBALPLAYERCONTEXT_HPP_
#define GLOBALPLAYERCONTEXT_HPP_

#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/PlaylistListItemModel.hpp"

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/MediaState>
#include <bb/multimedia/MediaError>
#include <bb/multimedia/EqualizerPreset>
#include <bb/multimedia/NowPlayingConnection>
#include <bb/multimedia/RepeatMode>

#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/UIOrientation>
#include <bb/cascades/ScalingMethod>

class GlobalPlayerContext: public QObject
{
    Q_OBJECT
    signals:
    void npcNext();
    void npcPrev();
    void positionChanged(unsigned int);
    void mediaStateChanged(bb::multimedia::MediaState::Type);
    void orientationChanged();
    void metadataChanged();
    void miniPlayerStopped();
    void playlistChanged(int);

private slots:
    void onNpcAcquired();
    void onNpcPause();
    void onNpcPlay();
    void onNpcPrev();
    void onNpcNext();
    void onNpcRevoked();
    void onPlayerDurationChanged(unsigned int);
    void onMediaStateChanged(bb::multimedia::MediaState::Type);
    void onVideoDimensionsChanged(const QSize&);
    void onPlayerPositionChanged(unsigned int);
    void onOrientationAboutToChange(bb::cascades::UIOrientation::Type);
    void onPlaylistVideoAdded(PlaylistVideoModel*);
    void onPlaylistVideoDeleted(QString, PlaylistListItemModel::Type);
    void onPlaylistVideoDeletedAll(PlaylistListItemModel::Type);
private:
    bb::multimedia::MediaPlayer *mediaPlayer;
    bb::multimedia::NowPlayingConnection *npc;
    bb::cascades::ForeignWindowControl *fwc;
    bb::cascades::ForeignWindowControl *ccfwc;
    bool classic;
    bool audioOnly;
    bool updateViewedPercent;
    bool continuePlaying;
    VideoMetadata videoMetadata;
    StorageData storageData;
    SingleVideoStorageData selectedStorageData;
    bb::cascades::ScalingMethod::Type scalingMethod;
    bb::cascades::ScalingMethod::Type playerScalingMethod;
    bb::multimedia::RepeatMode::Type repeatMode;
    QString ccLanguage;

    int frameWidth;
    int frameHeight;
    int screenWidth;
    int screenHeight;
    QString windowId;
    QString windowGroup;
    screen_window_t windowHandle;
    QString ccWindowId;
    QString ccWindowGroup;
    screen_window_t ccWindowHandle;

    void setFrameSize(int frameWidth, int frameHeight);
    void setScreenSize();
    void resizeVideo();
    void resizeVideoLogic(bb::cascades::ForeignWindowControl*);

    int playlistId;
    QList<QString> playlistSequence;
public:
    GlobalPlayerContext(QObject *parent = 0);
    bb::multimedia::MediaPlayer *getMediaPlayer();
    bb::multimedia::MediaState::Type getMediaState();
    int getPosition();
    int getDuration();
    bb::multimedia::MediaError::Type play();
    bb::multimedia::MediaError::Type play(QString url);
    bb::multimedia::MediaError::Type changeQuality(QString url);
    bb::multimedia::MediaError::Type setEqualizerPreset(
            bb::multimedia::EqualizerPreset::Type preset);
    void setMetadata(VideoMetadata videoMetadata, StorageData storageData, bool audioOnly);
    void setSelectedStorageData(SingleVideoStorageData storageData);
    void setAudioOnly(bool audioOnly);
    void pause();
    void seekTime(unsigned int);
    void skipXSecondsBackward(unsigned int);
    void skipXSecondsForward(unsigned int);
    void acquire();
    void setPrevEnabled(bool);
    void setNextEnabled(bool);
    void setNpcMetadata(QVariantMap);
    void setWindowToForeignWindowControl(bb::cascades::ForeignWindowControl*);
    void setWindowToCcForeignWindowControl(bb::cascades::ForeignWindowControl*);
    void setForeignWindowControl(bb::cascades::ForeignWindowControl*);
    void setCcForeignWindowControl(bb::cascades::ForeignWindowControl*);
    void resizeVideo(int frameWidth, int frameHeight);
    void goFullscreen();
    void resizeFullScreenVideo();
    bool isWindowInitialized();
    bool isClassic();
    bool hasKeyboard();
    void setWindowData(screen_window_t handle, QString windowId, QString windowGroup);
    screen_window_t getWindowHandle();
    void resetWindowHandle();
    void setCcWindowData(screen_window_t handle, QString windowId, QString windowGroup);
    void resetCcWindowHandle();
    VideoMetadata getVideoMetadata();
    StorageData getStorageData();
    SingleVideoStorageData getSelectedStorageData();
    bool isAudioOnly();
    bool containsVideo();
    void stopMiniPlayer();
    bb::cascades::ScalingMethod::Type getScalingMethod();
    void setScalingMethod(bb::cascades::ScalingMethod::Type);
    void resetScalingMethod();
    bb::cascades::ScalingMethod::Type getPlayerScalingMethod();
    void setPlayerScalingMethod(bb::cascades::ScalingMethod::Type method);
    void resetPlayerScalingMethod();
    int getScreenWidth();
    int getScreenHeight();
    bb::multimedia::EqualizerPreset::Type getEqualizerPreset();
    bb::multimedia::RepeatMode::Type getRepeatMode();
    void setRepeatMode(bb::multimedia::RepeatMode::Type);
    QString getCcLanguage();
    void setCcLanguage(QString language);
    void setClosedCaptionUrl(QUrl url);
    void resetClosedCaptionUrl();

    void setPlaylist(int playlistId, QList<QString> playlistSequence);
    int getPlaylistId();
    QString getNextVideoFromPlaylist();
    QString getPrevVideoFromPlaylist();
    QString getVideoFromPlaylistByIndex(int index);
    void stopPlaylist();
    void shufflePlaylist();
    QList<PlaylistVideoModel*> getPlaylistTransposition(const QMap<QString, PlaylistVideoModel*> *map);

    void setViewedPercent();
    void setContinuePlaying(bool value);
    bool isContinuePlaying();
};

#endif /* GLOBALPLAYERCONTEXT_HPP_ */
