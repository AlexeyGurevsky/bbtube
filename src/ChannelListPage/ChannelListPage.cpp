#include "ChannelListPage.hpp"
#include "ChannelListItemProvider.hpp"
#include "src/utils/BasePage.hpp"
#include "src/utils/MiniPlayer.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/db/DbHelper.hpp"
#include "src/ChannelPage/ChannelPage.hpp"
#include "src/utils/ChannelListProxy.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/applicationui.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "ImportChannelsSheet.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/utils/DateTimeUtils.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "SearchChannelsSheet.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"

#include "bb/data/XmlDataAccess"
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/Label>
#include <bb/cascades/ListView>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/InvokeQuery>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/Option>
#include <bb/cascades/Divider>

ChannelListPage::ChannelListPage(bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane)
{
    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);
    container->add(getTitleContainer());

    channelsOption = bb::cascades::Option::create().text("Channels").selected(true);
    feedOption = bb::cascades::Option::create().text("Feed").selected(false).enabled(false);
    segmented = bb::cascades::SegmentedControl::create().add(channelsOption).add(feedOption);
    container->add(segmented);

    channelsSegmentContainer = new bb::cascades::Container();
    channelsSegmentContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    channelsSegmentContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    channelListContainer = bb::cascades::Container::create();
    sortControls = bb::cascades::Container::create().visible(false);
    bb::cascades::Container *sortControlsInner = bb::cascades::Container::create().layout(
            bb::cascades::StackLayout::create().orientation(LayoutOrientation::LeftToRight));
    sortByDropdown = bb::cascades::DropDown::create().title("Sort by").layoutProperties(
            bb::cascades::StackLayoutProperties::create().spaceQuota(1));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Last Activity").value(
                    (int) ChannelListItemModel::LastActivity).selected(true));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Name").value((int) ChannelListItemModel::Name));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Date Visited").value(
                    (int) ChannelListItemModel::DateVisited));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Date Added").value(
                    (int) ChannelListItemModel::DateAdded));
    sortOrderDropdown = bb::cascades::DropDown::create().title("Order").layoutProperties(
            bb::cascades::StackLayoutProperties::create().spaceQuota(1));
    sortOrderDropdown->add(
            bb::cascades::Option::create().text("Ascending").value(
                    (int) ChannelListItemModel::Asc));
    sortOrderDropdown->add(
            bb::cascades::Option::create().text("Descending").value(
                    (int) ChannelListItemModel::Desc).selected(true));
    sortControlsInner->add(sortByDropdown);
    sortControlsInner->add(sortOrderDropdown);
    sortControls->add(sortControlsInner);
    sortControls->add(bb::cascades::Divider::create());
    channelListContainer->add(sortControls);
    channelList = bb::cascades::ListView::create();
    ChannelListItemProvider *listProvider = new ChannelListItemProvider();
    channelList->setListItemProvider(listProvider);
    channelList->setTopMargin(ui->du(1));
    channelListContainer->add(channelList);
    channelsSegmentContainer->add(channelListContainer);

    noChannelsContainer = bb::cascades::Container::create().layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    noChannelsContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    noChannelsContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    noChannelsContainer->setLayout(new bb::cascades::DockLayout());
    bb::cascades::Container *noChannelsLabelContainer = new bb::cascades::Container();
    noChannelsLabelContainer->setLeftPadding(ui->du(5));
    noChannelsLabelContainer->setRightPadding(ui->du(5));
    noChannelsLabelContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Center);
    noChannelsLabelContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    bb::cascades::Label *noChannelsLabel = bb::cascades::Label::create().text("No channels");
    noChannelsLabel->textStyle()->setBase(bb::cascades::SystemDefaults::TextStyles::titleText());
    noChannelsLabel->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    noChannelsLabelContainer->add(noChannelsLabel);
    noChannelsContainer->add(noChannelsLabelContainer);
    channelsSegmentContainer->add(noChannelsContainer);
    container->add(channelsSegmentContainer);

    feedSegmentContainer = bb::cascades::Container::create().visible(false);
    feedSegmentContainer->add(
            bb::cascades::Label::create().multiline(true).text(
                    "The feed contains last 15 videos from each channel posted for the last 90 days"));
    feedList = new CustomListView();
    SearchFlatListItemProvider *feedListProvider = new SearchFlatListItemProvider(feedList,
            new ChannelListPageFeedListItemActionSetBuilder());
    feedList->setListItemProvider(feedListProvider);
    feedList->setTopMargin(ui->du(1));
    feedSegmentContainer->add(feedList);
    container->add(feedSegmentContainer);

    root->add(container);
    root->add(overlay);
    this->setContent(root);

    QObject::connect(youtubeClient, SIGNAL(channelDataReceived(ChannelPageData)), this,
            SLOT(onChannelDataReceived(ChannelPageData)));
    QObject::connect(channelList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onChannelListItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(removeFromChannelsActionItemClick(QVariantList)), this,
            SLOT(onRemoveFromChannelsActionItemClick(QVariantList)));
    QObject::connect(segmented, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSegmentedSelectedIndexChanged(int)));
    QObject::connect(ChannelListProxy::getInstance(), SIGNAL(added(ChannelListItemModel*)), this,
            SLOT(onChannelItemAdded(ChannelListItemModel*)));
    QObject::connect(ChannelListProxy::getInstance(), SIGNAL(updated(ChannelListItemModel*)), this,
            SLOT(onChannelItemUpdated(ChannelListItemModel*)));
    QObject::connect(ChannelListProxy::getInstance(), SIGNAL(deleted(QString)), this,
            SLOT(onChannelItemDeleted(QString)));
    QObject::connect(feedListProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));
    QObject::connect(feedListProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
            SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(feedList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onFeedListItemClick(QVariantList)));
    QObject::connect(sortByDropdown, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSortDropdownSelectedIndexChanged(int)));
    QObject::connect(sortOrderDropdown, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSortDropdownSelectedIndexChanged(int)));

    refreshActionItem = bb::cascades::ActionItem::create();
    refreshActionItem->setImageSource(QString("asset:///images/ic_reload.png"));
    refreshActionItem->setTitle("Refresh");
    refreshActionItem->addShortcut(bb::cascades::Shortcut::create().key("r"));
    markAllAsSeenActionItem = bb::cascades::ActionItem::create();
    markAllAsSeenActionItem->setImageSource(QString("asset:///images/ic_done.png"));
    markAllAsSeenActionItem->setTitle("Mark All as Seen");
    markAllAsSeenActionItem->addShortcut(bb::cascades::Shortcut::create().key("m"));
    bb::cascades::ActionItem *importActionItem = bb::cascades::ActionItem::create();
    importActionItem->setImageSource(QString("asset:///images/ic_import.png"));
    importActionItem->setTitle("Import Channels");
    sortActionItem = bb::cascades::ActionItem::create();
    sortActionItem->setImageSource(QString("asset:///images/ic_sort.png"));
    sortActionItem->setTitle("Sort Channels");
    sortActionItem->addShortcut(bb::cascades::Shortcut::create().key("o"));
    searchActionItem = bb::cascades::ActionItem::create();
    searchActionItem->setImageSource(QString("asset:///images/ic_search.png"));
    searchActionItem->setTitle("Search");
    searchActionItem->addShortcut(bb::cascades::Shortcut::create().key("s"));
    bb::cascades::InvokeQuery *shareQuery = new bb::cascades::InvokeQuery(this);
    shareQuery->setInvokeActionId("bb.action.SHARE");
    shareQuery->setUri(QUrl("file://" + QDir::currentPath() + "/tmp/channels.txt"));
    shareQuery->setMimeType("application/xml");
    shareActionItem = bb::cascades::InvokeActionItem::create(shareQuery).parent(this);
    shareActionItem->setImageSource(QString("asset:///images/ic_share.png"));
    shareActionItem->setTitle("Share Channels");

    this->addAction(importActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(shareActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(searchActionItem, bb::cascades::ActionBarPlacement::OnBar);
    this->addAction(refreshActionItem, bb::cascades::ActionBarPlacement::Signature);
    this->addAction(sortActionItem, bb::cascades::ActionBarPlacement::OnBar);
    this->addAction(markAllAsSeenActionItem, bb::cascades::ActionBarPlacement::InOverflow);

    QObject::connect(refreshActionItem, SIGNAL(triggered()), this,
            SLOT(onRefreshActionItemClick()));
    QObject::connect(markAllAsSeenActionItem, SIGNAL(triggered()), this,
            SLOT(onMarkAllAsSeenActionItemClick()));
    QObject::connect(importActionItem, SIGNAL(triggered()), this, SLOT(onImportActionItemClick()));
    QObject::connect(shareActionItem, SIGNAL(triggered()), this, SLOT(onShareActionItemClick()));
    QObject::connect(sortActionItem, SIGNAL(triggered()), this, SLOT(onSortActionItemClick()));
    QObject::connect(searchActionItem, SIGNAL(triggered()), this, SLOT(onSearchActionItemClick()));

    UpdatableDataModel<ChannelListItemModel*> *model =
            new UpdatableDataModel<ChannelListItemModel*>(DbHelper::getChannelList());
    channelList->setDataModel(model);
    updateTitle();

    setAvailability();
    if (channelList->isVisible()) {
        channelList->requestFocus();
    }
}

void ChannelListPage::playVideoFromOutside(QString url)
{
    overlay->setVisible(true);
    youtubeClient->process(url);
}

void ChannelListPage::onChannelListItemClick(QVariantList indexPath)
{
    ChannelListItemModel *item = channelList->dataModel()->data(indexPath).value<
            ChannelListItemModel*>();
    channelList->setEnabled(false);
    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void ChannelListPage::onRemoveFromChannelsActionItemClick(QVariantList indexPath)
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    ChannelListItemModel *item = dataModel->data(indexPath).value<ChannelListItemModel*>();
    DbHelper::deleteChannel(item->channelId);
    ChannelListProxy::getInstance()->deleteById(item->channelId);

    UIUtils::toastInfo("Removed from Channels");
}

void ChannelListPage::onChannelDataReceived(ChannelPageData channelData)
{
    navigationPane->push(new ChannelPage(channelData, navigationPane));
    overlay->setVisible(false);
    channelList->setEnabled(true);
    feedList->setEnabled(true);
}

void ChannelListPage::onYoutubeError(QString message)
{
    channelList->setEnabled(true);
    BasePage::onYoutubeError(message);
}

void ChannelListPage::onChannelItemAdded(ChannelListItemModel *item)
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    ChannelListItemModel *newItem = new ChannelListItemModel(item->channelId, item->title,
            item->thumbnailUrl, item->dateAdded, item->dateLastVisited, item->lastVideoId,
            item->lastVideoTitle, item->dateLastActivity);
    dataModel->insert(0, newItem);

    updateTitle();
    setAvailability();
}

