#include "src/models/VideoListItemModel.hpp"
#include "src/applicationui.hpp"
#include "SearchListItemProvider.hpp"
#include "src/utils/ActionItemService.hpp"
#include "src/db/DbHelper.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/utils/ChannelListProxy.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Button>
#include <bb/cascades/Color>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/LabelAutoSizeProperties>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/Image>
#include <bb/cascades/Divider>
#include <bb/cascades/ScalingMethod>
#include <bb/cascades/UIConfig>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/CustomListItem>
#include <bb/cascades/ActionSet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/InvokeQuery>
#include <bb/cascades/UIPalette>

using namespace bb::cascades;

SearchListItem::SearchListItem(CustomListView *listView, const SearchListItemProvider *provider,
        SearchListItemActionSetBuilder *actionSetBuilder, Container *parent) :
        listView(listView), provider(provider), actionSetBuilder(actionSetBuilder), CustomListItem(
                parent)
{
    Container *rootContainer = new Container();
    UIConfig *ui = rootContainer->ui();
    rootContainer->setLayout(new DockLayout());
    rootContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    rootContainer->setVerticalAlignment(VerticalAlignment::Fill);

    itemContainer = new Container();
    StackLayout *contentLayout = new StackLayout();
    contentLayout->setOrientation(LayoutOrientation::LeftToRight);
    itemContainer->setLayout(contentLayout);

    Container *leftContainer = new Container();
    leftContainer->setLeftPadding(ui->du(1));
    leftContainer->setTopPadding(ui->du(1));
    leftContainer->setRightPadding(ui->du(1));
    leftContainer->setBottomPadding(ui->du(1));
    thumbnailContainer = new Container();
    thumbnailContainer->setLayout(new DockLayout());
    thumbnailContainer->setPreferredWidth(180);
    thumbnailContainer->setPreferredHeight(135);
    thumbnailContainer->setMinWidth(180);
    thumbnailContainer->setMinHeight(135);
    thumbnail = ImageView::create();
    thumbnail->setScalingMethod(ScalingMethod::AspectFill);
    thumbnail->setHorizontalAlignment(HorizontalAlignment::Fill);
    thumbnail->setVerticalAlignment(VerticalAlignment::Fill);

    Container *durationParentContainer = new Container();
    durationParentContainer->setLayout(new DockLayout());
    durationParentContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    durationParentContainer->setVerticalAlignment(VerticalAlignment::Fill);
    durationParentContainer->setBottomPadding(ui->du(0.5));
    durationParentContainer->setRightPadding(ui->du(0.5));

    durationContainer = new Container();
    duration = Label::create();
    duration->textStyle()->setColor(Color::White);
    durationContainer->setBackground(Color::Black);
    durationContainer->setHorizontalAlignment(HorizontalAlignment::Right);
    durationContainer->setVerticalAlignment(VerticalAlignment::Bottom);
    durationContainer->add(duration);

    durationParentContainer->add(durationContainer);
    thumbnailContainer->add(thumbnail);
    thumbnailContainer->add(durationParentContainer);
    leftContainer->add(thumbnailContainer);
    viewedPercentContainer = new Container();
    viewedPercentContainer->setPreferredHeight(ui->du(1));
    viewedPercentContainer->setBackground(ui->palette()->primary());
    leftContainer->add(viewedPercentContainer);
    itemContainer->add(leftContainer);

    Container *textAndIconsContainer = new Container();
    textAndIconsContainer->setVerticalAlignment(VerticalAlignment::Fill);
    textAndIconsContainer->setLayout(
            StackLayout::create().orientation(LayoutOrientation::LeftToRight));
    Container *textContainer = new Container();
    textContainer->setLeftMargin(ui->du(1));
    textContainer->setLeftPadding(ui->du(1));
    textContainer->setTopPadding(ui->du(1));
    textContainer->setRightPadding(ui->du(1));
    textContainer->setBottomPadding(ui->du(1));
    textContainer->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1));
    title = Label::create().text(" ");
    title->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    title->setMultiline(true);
    title->autoSize()->setMaxLineCount(2);
    textContainer->add(title);
    subtitle = Label::create().text(" ");
    subtitle->textStyle()->setBase(SystemDefaults::TextStyles::subtitleText());
    subtitle->setTopMargin(0);
    textContainer->add(subtitle);
    textAndIconsContainer->add(textContainer);

    Container *iconsContainer = new Container;
    iconsContainer->setMinWidth(32);
    iconsContainer->setMaxWidth(32);
    iconsContainer->setTopPadding(ui->du(1));
    iconsContainer->setBottomPadding(ui->du(1));
    iconsContainer->setVerticalAlignment(VerticalAlignment::Fill);
    favoriteIcon = ImageView::create().imageSource(QString("asset:///images/ic_favorite.png"));
    favoriteIcon->setScalingMethod(ScalingMethod::AspectFit);
    favoriteIcon->setBottomMargin(16);
    iconsContainer->add(favoriteIcon);
    watchLaterIcon = ImageView::create().imageSource(QString("asset:///images/ic_later.png"));
    watchLaterIcon->setScalingMethod(ScalingMethod::AspectFit);
    watchLaterIcon->setBottomMargin(16);
    iconsContainer->add(watchLaterIcon);
    historyIcon = ImageView::create().imageSource(QString("asset:///images/ic_done.png"));
    historyIcon->setScalingMethod(ScalingMethod::AspectFit);
    iconsContainer->add(historyIcon);
    textAndIconsContainer->add(iconsContainer);

    itemContainer->add(textAndIconsContainer);
    rootContainer->add(itemContainer);

    showMoreContainer = new Container();
    showMoreContainer->setHorizontalAlignment(HorizontalAlignment::Center);
    showMoreContainer->setVerticalAlignment(VerticalAlignment::Center);
    showMoreContainer->setTopPadding(ui->du(2));
    showMoreContainer->setBottomPadding(ui->du(2));
    showMoreContainer->setLeftPadding(ui->du(5));
    showMoreContainer->setRightPadding(ui->du(5));
    showMoreContainer->setVisible(false);
    Button *showMoreButton = Button::create().text("Show More");
    showMoreContainer->add(showMoreButton);
    rootContainer->add(showMoreContainer);

    setDividerVisible(true);
    setContent(rootContainer);

    QObject::connect(showMoreButton, SIGNAL(clicked()), this, SLOT(onShowMoreClick()));

    actionSet = ActionSet::create().parent(this);
    this->addActionSet(actionSet);

    playAudioOnlyActionItem = ActionItemService::playAudioOnlyActionItem(this);
    continueActionItem = ActionItemService::continueActionItem(this);
    channelActionItem = ActionItemService::channelActionItem(this);
    addToChannelsActionItem = ActionItemService::addToChannelsActionItem(this);
    addToFavoritesActionItem = ActionItemService::addToFavoritesActionItem(this);
    addToWatchLaterActionItem = ActionItemService::addToWatchLaterActionItem(this);
    shareVideoActionItem = ActionItemService::shareVideoActionItem(this);
    copyVideoLinkActionItem = ActionItemService::copyVideoLinkActionItem(this);
    openVideoInBrowserActionItem = ActionItemService::openVideoInBrowserActionItem(this);

    copyChannelLinkActionItem = ActionItemService::copyChannelLinkActionItem(this);
    copyRssLinkActionItem = ActionItemService::copyRssActionItem(this);
    openChannelInBrowserActionItem = ActionItemService::openChannelInBrowserActionItem(this);
    shareChannelActionItem = ActionItemService::shareChannelActionItem(this);

    QObject::connect(channelActionItem, SIGNAL(triggered()), this,
            SLOT(onChannelActionItemClick()));
    QObject::connect(addToChannelsActionItem, SIGNAL(triggered()), this,
            SLOT(onAddToChannelsActionItemClick()));
    QObject::connect(addToFavoritesActionItem, SIGNAL(triggered()), this,
            SLOT(onAddToFavoritesActionItemClick()));
    QObject::connect(addToWatchLaterActionItem, SIGNAL(triggered()), this,
            SLOT(onAddToWatchLaterActionItemClick()));
    QObject::connect(copyChannelLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyChannelLinkActionItemClick()));
    QObject::connect(copyRssLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyRssLinkActionItemClick()));
    QObject::connect(copyVideoLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyVideoLinkActionItemClick()));
    QObject::connect(openVideoInBrowserActionItem, SIGNAL(triggered()), this,
            SLOT(onOpenVideoInBrowserActionItemClick()));
    QObject::connect(playAudioOnlyActionItem, SIGNAL(triggered()), this,
            SLOT(onPlayAudioOnlyActionItemClick()));
    QObject::connect(continueActionItem, SIGNAL(triggered()), this,
            SLOT(onContinueActionItemClick()));
    QObject::connect(openChannelInBrowserActionItem, SIGNAL(triggered()), this,
            SLOT(onOpenChannelInBrowserActionItemClick()));
}

