#ifndef BASEPAGE_HPP_
#define BASEPAGE_HPP_

#include "MiniPlayer.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "GlobalPlayerContext.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/UIConfig>

class BasePage: public bb::cascades::Page
{
Q_OBJECT
private:
    bb::cascades::Label *title;
protected:
    YoutubeClient *youtubeClient;
    GlobalPlayerContext *playerContext;
    MiniPlayer *miniPlayer;

    bb::cascades::NavigationPane *navigationPane;
    bb::cascades::Container *root;
    bb::cascades::Container *overlay;
    bb::cascades::UIConfig *ui;

    bool audioOnly;
    bool isPlaylist;
protected slots:
    virtual void onYoutubeError(QString message);
    virtual void onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData);
public:
    BasePage(bb::cascades::NavigationPane *navigationPane, bool addMiniPlayer = true);
    virtual ~BasePage()
    {
    }
    virtual void playVideoFromOutside(QString url);
    virtual void playVideoFromPlaylist(QString url);
    virtual void setVideoToPlayer();
    virtual void lazyLoad();

    bb::cascades::Container *getTitleContainer();
    void setTitle(QString title);
    void setAudioOnly(bool value);
    void setIsPlaylist(bool value);
};

#endif /* BASEPAGE_HPP_ */