void ChannelListPage::onChannelItemUpdated(ChannelListItemModel *item)
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    for (int i = 0; i < dataModel->size(); i++) {
        ChannelListItemModel *itemToUpdate = dataModel->value(i);

        if (itemToUpdate->channelId == item->channelId) {
            itemToUpdate->title = item->title;
            itemToUpdate->thumbnailUrl = item->thumbnailUrl;
            itemToUpdate->dateLastVisited = item->dateLastVisited;

            QVariantList indexPath;
            indexPath.append(i);
            dataModel->updateItem(indexPath);

            break;
        }
    }
}

void ChannelListPage::onChannelItemDeleted(QString channelId)
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();

    for (int i = 0; i < dataModel->size(); i++) {
        if (dataModel->value(i)->channelId == channelId) {
            dataModel->removeAt(i);

            break;
        }
    }

    updateTitle();
    setAvailability();
}

void ChannelListPage::onRefreshActionItemClick()
{
    refreshActionItem->setEnabled(false);
    overlay->setVisible(true);
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    videosFromRss.clear();
    channelsToUpdate = dataModel->size();
    channelsRead = 0;

    for (int i = 0; i < dataModel->size(); i++) {
        ChannelListItemModel *itemToUpdate = dataModel->value(i);

        QNetworkRequest request(
                "https://www.youtube.com/feeds/videos.xml?channel_id=" + itemToUpdate->channelId);
        QNetworkReply *reply = ApplicationUI::networkManager->get(request);
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(onFeedFinished()));
    }
}