void SearchListItem::updateItem(const VideoListItemModel *item, QVariantList indexPath)
{
    this->indexPath = indexPath;

    this->removeActionSet(actionSet);
    actionSet = ActionSet::create().parent(this);
    this->addActionSet(actionSet);
    actionSet->setTitle(item->title());
    actionSetBuilder->buildActionSet(item, this);

    if (item->type == VideoListItemModel::ShowMore) {
        itemContainer->setVisible(false);
        showMoreContainer->setVisible(true);

        return;
    }

    itemContainer->setVisible(true);
    showMoreContainer->setVisible(false);
    viewedPercentContainer->setVisible(false);
    favoriteIcon->setVisible(item->isFavorite);
    watchLaterIcon->setVisible(item->isWatchLater);
    historyIcon->setVisible(item->isHistory);
    title->setText(item->title());
    subtitle->setText(item->subtitle);

    if (item->type == VideoListItemModel::Video) {
        duration->setVisible(true);

        if (item->isLiveStream()) {
            durationContainer->setBackground(Color::Red);
            duration->setText("LIVE");
        } else if (item->lengthText != "") {
            durationContainer->setBackground(Color::Black);
            duration->setText(item->lengthText);
        } else {
            duration->setVisible(false);
            duration->setText("");
        }

        int viewedPercent = VideoViewedPercentProxy::getInstance()->getPercent(item->id);
        if (viewedPercent > 0) {
            viewedPercentContainer->setVisible(true);
            viewedPercentContainer->setPreferredWidth(
                    (0.0f + thumbnailContainer->preferredWidth() * viewedPercent) / 10000);
        }
    } else {
        duration->setVisible(false);
        duration->setText("");
    }

    if (item->type == VideoListItemModel::Channel && item->thumbnailUrl == "") {
        thumbnail->setImageSource(QString("asset:///images/ic_contact.png"));
    } else {
        QString url;

        if (item->type == VideoListItemModel::Video) {
            url = "https://i.ytimg.com/vi/" + item->id + "/default.jpg";
        } else if (item->type == VideoListItemModel::Channel && item->thumbnailUrl != "") {
            if (!item->thumbnailUrl.startsWith("http")) {
                url = "https:" + item->thumbnailUrl;
            } else {
                url = item->thumbnailUrl;
            }
        }

        if (!provider->imagesCache.contains(url)) {
            QEventLoop loop;
            QNetworkRequest request(url);
            QNetworkReply *reply = ApplicationUI::networkManager->get(request);
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            if (!reply->error()) {
                QByteArray image = reply->readAll();
                ((SearchListItemProvider*) provider)->imagesCache.insert(url, image);
                thumbnail->setImage(Image(image));

                if (provider->imagesCache.count() > 100) { // clean cache after 100 images have been saved
                    ((SearchListItemProvider*) provider)->imagesCache.clear();
                }
            }
            reply->deleteLater();
        } else {
            thumbnail->setImage(Image(provider->imagesCache[url]));
        }
    }
}

