#include "ChannelPage.hpp"
#include "src/PlayerPage/PlayerPage.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/utils/ActionItemService.hpp"
#include "src/utils/MiniPlayer.hpp"
#include "src/utils/ChannelListProxy.hpp"

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
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/UIConfig>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <QVariantList>
#include <bb/system/Clipboard>
#include <bb/cascades/Color>

int ChannelPage::pageSize = 12;

ChannelPage::ChannelPage(ChannelPageData channelData, bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane), channelData(channelData)
{
    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);
    container->add(getTitleContainer());
    setTitle(channelData.title);

    videoList = new CustomListView();
    SearchFlatListItemProvider *listProvider = new SearchFlatListItemProvider(videoList,
            new ChannelPageSearchListItemActionSetBuilder());
    videoList->setListItemProvider(listProvider);
    container->add(videoList);
    root->add(container);

    root->add(overlay);

    this->setContent(root);

    QObject::connect(youtubeClient, SIGNAL(channelVideosNextBatchReceived(ChannelPageData)), this,
            SLOT(onChannelVideosNextBatchReceived(ChannelPageData)));
    QObject::connect(videoList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onVideoListItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(showMore(QVariantList)), this,
            SLOT(onShowMore(QVariantList)));
    QObject::connect(listProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
            SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));

    QListDataModel<VideoListItemModel*> *model = new QListDataModel<VideoListItemModel*>();

    for (int i = 0; i < pageSize && i < channelData.videos.count(); i++) {
        VideoListItemModel *item = VideoListItemModel::mapVideo(&channelData.videos[i]);
        model->append(item);
    }

    if (channelData.videos.count() > pageSize) {
        VideoListItemModel *item = new VideoListItemModel();
        item->type = VideoListItemModel::ShowMore;
        model->append(item);
    }

    videoList->setDataModel(model);

    bb::cascades::ActionItem *copyChannelLinkActionItem =
            ActionItemService::copyChannelLinkActionItem(this);
    addToChannelsActionItem = ActionItemService::addToChannelsActionItem(this);
    ActionItemService::updateAddToChannelsActionItem(addToChannelsActionItem,
            ChannelListProxy::getInstance()->contains(channelData.channelId));
    bb::cascades::ActionItem *copyRssLinkActionItem = ActionItemService::copyRssActionItem(this);
    bb::cascades::ActionItem *openChannelInBrowserActionItem =
            ActionItemService::openChannelInBrowserActionItem(this);
    bb::cascades::InvokeActionItem *shareChannelActionItem =
            ActionItemService::shareChannelActionItem(this);
    ActionItemService::setShareChannelText(shareChannelActionItem, channelData.title,
            channelData.channelId);

    this->addAction(addToChannelsActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(shareChannelActionItem, bb::cascades::ActionBarPlacement::Signature);
    this->addAction(openChannelInBrowserActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(copyChannelLinkActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(copyRssLinkActionItem, bb::cascades::ActionBarPlacement::InOverflow);

    QObject::connect(openChannelInBrowserActionItem, SIGNAL(triggered()), this,
            SLOT(onOpenChannelInBrowserActionItemClick()));
    QObject::connect(copyChannelLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyChannelLinkActionItemClick()));
    QObject::connect(copyRssLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyRssLinkActionItemClick()));
    QObject::connect(addToChannelsActionItem, SIGNAL(triggered()), this,
            SLOT(onAddToChannelsActionItemClick()));

    videoList->requestFocus();

    bool isFavourite = ChannelListProxy::getInstance()->contains(channelData.channelId);
    int nowTime = QDateTime::currentDateTimeUtc().toTime_t();

    if (isFavourite) {
        ChannelListItemModel channel;
        channel.channelId = channelData.channelId;
        channel.title = channelData.title;
        channel.thumbnailUrl = channelData.thumbnailUrl;
        channel.dateLastVisited = nowTime;
        DbHelper::updateChannelGeneralInfo(&channel);
        ChannelListProxy::getInstance()->updateChannelGeneralInfo(&channel);
    }
}

void ChannelPage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    BasePage::onMetadataReceived(videoMetadata, storageData);
    videoList->setEnabled(true);
}

void ChannelPage::onVideoListItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = videoList->dataModel()->data(indexPath).value<VideoListItemModel*>();

    if (item->type == VideoListItemModel::Video) {
        videoList->setEnabled(false);
        overlay->setVisible(true);
        youtubeClient->process("https://www.youtube.com/watch?v=" + item->id);
    }
}

void ChannelPage::onShowMore(QVariantList indexPath)
{
    int index = indexPath[0].toInt();
    if (channelData.videos.count() > index) {
        QListDataModel<VideoListItemModel*> *model =
                static_cast<QListDataModel<VideoListItemModel*> *>(videoList->dataModel());
        QList<VideoListItemModel*> temp;

        for (int i = index; i < index + pageSize && i < channelData.videos.count(); i++) {
            temp.append(VideoListItemModel::mapVideo(&channelData.videos[i]));
        }

        if (channelData.videos.count() > index + pageSize) {
            VideoListItemModel *item = new VideoListItemModel();
            item->type = VideoListItemModel::ShowMore;
            temp.append(item);
        }

        model->removeAt(index);
        model->append(temp);

        if (channelData.videos.count() - (index + pageSize) <= 2 * pageSize
                && channelData.ctoken != "") {
            // lazyload videos in the background
            youtubeClient->channelVideosNextBatch(&channelData);
        }
    }
}

void ChannelPage::onChannelVideosNextBatchReceived(ChannelPageData data)
{
    channelData.ctoken = data.ctoken;
    channelData.videos.append(data.videos);
}

void ChannelPage::onYoutubeError(QString message)
{
    videoList->setEnabled(true);
    BasePage::onYoutubeError(message);
}

void ChannelPage::onOpenChannelInBrowserActionItemClick()
{
    ActionItemService::openChannelInBrowser(channelData.channelId);
}
void ChannelPage::onCopyChannelLinkActionItemClick()
{
    ActionItemService::copyChannelLink(channelData.channelId);
}
void ChannelPage::onCopyRssLinkActionItemClick()
{
    ActionItemService::copyRss(channelData.channelId);
}
void ChannelPage::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    audioOnly = true;
    onVideoListItemClick(indexPath);
}
void ChannelPage::onAddToChannelsActionItemClick()
{
    bool isFavorite = !ChannelListProxy::getInstance()->contains(channelData.channelId);
    int nowTime = QDateTime::currentDateTimeUtc().toTime_t();

    if (isFavorite) {
        ChannelListItemModel channel(channelData.channelId, channelData.title,
                channelData.thumbnailUrl, nowTime, nowTime, "", "", 0);
        DbHelper::createChannel(&channel);
        ChannelListProxy::getInstance()->add(&channel);
    } else {
        DbHelper::deleteChannel(channelData.channelId);
        ChannelListProxy::getInstance()->deleteById(channelData.channelId);
    }

    ActionItemService::updateAddToChannelsActionItem(addToChannelsActionItem, isFavorite);
    UIUtils::toastInfo(isFavorite ? "Added to Channels" : "Removed from Channels");
}
