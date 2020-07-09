#include "ActionItemService.hpp"
#include "src/utils/UIUtils.hpp"

#include <bb/cascades/ActionItem>
#include <bb/system/Clipboard>
#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/InvokeQuery>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>
#include <bb/cascades/Shortcut>

bb::cascades::ActionItem* ActionItemService::playAudioOnlyActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_speaker_dk.png"));
    item->setTitle("Play Audio Only");
    item->addShortcut(bb::cascades::Shortcut::create().key("a"));

    return item;
}

bb::cascades::ActionItem* ActionItemService::continueActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_continue.png"));
    item->setTitle("Continue Playing");
    item->addShortcut(bb::cascades::Shortcut::create().key("c"));

    return item;
}

bb::cascades::InvokeActionItem* ActionItemService::shareVideoActionItem(QObject *parent)
{
    bb::cascades::InvokeQuery *query = new bb::cascades::InvokeQuery(parent);
    query->setInvokeActionId("bb.action.SHARE");
    query->setMimeType("text/plain");

    bb::cascades::InvokeActionItem *item = bb::cascades::InvokeActionItem::create(query).parent(
            parent);

    item->setImageSource(QString("asset:///images/ic_share.png"));
    item->setTitle("Share Link");

    return item;
}

bb::cascades::ActionItem* ActionItemService::copyVideoLinkActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_copy_link.png"));
    item->setTitle("Copy Link");

    return item;
}

bb::cascades::ActionItem* ActionItemService::openVideoInBrowserActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_open_link.png"));
    item->setTitle("Open in Browser");

    return item;
}

bb::cascades::ActionItem* ActionItemService::channelActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_contact.png"));
    item->setTitle("Channel");

    return item;
}

bb::cascades::ActionItem* ActionItemService::copyRssActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_rss.png"));
    item->setTitle("Copy RSS Link");

    return item;
}

bb::cascades::InvokeActionItem* ActionItemService::shareChannelActionItem(QObject *parent)
{
    bb::cascades::InvokeQuery *query = new bb::cascades::InvokeQuery(parent);
    query->setInvokeActionId("bb.action.SHARE");
    query->setMimeType("text/plain");

    bb::cascades::InvokeActionItem *item = bb::cascades::InvokeActionItem::create(query).parent(
            parent);

    item->setImageSource(QString("asset:///images/ic_share.png"));
    item->setTitle("Share Link");

    return item;
}

bb::cascades::ActionItem* ActionItemService::copyChannelLinkActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_copy_link.png"));
    item->setTitle("Copy Link");

    return item;
}

bb::cascades::ActionItem* ActionItemService::openChannelInBrowserActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);
    item->setImageSource(QString("asset:///images/ic_open_link.png"));
    item->setTitle("Open in Browser");

    return item;
}

bb::cascades::ActionItem* ActionItemService::addToChannelsActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);

    return item;
}

bb::cascades::DeleteActionItem* ActionItemService::deleteActionItem(QObject *parent)
{
    bb::cascades::DeleteActionItem *item = bb::cascades::DeleteActionItem::create().parent(parent);

    return item;
}

bb::cascades::ActionItem* ActionItemService::addToFavoritesActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);

    return item;
}

bb::cascades::ActionItem* ActionItemService::addToWatchLaterActionItem(QObject *parent)
{
    bb::cascades::ActionItem *item = bb::cascades::ActionItem::create().parent(parent);

    return item;
}

void ActionItemService::updateAddToChannelsActionItem(bb::cascades::ActionItem* actionItem,
        bool isFavorite)
{
    actionItem->setImageSource(
            QString("asset:///images/ic_") + (isFavorite ? "remove" : "add") + "_bookmarks.png");
    actionItem->setTitle((isFavorite ? "Remove from" : "Add to") + QString(" Channels"));
}

void ActionItemService::updateAddToFavoritesActionItem(bb::cascades::ActionItem* actionItem,
        bool isFavorite)
{
    actionItem->setImageSource(
            QString("asset:///images/ic_") + (isFavorite ? "remove" : "add") + "_bookmarks.png");
    actionItem->setTitle((isFavorite ? "Remove from" : "Add to") + QString(" Favorites"));
}

void ActionItemService::updateAddToWatchLaterActionItem(bb::cascades::ActionItem* actionItem,
        bool isWatchLater)
{
    actionItem->setImageSource(
            QString("asset:///images/ic_%1later.png").arg(isWatchLater ? "remove_" : ""));
    actionItem->setTitle((isWatchLater ? "Remove from" : "Add to") + QString(" Watch Later"));
}

void ActionItemService::copyVideoLink(QString videoId)
{
    bb::system::Clipboard clipboard;
    QString link = "https://youtu.be/" + videoId;
    clipboard.insert(QString("text/plain"), link.toUtf8());

    UIUtils::toastInfo("The link was copied to Clipboard");
}

void ActionItemService::openVideoInBrowser(QString videoId)
{
    bb::system::InvokeRequest request;
    request.setAction("bb.action.OPEN");
    request.setTarget("sys.browser");
    request.setUri("https://youtu.be/" + videoId);

    bb::system::InvokeTargetReply* reply = (new bb::system::InvokeManager)->invoke(request);
    reply->deleteLater();
}

void ActionItemService::openChannelInBrowser(QString channelId)
{
    bb::system::InvokeRequest request;
    request.setAction("bb.action.OPEN");
    request.setTarget("sys.browser");
    request.setUri("https://m.youtube.com/channel/" + channelId);

    bb::system::InvokeTargetReply* reply = (new bb::system::InvokeManager)->invoke(request);
    reply->deleteLater();
}

void ActionItemService::copyRss(QString channelId)
{
    bb::system::Clipboard clipboard;
    QString link = "https://www.youtube.com/feeds/videos.xml?channel_id=" + channelId;
    clipboard.insert(QString("text/plain"), link.toUtf8());

    UIUtils::toastInfo("The link was copied to Clipboard");
}

void ActionItemService::copyChannelLink(QString channelId)
{
    bb::system::Clipboard clipboard;
    QString link = "https://m.youtube.com/channel/" + channelId;
    clipboard.insert(QString("text/plain"), link.toUtf8());

    UIUtils::toastInfo("The link was copied to Clipboard");
}

void ActionItemService::setShareChannelText(bb::cascades::InvokeActionItem* actionItem,
        QString title, QString channelId)
{
    actionItem->setData(QString(title + "\nhttps://m.youtube.com/channel/" + channelId).toUtf8());
}

void ActionItemService::setShareVideoText(bb::cascades::InvokeActionItem* actionItem, QString title,
        QString videoId)
{
    actionItem->setData(QString(title + "\nhttps://youtu.be/" + videoId).toUtf8());
}
