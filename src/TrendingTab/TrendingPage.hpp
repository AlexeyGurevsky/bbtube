#ifndef TrendingPage_HPP_
#define TrendingPage_HPP_

#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/models/TrendingData.hpp"
#include "src/utils/BasePage.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/QListDataModel>

#include <QVariantList>
#include <QMap>

class TrendingPage: public BasePage
{
Q_OBJECT
private slots:
    void onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData);
    void onListItemClick(QVariantList indexPath);
    void onTrendingDataReceived(TrendingData);
    void onYoutubeError(QString message);
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath);
    void onRefreshActionItemClick();
    void onChannelActionItemClick(QVariantList);
    void onChannelDataReceived(ChannelPageData);
    void onSegmentedSelectedIndexChanged(int);
private:
    QList<TrendingDataCategory> categories;
    CustomListView *videoList;
    bb::cascades::SegmentedControl *segmented;
    QMap<int, QListDataModel<VideoListItemModel*>*> modelsMap;
    bool isLoaded;

    void lazyLoad();
public:
    TrendingPage(bb::cascades::NavigationPane *navigationPane);
    virtual ~TrendingPage()
    {
    }
};

class TrendingPageListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~TrendingPageListItemActionSetBuilder()
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

#endif /* TrendingPage_HPP_ */
