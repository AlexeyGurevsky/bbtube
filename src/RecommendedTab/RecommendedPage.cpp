#include "RecommendedPage.hpp"
#include "src/PlayerPage/PlayerPage.hpp"
#include "src/ChannelPage/ChannelPage.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/models/RecommendedData.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/utils/MiniPlayer.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Label>
#include <bb/cascades/Divider>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ListView>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/DockLayout>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/UIConfig>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <QVariantList>

int RecommendedPage::pageSize = 12;

RecommendedPage::RecommendedPage(bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane)
{
    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);
    container->add(getTitleContainer());
    setTitle("Recommended");

    videoList = new CustomListView();
    SearchFlatListItemProvider *listProvider = new SearchFlatListItemProvider(videoList,
            new RecommendedPageListItemActionSetBuilder());
    videoList->setListItemProvider(listProvider);
    container->add(videoList);
    root->add(container);

    root->add(overlay);

    this->setContent(root);

    QObject::connect(youtubeClient, SIGNAL(recommendedNextBatchReceived(RecommendedData)), this,
            SLOT(onNextBatchReceived(RecommendedData)));
    QObject::connect(youtubeClient, SIGNAL(recommendedDataReceived(RecommendedData)), this,
            SLOT(onRecommendedDataReceived(RecommendedData)));
    QObject::connect(youtubeClient, SIGNAL(channelDataReceived(ChannelPageData)), this,
                SLOT(onChannelDataReceived(ChannelPageData)));
    QObject::connect(videoList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onListItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(showMore(QVariantList)), this,
            SLOT(onShowMore(QVariantList)));
    QObject::connect(listProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
            SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));

    bb::cascades::ActionItem *refreshActionItem = bb::cascades::ActionItem::create();
    refreshActionItem->setImageSource(QString("asset:///images/ic_reload.png"));
    refreshActionItem->setTitle("Refresh");
    refreshActionItem->addShortcut(bb::cascades::Shortcut::create().key("r"));
    this->addAction(refreshActionItem, bb::cascades::ActionBarPlacement::Signature);
    QObject::connect(refreshActionItem, SIGNAL(triggered()), this,
            SLOT(onRefreshActionItemClick()));

    overlay->setVisible(true);
    youtubeClient->recommended();
}

void RecommendedPage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    BasePage::onMetadataReceived(videoMetadata, storageData);
    videoList->setEnabled(true);
}

void RecommendedPage::onListItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = videoList->dataModel()->data(indexPath).value<VideoListItemModel*>();

    if (item->type == VideoListItemModel::Video) {
        videoList->setEnabled(false);
        overlay->setVisible(true);
        youtubeClient->process("https://www.youtube.com/watch?v=" + item->id);
    }
}

void RecommendedPage::onShowMore(QVariantList indexPath)
{
    int index = indexPath[0].toInt();
    if (recommendedData.videos.count() > index) {
        QListDataModel<VideoListItemModel*> *model =
                static_cast<QListDataModel<VideoListItemModel*> *>(videoList->dataModel());
        QList<VideoListItemModel*> temp;

        for (int i = index; i < index + pageSize && i < recommendedData.videos.count(); i++) {
            temp.append(VideoListItemModel::mapVideo(&recommendedData.videos[i]));
        }

        if (recommendedData.videos.count() > index + pageSize) {
            VideoListItemModel *item = new VideoListItemModel();
            item->type = VideoListItemModel::ShowMore;
            temp.append(item);
        }

        model->removeAt(index);
        model->append(temp);

        if (recommendedData.videos.count() - (index + pageSize) <= 2 * pageSize
                && recommendedData.ctoken != "") {
            // lazyload videos in the background
            youtubeClient->recommendedNextBatch(&recommendedData);
        }
    }
}

void RecommendedPage::onNextBatchReceived(RecommendedData data)
{
    recommendedData.ctoken = data.ctoken;
    recommendedData.videos.append(data.videos);
}

void RecommendedPage::onYoutubeError(QString message)
{
    videoList->setEnabled(true);
    BasePage::onYoutubeError(message);
}

void RecommendedPage::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    audioOnly = true;
    onListItemClick(indexPath);
}

void RecommendedPage::onRecommendedDataReceived(RecommendedData data)
{
    recommendedData = data;

    QListDataModel<VideoListItemModel*> *model = new QListDataModel<VideoListItemModel*>();
    for (int i = 0; i < pageSize && i < recommendedData.videos.count(); i++) {
        VideoListItemModel *item = VideoListItemModel::mapVideo(&recommendedData.videos[i]);
        model->append(item);
    }

    if (recommendedData.videos.count() > pageSize) {
        VideoListItemModel *item = new VideoListItemModel();
        item->type = VideoListItemModel::ShowMore;
        model->append(item);
    }

    videoList->setDataModel(model);
    videoList->requestFocus();
    overlay->setVisible(false);

    if (recommendedData.ctoken != "") {
        // lazyload videos in the background
        youtubeClient->recommendedNextBatch(&recommendedData);
    }
}

void RecommendedPage::onChannelActionItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = videoList->dataModel()->data(indexPath).value<VideoListItemModel*>();
    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void RecommendedPage::onChannelDataReceived(ChannelPageData data)
{
    navigationPane->push(new ChannelPage(data, this->navigationPane));
    overlay->setVisible(false);
    videoList->setEnabled(true);
}

void RecommendedPage::onRefreshActionItemClick()
{
    overlay->setVisible(true);
    youtubeClient->recommended();
}
