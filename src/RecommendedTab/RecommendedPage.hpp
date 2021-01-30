#ifndef RecommendedPage_HPP_
#define RecommendedPage_HPP_

#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/models/RecommendedData.hpp"
#include "src/utils/BasePage.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <QVariantList>

class RecommendedPage: public BasePage
{
Q_OBJECT
private slots:
    void onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData);
    void onListItemClick(QVariantList indexPath);
    void onRecommendedDataReceived(RecommendedData);
    void onNextBatchReceived(RecommendedData);
    void onYoutubeError(QString message);
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath);
    void onRefreshActionItemClick();
    void onChannelActionItemClick(QVariantList);
    void onChannelDataReceived(ChannelPageData);
private:
    RecommendedData recommendedData;
    CustomListView *videoList;
    static int pageSize;
    bool isLoaded;

    void lazyLoad();
private slots:
    void onShowMore(QVariantList);
public:
    RecommendedPage(bb::cascades::NavigationPane *navigationPane);
    virtual ~RecommendedPage()
    {
    }
};

class RecommendedPageListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~RecommendedPageListItemActionSetBuilder()
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

#endif /* RecommendedPage_HPP_ */
