#include "src/models/PlaylistVideoModel.hpp"
#include "src/applicationui.hpp"
#include "PlaylistVideoListItemProvider.hpp"
#include "src/utils/ActionItemService.hpp"
#include "src/utils/DateTimeUtils.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/LabelAutoSizeProperties>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/Image>
#include <bb/cascades/ScalingMethod>
#include <bb/cascades/UIConfig>
#include <bb/cascades/UIPalette>
#include <bb/cascades/CustomListItem>
#include <bb/cascades/ActionSet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/InvokeQuery>
#include <bb/cascades/FontWeight>
#include <bb/cascades/Color>

using namespace bb::cascades;

PlaylistVideoListItem::PlaylistVideoListItem(CustomListView *listView,
        const PlaylistVideoListItemProvider *provider, Container *parent) :
        listView(listView), provider(provider), CustomListItem(parent)
{
    Container *rootContainer = new Container();
    UIConfig *ui = rootContainer->ui();
    rootContainer->setLayout(new DockLayout());
    rootContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    rootContainer->setVerticalAlignment(VerticalAlignment::Fill);

    itemContainer = Container::create();
    itemContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    itemContainer->setVerticalAlignment(VerticalAlignment::Fill);
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
    thumbnailContainer->add(thumbnail);
    leftContainer->add(thumbnailContainer);
    viewedPercentContainer = new Container();
    viewedPercentContainer->setPreferredHeight(ui->du(1));
    viewedPercentContainer->setBackground(ui->palette()->primary());
    leftContainer->add(viewedPercentContainer);
    itemContainer->add(leftContainer);

    Container *textContainer = Container::create().layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    textContainer->setLeftMargin(ui->du(1));
    textContainer->setLeftPadding(ui->du(1));
    textContainer->setTopPadding(ui->du(1));
    textContainer->setRightPadding(ui->du(1));
    textContainer->setBottomPadding(ui->du(1));

    title = Label::create().text(" ");
    title->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    title->setMultiline(true);
    title->autoSize()->setMaxLineCount(2);
    subtitle = Label::create().text(" ");
    subtitle->textStyle()->setBase(SystemDefaults::TextStyles::subtitleText());
    subtitle->setTopMargin(0);
    subtitle->autoSize()->setMaxLineCount(2);
    textContainer->add(title);
    textContainer->add(subtitle);
    itemContainer->add(textContainer);
    onAirContainer = Container::create().background(ui->palette()->primary()).opacity(0.7);
    onAirContainer->setMinWidth(ui->du(1));
    onAirContainer->setVerticalAlignment(VerticalAlignment::Fill);
    itemContainer->add(onAirContainer);
    rootContainer->add(itemContainer);

    setDividerVisible(true);
    setContent(rootContainer);

    actionSet = ActionSet::create().parent(this);
    this->addActionSet(actionSet);

    continueActionItem = ActionItemService::continueActionItem(this);
    channelActionItem = ActionItemService::channelActionItem(this);
    copyVideoLinkActionItem = ActionItemService::copyVideoLinkActionItem(this);
    openVideoInBrowserActionItem = ActionItemService::openVideoInBrowserActionItem(this);
    playAudioOnlyActionItem = ActionItemService::playAudioOnlyActionItem(this);
    shareVideoActionItem = ActionItemService::shareVideoActionItem(this);
    deleteActionItem = ActionItemService::deleteActionItem(this);

    actionSet->add(continueActionItem);
    actionSet->add(playAudioOnlyActionItem);
    actionSet->add(channelActionItem);
    actionSet->add(shareVideoActionItem);
    actionSet->add(openVideoInBrowserActionItem);
    actionSet->add(copyVideoLinkActionItem);
    actionSet->add(deleteActionItem);

    QObject::connect(channelActionItem, SIGNAL(triggered()), this,
            SLOT(onChannelActionItemClick()));
    QObject::connect(copyVideoLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyVideoLinkActionItemClick()));
    QObject::connect(openVideoInBrowserActionItem, SIGNAL(triggered()), this,
            SLOT(onOpenVideoInBrowserActionItemClick()));
    QObject::connect(continueActionItem, SIGNAL(triggered()), this,
                SLOT(onContinueActionItemClick()));
    QObject::connect(playAudioOnlyActionItem, SIGNAL(triggered()), this,
            SLOT(onPlayAudioOnlyActionItemClick()));
    QObject::connect(deleteActionItem, SIGNAL(triggered()), this, SLOT(onDeleteActionItemClick()));
}