void SearchListItem::select(bool select)
{
}

void SearchListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);

    select(selected);
}

void SearchListItem::activate(bool activate)
{
    select(activate);
}

void SearchListItem::onShowMoreClick()
{
    emit showMore(indexPath);
}

void SearchListItem::onChannelActionItemClick()
{
    emit channelActionItemClick(indexPath);
}

void SearchListItem::onAddToChannelsActionItemClick()
{
    VideoListItemModel *item = getModelByPath();
    bool isFavorite = !item->isFavorite;

    if (isFavorite) {
        ChannelListItemModel channel(item->id, item->title(), item->thumbnailUrl,
                QDateTime::currentDateTimeUtc().toTime_t(), 0, "", "", 0);
        DbHelper::createChannel(&channel);
        ChannelListProxy::getInstance()->add(&channel);
    } else {
        DbHelper::deleteChannel(item->id);
        ChannelListProxy::getInstance()->deleteById(item->id);
    }

    UIUtils::toastInfo(isFavorite ? "Added to Channels" : "Removed from Channels");
}

void SearchListItem::onAddToFavoritesActionItemClick()
{
    VideoListItemModel *item = getModelByPath();
    bool isFavorite = !item->isFavorite;

    if (isFavorite) {
        PlaylistVideoModel video(item->id, (int) PlaylistListItemModel::Favorites, item->title(),
                item->channelId, item->channelTitle, QDateTime::currentDateTimeUtc().toTime_t(), 0,
                0);
        DbHelper::createPlaylistVideo(&video);
        PlaylistVideoProxy::getInstance()->add(&video);
    } else {
        DbHelper::deletePlaylistVideo(item->id, (int) PlaylistListItemModel::Favorites);
        PlaylistVideoProxy::getInstance()->deleteById(item->id, PlaylistListItemModel::Favorites);
    }

    UIUtils::toastInfo(isFavorite ? "Added to Favorites" : "Removed from Favorites");
}

