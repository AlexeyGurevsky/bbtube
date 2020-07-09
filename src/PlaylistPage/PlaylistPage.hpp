#ifndef PlaylistPage_HPP_
#define PlaylistPage_HPP_

#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/utils/BasePage.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/DropDown>
#include <bb/cascades/ActionItem>
#include <bb/cascades/DeleteActionItem>
#include <bb/system/SystemUiResult>
#include <QVariantList>

class PlaylistPage: public BasePage
{
Q_OBJECT
private slots:
    void onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData);
    void onChannelDataReceived(ChannelPageData);
    void onYoutubeError(QString message);
    void onPlaylistVideoAdded(PlaylistVideoModel* video);
    void onPlaylistVideoUpdated(PlaylistVideoModel* video);
    void onPlaylistVideoDeleted(QString videoId, PlaylistListItemModel::Type playlistType);
    void onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType);
    void onVideoListItemClick(QVariantList indexPath);
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath);
    void onChannelActionItemClick(QVariantList indexPath);
    void onDeleteActionItemClick(QVariantList indexPath);
    void onSortActionItemClick();
    void onSearchActionItemClick();
    void onSortDropdownSelectedIndexChanged(int index);
    void onToTopActionItemClick();
    void onToBottomActionItemClick();
    void onPlayAllActionItemClick();
    void onShuffleAllActionItemClick();
    void onDeleteAllActionItemClick();
    void onDeleteAllDialogFinished(bb::system::SystemUiResult::Type);
private:
    int playlistId;
    PlaylistListItemModel *playlist;
    GlobalPlayerContext *playerContext;

    bb::cascades::Container *videoListContainer;
    bb::cascades::Container *noVideosContainer;
    bb::cascades::Container *sortControls;
    CustomListView *videoList;
    bb::cascades::DropDown *sortByDropdown;
    bb::cascades::DropDown *sortOrderDropdown;

    bb::cascades::ActionItem *sortActionItem;
    bb::cascades::ActionItem *searchActionItem;
    bb::cascades::ActionItem *toTopActionItem;
    bb::cascades::ActionItem *toBottomActionItem;
    bb::cascades::DeleteActionItem *deleteAllActionItem;
    bb::cascades::ActionItem *playAllActionItem;
    bb::cascades::ActionItem *shuffleAllActionItem;

    void updateTitle();
    void setAvailability();
    void setPlaylist();
    void playAll(bool shuffle);
public:
    PlaylistPage(int playlistId, bb::cascades::NavigationPane *navigationPane);
    virtual ~PlaylistPage()
    {
        delete playlist;
    }

    void playVideo(QString videoId);
    void playAudioOnly(QString videoId);
    void openChannel(QString channelId);
    PlaylistListItemModel *getPlaylist() const
    {
        return playlist;
    }
};

#endif /* PlaylistPage_HPP_ */
