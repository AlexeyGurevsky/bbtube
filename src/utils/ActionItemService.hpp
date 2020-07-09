#ifndef ACTIONITEMSERVICE_HPP_
#define ACTIONITEMSERVICE_HPP_

#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/DeleteActionItem>

class ActionItemService
{
public:
    static bb::cascades::ActionItem* playAudioOnlyActionItem(QObject *parent);
    static bb::cascades::ActionItem* continueActionItem(QObject *parent);
    static bb::cascades::InvokeActionItem* shareVideoActionItem(QObject *parent);
    static bb::cascades::ActionItem* copyVideoLinkActionItem(QObject *parent);
    static bb::cascades::ActionItem* openVideoInBrowserActionItem(QObject *parent);
    static bb::cascades::ActionItem* channelActionItem(QObject *parent);
    static bb::cascades::ActionItem* copyRssActionItem(QObject *parent);
    static bb::cascades::InvokeActionItem* shareChannelActionItem(QObject *parent);
    static bb::cascades::ActionItem* copyChannelLinkActionItem(QObject *parent);
    static bb::cascades::ActionItem* openChannelInBrowserActionItem(QObject *parent);
    static bb::cascades::ActionItem* addToChannelsActionItem(QObject *parent);
    static bb::cascades::ActionItem* addToFavoritesActionItem(QObject *parent);
    static bb::cascades::ActionItem* addToWatchLaterActionItem(QObject *parent);
    static bb::cascades::DeleteActionItem* deleteActionItem(QObject *parent);

    static void updateAddToChannelsActionItem(bb::cascades::ActionItem* actionItem, bool isFavorite);
    static void updateAddToFavoritesActionItem(bb::cascades::ActionItem* actionItem, bool isFavorite);
    static void updateAddToWatchLaterActionItem(bb::cascades::ActionItem* actionItem, bool isWatchLater);
    static void copyVideoLink(QString videoId);
    static void openVideoInBrowser(QString videoId);
    static void copyRss(QString channelId);
    static void openChannelInBrowser(QString channelId);
    static void copyChannelLink(QString channelId);
    static void setShareChannelText(bb::cascades::InvokeActionItem* actionItem, QString title, QString channelId);
    static void setShareVideoText(bb::cascades::InvokeActionItem* actionItem, QString title, QString videoId);
};

#endif /* ACTIONITEMSERVICE_HPP_ */