void SearchListItem::onAddToWatchLaterActionItemClick()
{
    VideoListItemModel *item = getModelByPath();
    bool isWatchLater = !item->isWatchLater;

    if (isWatchLater) {
        PlaylistVideoModel video(item->id, (int) PlaylistListItemModel::WatchLater, item->title(),
                item->channelId, item->channelTitle, QDateTime::currentDateTimeUtc().toTime_t(), 0,
                0);
        DbHelper::createPlaylistVideo(&video);
        PlaylistVideoProxy::getInstance()->add(&video);
    } else {
        DbHelper::deletePlaylistVideo(item->id, (int) PlaylistListItemModel::WatchLater);
        PlaylistVideoProxy::getInstance()->deleteById(item->id, PlaylistListItemModel::WatchLater);
    }

    UIUtils::toastInfo(isWatchLater ? "Added to Watch Later" : "Removed from Watch Later");
}

void SearchListItem::addChannelActionItem()
{
    actionSet->add(channelActionItem);
}

void SearchListItem::addAddToChannelsActionItem()
{
    VideoListItemModel *item = getModelByPath();
    ActionItemService::updateAddToChannelsActionItem(addToChannelsActionItem, item->isFavorite);

    actionSet->add(addToChannelsActionItem);
}

void SearchListItem::addAddToFavoritesActionItem()
{
    VideoListItemModel *item = getModelByPath();
    ActionItemService::updateAddToFavoritesActionItem(addToFavoritesActionItem, item->isFavorite);

    actionSet->add(addToFavoritesActionItem);
}

void SearchListItem::addAddToWatchLaterActionItem()
{
    VideoListItemModel *item = getModelByPath();
    ActionItemService::updateAddToWatchLaterActionItem(addToWatchLaterActionItem,
            item->isWatchLater);

    actionSet->add(addToWatchLaterActionItem);
}

void SearchListItem::addCopyChannelLinkActionItem()
{
    actionSet->add(copyChannelLinkActionItem);
}

void SearchListItem::addCopyRssLinkActionItem()
{
    actionSet->add(copyRssLinkActionItem);
}

void SearchListItem::addOpenChannelInBrowserActionItem()
{
    actionSet->add(openChannelInBrowserActionItem);
}

void SearchListItem::addShareChannelActionItem()
{
    VideoListItemModel *item = getModelByPath();

    actionSet->add(shareChannelActionItem);
    ActionItemService::setShareChannelText(shareChannelActionItem, item->title(), item->id);
}

void SearchListItem::addPlayAudioOnlyActionItem()
{
    actionSet->add(playAudioOnlyActionItem);
}

void SearchListItem::addContinueActionItem()
{
    VideoListItemModel *item = getModelByPath();

    int viewedProgress = VideoViewedPercentProxy::getInstance()->getPercent(item->id);
    if (viewedProgress > 0 && viewedProgress < 10000) {
        actionSet->add(continueActionItem);
    }
}

void SearchListItem::addCopyVideoLinkActionItem()
{
    actionSet->add(copyVideoLinkActionItem);
}

void SearchListItem::addOpenVideoInBrowserActionItem()
{
    actionSet->add(openVideoInBrowserActionItem);
}

void SearchListItem::addShareVideoActionItem()
{
    VideoListItemModel *item = getModelByPath();

    actionSet->add(shareVideoActionItem);
    ActionItemService::setShareVideoText(shareVideoActionItem, item->title(), item->id);
}

void SearchListItem::onCopyChannelLinkActionItemClick()
{
    ActionItemService::copyChannelLink(getModelByPath()->id);
}

void SearchListItem::onCopyRssLinkActionItemClick()
{
    ActionItemService::copyRss(getModelByPath()->id);
}

void SearchListItem::onOpenChannelInBrowserActionItemClick()
{
    ActionItemService::openChannelInBrowser(getModelByPath()->id);
}

void SearchListItem::onCopyVideoLinkActionItemClick()
{
    ActionItemService::copyVideoLink(getModelByPath()->id);
}

void SearchListItem::onOpenVideoInBrowserActionItemClick()
{
    ActionItemService::openVideoInBrowser(getModelByPath()->id);
}

void SearchListItem::onPlayAudioOnlyActionItemClick()
{
    emit playAudioOnlyActionItemClick(indexPath);
}

void SearchListItem::onContinueActionItemClick()
{
    ApplicationUI::playerContext->setContinuePlaying(true);
    listView->trigger(indexPath);
}

VideoListItemModel* SearchListItem::getModelByPath()
{
    if (indexPath.count() == 1) {
        return listView->dataModel()->data(indexPath).value<VideoListItemModel*>();
    }

    return qobject_cast<VideoListItemModel *>(
            listView->dataModel()->data(indexPath).value<QObject*>());
}
