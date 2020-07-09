#ifndef CHANNELPAGE_HPP_
#define CHANNELPAGE_HPP_

#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/utils/BasePage.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <QVariantList>

class ChannelPage: public BasePage
{
Q_OBJECT
private slots:
    void onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData);
    void onVideoListItemClick(QVariantList indexPath);
    void onChannelVideosNextBatchReceived(ChannelPageData);
    void onYoutubeError(QString message);
    void onCopyRssLinkActionItemClick();
    void onOpenChannelInBrowserActionItemClick();
    void onCopyChannelLinkActionItemClick();
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath);
    void onAddToChannelsActionItemClick();
private:
    ChannelPageData channelData;
    CustomListView *videoList;
    static int pageSize;

    bb::cascades::ActionItem *addToChannelsActionItem;
private slots:
    void onShowMore(QVariantList);
public:
    ChannelPage(ChannelPageData channelData, bb::cascades::NavigationPane *navigationPane);
    virtual ~ChannelPage()
    {
    }
};

class ChannelPageSearchListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~ChannelPageSearchListItemActionSetBuilder()
    {
    }

    virtual void buildActionSet(const VideoListItemModel* item, SearchListItem *listItem)
    {
        listItem->addContinueActionItem();
        if (!item->isLiveStream()) {
            listItem->addPlayAudioOnlyActionItem();
        }
        buildCommonVideoActionSet(item, listItem);
    }
};

#endif /* CHANNELPAGE_HPP_ */