void PlaylistVideoListItem::updateItem(const PlaylistVideoModel *item, QVariantList indexPath)
{
    this->indexPath = indexPath;
    int nowTime = QDateTime::currentDateTimeUtc().toTime_t();

    ActionItemService::setShareVideoText(shareVideoActionItem, item->title, item->videoId);
    actionSet->setTitle(item->title);
    title->setText(item->title);

    QStringList subtitleParts;
    if (item->channelTitle != "") {
        subtitleParts.append(item->channelTitle);
    }
    subtitleParts.append("added " + DateTimeUtils::timeAgo(nowTime - item->dateAdded));
    if (item->dateLastPlayed > 0) {
        subtitleParts.append("viewed " + DateTimeUtils::timeAgo(nowTime - item->dateLastPlayed));
    }
    subtitle->setText(subtitleParts.join(" • "));

    onAirContainer->setVisible(item->onAir);
    title->textStyle()->setFontWeight(!item->onAir ? FontWeight::Normal : FontWeight::Bold);

    continueActionItem->setEnabled(false);
    int viewedPercent = VideoViewedPercentProxy::getInstance()->getPercent(item->videoId);
    if (viewedPercent > 0) {
        viewedPercentContainer->setVisible(true);
        viewedPercentContainer->setPreferredWidth(
                (0.0f + thumbnailContainer->preferredWidth() * viewedPercent) / 10000);

        if (viewedPercent > 0 && viewedPercent < 10000) {
            continueActionItem->setEnabled(true);
        }
    } else {
        viewedPercentContainer->setVisible(false);
    }

    QString url = "https://i.ytimg.com/vi/" + item->videoId + "/default.jpg";

    if (!provider->imagesCache.contains(url)) {
        QEventLoop loop;
        QNetworkRequest request(url);
        QNetworkReply *reply = ApplicationUI::networkManager->get(request);
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        if (!reply->error()) {
            QByteArray image = reply->readAll();
            ((PlaylistVideoListItemProvider*) provider)->imagesCache.insert(url, image);
            thumbnail->setImage(Image(image));

            if (provider->imagesCache.count() > 100) { // clean cache after 100 images have been saved
                ((PlaylistVideoListItemProvider*) provider)->imagesCache.clear();
            }
        }
        reply->deleteLater();
    } else {
        thumbnail->setImage(Image(provider->imagesCache[url]));
    }
}

void PlaylistVideoListItem::select(bool select)
{
}

void PlaylistVideoListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);

    select(selected);
}

void PlaylistVideoListItem::activate(bool activate)
{
    select(activate);
}

void PlaylistVideoListItem::onCopyVideoLinkActionItemClick()
{
    ActionItemService::copyVideoLink(getModelByPath()->videoId);
}

void PlaylistVideoListItem::onOpenVideoInBrowserActionItemClick()
{
    ActionItemService::openVideoInBrowser(getModelByPath()->videoId);
}

void PlaylistVideoListItem::onPlayAudioOnlyActionItemClick()
{
    emit playAudioOnlyActionItemClick(indexPath);
}

void PlaylistVideoListItem::onChannelActionItemClick()
{
    emit channelActionItemClick(indexPath);
}

void PlaylistVideoListItem::onDeleteActionItemClick()
{
    emit deleteActionItemClick(indexPath);
}

void PlaylistVideoListItem::onContinueActionItemClick()
{
    ApplicationUI::playerContext->setContinuePlaying(true);
    listView->trigger(indexPath);
}

PlaylistVideoModel* PlaylistVideoListItem::getModelByPath()
{
    return listView->dataModel()->data(indexPath).value<PlaylistVideoModel*>();
}
