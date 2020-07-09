#ifndef MINIPLAYER_HPP_
#define MINIPLAYER_HPP_

#include "src/utils/GlobalPlayerContext.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/UIConfig>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/ImageButton>
#include <bb/cascades/TouchEvent>
#include <bb/multimedia/MediaState>
#include <bb/cascades/TrackpadEvent>

using namespace bb::cascades;
using namespace bb::multimedia;

class MiniPlayer: public Container
{
    Q_OBJECT
private:
    static float videoHeight;
    static float videoWidth;

    GlobalPlayerContext *playerContext;
    NavigationPane *navigationPane;
    ForeignWindowControl *foreignWindowControl;
    bool needHack;
    void setPositionAndSize(screen_window_t win, float x, float y, float width, float height);
    UIConfig *uiConfig;
    ImageView *backgroundImage;
    Label *title;
    ImageButton *playButton;
    Container *playlistButtonContainer;

    void openInNewWindow();
private slots:
    void onForeignWindowBoundingChanged(bool isBound);
    void onControlFrameChanged(const QRectF & rect);
    void onMetadataChanged();
    void onMediaStateChanged(bb::multimedia::MediaState::Type);
    void onGoToVideoTouch(bb::cascades::TouchEvent *);
    void onPlayButtonClicked();
    void onStopButtonClicked();
    void onPlaylistButtonClicked();
    void onMiniPlayerStopped();
    void onPlaylistChanged(int);
    void onGoToVideoClickableContainerTrackpadEvent(bb::cascades::TrackpadEvent *e);
    void onEnterKey();
public:
    MiniPlayer(NavigationPane *navigationPane);
    virtual ~MiniPlayer()
    {
    }
    void setVideo();
};

#endif /* MINIPLAYER_HPP_ */
