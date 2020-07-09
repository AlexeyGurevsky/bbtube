#include "src/models/ChannelListItemModel.hpp"
#include "src/applicationui.hpp"
#include "ChannelListItemProvider.hpp"
#include "src/utils/ActionItemService.hpp"
#include "src/utils/DateTimeUtils.hpp"

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

ChannelListItem::ChannelListItem(ListView *listView, const ChannelListItemProvider *provider,
        Container *parent) :
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

    thumbnailContainer = new Container();
    thumbnailContainer->setLayout(new DockLayout());
    thumbnailContainer->setPreferredWidth(180);
    thumbnailContainer->setPreferredHeight(135);
    thumbnailContainer->setMinWidth(180);
    thumbnailContainer->setMinHeight(135);
    thumbnailContainer->setLeftPadding(ui->du(1));
    thumbnailContainer->setTopPadding(ui->du(1));
    thumbnailContainer->setRightPadding(ui->du(1));
    thumbnailContainer->setBottomPadding(ui->du(1));
    thumbnail = ImageView::create();
    thumbnail->setScalingMethod(ScalingMethod::AspectFill);
    thumbnail->setHorizontalAlignment(HorizontalAlignment::Fill);
    thumbnail->setVerticalAlignment(VerticalAlignment::Fill);
    thumbnailContainer->add(thumbnail);
    itemContainer->add(thumbnailContainer);

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
    textContainer->add(title);
    textContainer->add(subtitle);
    itemContainer->add(textContainer);
    unseenContainer = Container::create().background(ui->palette()->primary()).opacity(0.7);
    unseenContainer->setMinWidth(ui->du(1));
    unseenContainer->setVerticalAlignment(VerticalAlignment::Fill);
    itemContainer->add(unseenContainer);
    rootContainer->add(itemContainer);

    setDividerVisible(true);
    setContent(rootContainer);

    actionSet = ActionSet::create().parent(this);
    this->addActionSet(actionSet);

    removeFromChannelsActionItem = ActionItemService::addToChannelsActionItem(this);
    ActionItemService::updateAddToChannelsActionItem(removeFromChannelsActionItem, true);
    copyChannelLinkActionItem = ActionItemService::copyChannelLinkActionItem(this);
    copyRssLinkActionItem = ActionItemService::copyRssActionItem(this);
    openChannelInBrowserActionItem = ActionItemService::openChannelInBrowserActionItem(this);
    shareChannelActionItem = ActionItemService::shareChannelActionItem(this);
    actionSet->add(removeFromChannelsActionItem);
    actionSet->add(shareChannelActionItem);
    actionSet->add(openChannelInBrowserActionItem);
    actionSet->add(copyChannelLinkActionItem);
    actionSet->add(copyRssLinkActionItem);

    QObject::connect(removeFromChannelsActionItem, SIGNAL(triggered()), this,
            SLOT(onRemoveFromChannelsActionItemClick()));
    QObject::connect(copyChannelLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyChannelLinkActionItemClick()));
    QObject::connect(copyRssLinkActionItem, SIGNAL(triggered()), this,
            SLOT(onCopyRssLinkActionItemClick()));
    QObject::connect(openChannelInBrowserActionItem, SIGNAL(triggered()), this,
            SLOT(onOpenChannelInBrowserActionItemClick()));
}

void ChannelListItem::updateItem(const ChannelListItemModel *item, QVariantList indexPath)
{
    this->indexPath = indexPath;
    bool isRead = (item->dateLastVisited >= item->dateLastActivity);
    int nowTime = QDateTime::currentDateTimeUtc().toTime_t();

    ActionItemService::setShareChannelText(shareChannelActionItem, item->title, item->channelId);
    actionSet->setTitle(item->title);
    title->setText(item->title);
    subtitle->setText(
            item->dateLastActivity > 0 ?
                    DateTimeUtils::timeAgo(nowTime - item->dateLastActivity) + " • " + item->lastVideoTitle :
                    "Last video: N/A");
    unseenContainer->setVisible(!isRead);
    title->textStyle()->setFontWeight(isRead ? FontWeight::Normal : FontWeight::Bold);

    if (item->thumbnailUrl != "") {
        QString url;

        if (!item->thumbnailUrl.startsWith("http")) {
            url = "https:" + item->thumbnailUrl;
        } else {
            url = item->thumbnailUrl;
        }

        if (!provider->imagesCache.contains(url)) {
            QEventLoop loop;
            QNetworkRequest request(url);
            QNetworkReply *reply = ApplicationUI::networkManager->get(request);
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            if (!reply->error()) {
                QByteArray image = reply->readAll();
                ((ChannelListItemProvider*) provider)->imagesCache.insert(url, image);
                thumbnail->setImage(Image(image));

                if (provider->imagesCache.count() > 100) { // clean cache after 100 images have been saved
                    ((ChannelListItemProvider*) provider)->imagesCache.clear();
                }
            }
            reply->deleteLater();
        } else {
            thumbnail->setImage(Image(provider->imagesCache[url]));
        }
    } else {
        thumbnail->setImageSource(QString("asset:///images/ic_contact.png"));
    }
}

void ChannelListItem::select(bool select)
{
}

void ChannelListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);

    select(selected);
}

void ChannelListItem::activate(bool activate)
{
    select(activate);
}

void ChannelListItem::onRemoveFromChannelsActionItemClick()
{
    emit removeFromChannelsActionItemClick(indexPath);
}

void ChannelListItem::onCopyChannelLinkActionItemClick()
{
    ActionItemService::copyChannelLink(getModelByPath()->channelId);
}

void ChannelListItem::onCopyRssLinkActionItemClick()
{
    ActionItemService::copyRss(getModelByPath()->channelId);
}

void ChannelListItem::onOpenChannelInBrowserActionItemClick()
{
    ActionItemService::openChannelInBrowser(getModelByPath()->channelId);
}

ChannelListItemModel* ChannelListItem::getModelByPath()
{
    return listView->dataModel()->data(indexPath).value<ChannelListItemModel*>();
}