void ChannelListPage::onMarkAllAsSeenActionItemClick()
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    int utcNow = QDateTime::currentDateTimeUtc().toTime_t();
    QVariantList indexPath;

    DbHelper::markAllChannelsAsRead(utcNow);
    indexPath.append(0);

    for (int i = 0; i < dataModel->size(); i++) {
        ChannelListItemModel *itemToUpdate = dataModel->value(i);
        bool needUpdate = itemToUpdate->dateLastActivity > itemToUpdate->dateLastVisited;

        itemToUpdate->dateLastVisited = utcNow;

        if (needUpdate) {
            indexPath[0] = i;
            dataModel->updateItem(indexPath);
        }
    }
}

void ChannelListPage::setAvailability()
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    bool hasChannels = dataModel->size() > 0;

    channelListContainer->setVisible(hasChannels);
    noChannelsContainer->setVisible(!hasChannels);
    refreshActionItem->setEnabled(hasChannels);
    markAllAsSeenActionItem->setEnabled(hasChannels);
    shareActionItem->setEnabled(hasChannels);
    sortActionItem->setEnabled(hasChannels);
    searchActionItem->setEnabled(hasChannels);
}

void ChannelListPage::onImportActionItemClick()
{
    ImportChannelsSheet *sheet = new ImportChannelsSheet();
}

