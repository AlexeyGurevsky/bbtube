#ifndef PlaylistSheet_HPP_
#define PlaylistSheet_HPP_

#include "src/utils/BaseSheet.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/utils/CustomListView.hpp"

#include <QObject>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ListView>

using namespace bb::cascades;

class PlaylistSheet: public BaseSheet
{
Q_OBJECT
public:
    PlaylistSheet(NavigationPane *navigationPane);
    virtual ~PlaylistSheet()
    {
    }
private slots:
    void onListItemClick(QVariantList);
    void onChannelActionItemClick(QVariantList);
    void onPlayAudioOnlyActionItemClick(QVariantList);
    void onDeleteActionItemClick(QVariantList);
    void onChannelDataReceived(ChannelPageData);
    void onToTopActionItemClick();
    void onToBottomActionItemClick();
    void onShuffleActionItemClick();
    void onMetadataChanged();
private:
    CustomListView *list;
    Container *overlay;
    NavigationPane *navigationPane;
    GlobalPlayerContext *playerContext;
    PlaylistListItemModel *playlist;
    YoutubeClient *youtubeClient;
    bool audioOnly;

    void playVideo(QString videoId);
};

#endif /* PlaylistSheet_HPP_ */
