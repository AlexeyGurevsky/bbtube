#ifndef CHANNELLISTPAGE_HPP_
#define CHANNELLISTPAGE_HPP_

#include "src/parser/YoutubeClient.hpp"
#include "src/utils/BasePage.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "RssVideoModel.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/Option>
#include <bb/cascades/Dropdown>
#include <QVariantList>
#include <QMutex>

class ChannelListPage: public BasePage
{
    Q_OBJECT
public:
    ChannelListPage(bb::cascades::NavigationPane *navigationPane);
    virtual ~ChannelListPage()
    {
    }
    virtual void playVideoFromOutside(QString url);
    void playVideo(QString videoId);
    void playAudioOnly(QString videoId);
    void openChannel(QString channelId);
private:
    bb::cascades::ListView *channelList;
    bb::cascades::Container *noChannelsContainer;
    bb::cascades::ActionItem *refreshActionItem;
    bb::cascades::ActionItem *markAllAsSeenActionItem;
    bb::cascades::ActionItem *sortActionItem;
    bb::cascades::ActionItem *searchActionItem;
    bb::cascades::InvokeActionItem *shareActionItem;
    bb::cascades::Option *channelsOption;
    bb::cascades::Option *feedOption;
    bb::cascades::SegmentedControl *segmented;
    bb::cascades::Container *channelsSegmentContainer;
    bb::cascades::Container *feedSegmentContainer;
    CustomListView *feedList;
    bb::cascades::Container *sortControls;
    bb::cascades::Container *channelListContainer;
    bb::cascades::DropDown *sortByDropdown;
    bb::cascades::DropDown *sortOrderDropdown;

    QMutex mutex;
    QMap<QString, QList<RssVideoModel> > videosFromRss;
    int channelsToUpdate;
    int channelsRead;

    void setAvailability();
    void buildFeed();
    void updateTitle();
    static bool feedComparator(VideoListItemModel *item1, VideoListItemModel *item2);
private slots:
    void onChannelListItemClick(QVariantList);
    void onRemoveFromChannelsActionItemClick(QVariantList);
    void onChannelDataReceived(ChannelPageData);
    void onYoutubeError(QString);
    void onChannelItemAdded(ChannelListItemModel *);
    void onChannelItemUpdated(ChannelListItemModel *);
    void onChannelItemDeleted(QString);
    void onRefreshActionItemClick();
    void onMarkAllAsSeenActionItemClick();
    void onImportActionItemClick();
    void onShareActionItemClick();
    void onSortActionItemClick();
    void onSearchActionItemClick();
    void onSortDropdownSelectedIndexChanged(int);
    void onFeedFinished();
    void onSegmentedSelectedIndexChanged(int);
    void onChannelActionItemClick(QVariantList);
    void onFeedListItemClick(QVariantList);
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath);
    void onMetadataReceived(VideoMetadata, StorageData);
};

class ChannelListPageFeedListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~ChannelListPageFeedListItemActionSetBuilder()
    {
    }

    virtual void buildActionSet(const VideoListItemModel* item, SearchListItem *listItem)
    {
        listItem->addContinueActionItem();
        listItem->addPlayAudioOnlyActionItem();
        listItem->addChannelActionItem();
        buildCommonVideoActionSet(item, listItem);
    }
};
#endif /* CHANNELLISTPAGE_HPP_ */