void ChannelListPage::onShareActionItemClick()
{
    QString appFolder(QDir::homePath());
    appFolder.chop(4);
    QString fileName = appFolder + "tmp/channels.txt";
    QFile file(fileName);
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "Channel Id,Channel Url,Channel Title"<<endl;

        for (int i = 0; i < dataModel->size(); i++) {
            ChannelListItemModel *item = dataModel->value(i);
            stream<<QString("%1,http://www.youtube.com/channel/%1,%2").arg(item->channelId, item->title)<<endl;
        }
    }
}

void ChannelListPage::onFeedFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(QObject::sender());

    mutex.lock();
    channelsRead++;
    mutex.unlock();

    if (!reply->error()) {
        bb::data::XmlDataAccess xda;
        QVariantList list =
                xda.loadFromBuffer(reply->readAll(), "/feed/entry").value<QVariantList>();
        QList<RssVideoModel> videos;

        for (int i = 0; i < list.count(); i++) {
            QVariantMap videoNode = list[i].toMap();
            RssVideoModel video;

            video.title = videoNode["title"].toString();
            video.videoId = videoNode["yt:videoId"].toString();
            QDateTime publishDate = QDateTime::fromString(videoNode["published"].toString(),
                    Qt::ISODate);
            publishDate.setTimeSpec(Qt::UTC);
            video.published = publishDate.toTime_t();

            videos.append(video);
        }
        mutex.lock();
        videosFromRss.insert(reply->request().url().queryItemValue("channel_id"), videos);
        mutex.unlock();
    }

    reply->deleteLater();

    if (channelsRead == channelsToUpdate) {
        buildFeed();
    }
}

void ChannelListPage::buildFeed()
{
    UpdatableDataModel<ChannelListItemModel*> *dataModel =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();
    QList<VideoListItemModel *> feed;
    int feedLimit = QDateTime::currentDateTimeUtc().addDays(-90).toTime_t();
    int now = QDateTime::currentDateTimeUtc().toTime_t();

    DbHelper::transaction();

    for (int i = 0; i < dataModel->size(); i++) {
        ChannelListItemModel *itemToUpdate = dataModel->value(i);

        if (videosFromRss.contains(itemToUpdate->channelId)) {
            QList<RssVideoModel> videos = videosFromRss[itemToUpdate->channelId];
            feedOption->setEnabled(true);

            if (videos.count() > 0) {
                RssVideoModel lastVideo = videos[0];

                itemToUpdate->lastVideoId = lastVideo.videoId;
                itemToUpdate->lastVideoTitle = lastVideo.title;
                itemToUpdate->dateLastActivity = lastVideo.published;

                DbHelper::updateChannelActivityInfo(itemToUpdate);

                for (int j = 0; j < videos.count(); j++) {
                    if (videos[j].published >= feedLimit) {
                        VideoListItemModel *video = new VideoListItemModel();
                        QStringList subtitleParts;

                        video->setTitle(videos[j].title);
                        subtitleParts.append(itemToUpdate->title);
                        subtitleParts.append(DateTimeUtils::timeAgo(now - videos[j].published));
                        video->subtitle = subtitleParts.join(" • ");
                        video->id = videos[j].videoId;
                        video->type = VideoListItemModel::Video;
                        video->channelId = itemToUpdate->channelId;
                        video->channelTitle = itemToUpdate->title;
                        video->published = videos[j].published;
                        video->isFavorite = PlaylistVideoProxy::getInstance()->contains(video->id,
                                PlaylistListItemModel::Favorites);
                        video->isWatchLater = PlaylistVideoProxy::getInstance()->contains(video->id,
                                PlaylistListItemModel::WatchLater);
                        video->isHistory = PlaylistVideoProxy::getInstance()->contains(video->id,
                                PlaylistListItemModel::History);

                        feed.append(video);
                    }
                }
            }
        }
    }

    DbHelper::commit();
    UpdatableDataModel<ChannelListItemModel*> *model =
            new UpdatableDataModel<ChannelListItemModel*>(DbHelper::getChannelList());
    channelList->setDataModel(model);

    qSort(feed.begin(), feed.end(), ChannelListPage::feedComparator);
    feedList->setDataModel(new UpdatableDataModel<VideoListItemModel*>(feed));

    refreshActionItem->setEnabled(true);
    overlay->setVisible(false);
    sortControls->setVisible(false);

    QObject::disconnect(sortByDropdown, SIGNAL(selectedIndexChanged(int)), 0, 0);
    QObject::disconnect(sortOrderDropdown, SIGNAL(selectedIndexChanged(int)), 0, 0);
    sortByDropdown->setSelectedIndex((int) ChannelListItemModel::LastActivity);
    sortOrderDropdown->setSelectedIndex((int) ChannelListItemModel::Desc);
    QObject::connect(sortByDropdown, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSortDropdownSelectedIndexChanged(int)));
    QObject::connect(sortOrderDropdown, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSortDropdownSelectedIndexChanged(int)));
}

void ChannelListPage::onSegmentedSelectedIndexChanged(int index)
{
    channelsSegmentContainer->setVisible(channelsOption->isSelected());
    feedSegmentContainer->setVisible(feedOption->isSelected());
}

void ChannelListPage::updateTitle()
{
    UpdatableDataModel<ChannelListItemModel*> *model =
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel();

    setTitle(QString("Channels (%1)").arg(model->size()));
}

bool ChannelListPage::feedComparator(VideoListItemModel *item1, VideoListItemModel *item2)
{
    return item1->published > item2->published;
}

void ChannelListPage::onChannelActionItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = feedList->dataModel()->data(indexPath).value<VideoListItemModel*>();
    feedList->setEnabled(false);
    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void ChannelListPage::onFeedListItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = feedList->dataModel()->data(indexPath).value<VideoListItemModel*>();
    feedList->setEnabled(false);
    overlay->setVisible(true);
    youtubeClient->process("https://www.youtube.com/watch?v=" + item->id);
}

void ChannelListPage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    BasePage::onMetadataReceived(videoMetadata, storageData);
    feedList->setEnabled(true);
}

void ChannelListPage::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    audioOnly = true;
    onFeedListItemClick(indexPath);
}

void ChannelListPage::onSortActionItemClick()
{
    sortControls->setVisible(!sortControls->isVisible());
    channelsOption->setSelected(true);
}

void ChannelListPage::onSortDropdownSelectedIndexChanged(int index)
{
    sortControls->setVisible(false);

    UpdatableDataModel<ChannelListItemModel*> *model =
            new UpdatableDataModel<ChannelListItemModel*>(
                    DbHelper::getChannelList(
                            (ChannelListItemModel::SortBy) sortByDropdown->selectedValue().toInt(),
                            (ChannelListItemModel::SortDirection) sortOrderDropdown->selectedValue().toInt()));
    channelList->setDataModel(model);
}

void ChannelListPage::onSearchActionItemClick()
{
    SearchChannelsSheet *sheet = new SearchChannelsSheet(
            (UpdatableDataModel<ChannelListItemModel*> *) channelList->dataModel(),
            (UpdatableDataModel<VideoListItemModel*> *) feedList->dataModel(), this);
}

void ChannelListPage::playVideo(QString videoId)
{
    overlay->setVisible(true);
    youtubeClient->process("https://www.youtube.com/watch?v=" + videoId);
}

void ChannelListPage::playAudioOnly(QString videoId)
{
    audioOnly = true;
    playVideo(videoId);
}

void ChannelListPage::openChannel(QString channelId)
{
    overlay->setVisible(true);
    youtubeClient->channel(channelId);
}
