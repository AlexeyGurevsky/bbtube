#ifndef SEARCHLISTITEMPROVIDER_HPP_
#define SEARCHLISTITEMPROVIDER_HPP_

#include "src/models/VideoListItemModel.hpp"
#include "src/utils/ChannelListProxy.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/CustomListItem>
#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>
#include <bb/cascades/Header>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/ActionSet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>

using namespace bb::cascades;

class SearchListItemProvider;
class SearchListItem;
class SearchListItemActionSetBuilder;

class SearchListItem: public CustomListItem, public ListItemListener
{
Q_OBJECT

public:
    SearchListItem(CustomListView *listView, const SearchListItemProvider *provider,
            SearchListItemActionSetBuilder *actionSetBuilder, Container *parent = 0);
    virtual ~SearchListItem()
    {
    }

    void updateItem(const VideoListItemModel *item, QVariantList indexPath);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);

    void addChannelActionItem();
    void addAddToChannelsActionItem();
    void addAddToFavoritesActionItem();
    void addAddToWatchLaterActionItem();
    void addCopyChannelLinkActionItem();
    void addCopyRssLinkActionItem();
    void addOpenChannelInBrowserActionItem();
    void addShareChannelActionItem();
    void addPlayAudioOnlyActionItem();
    void addContinueActionItem();
    void addCopyVideoLinkActionItem();
    void addOpenVideoInBrowserActionItem();
    void addShareVideoActionItem();

signals:
    void showMore(QVariantList indexPath);
    void channelActionItemClick(QVariantList indexPath);
    void playAudioOnlyActionItemClick(QVariantList indexPath);
private slots:
    void onShowMoreClick();
    void onChannelActionItemClick();
    void onAddToChannelsActionItemClick();
    void onAddToFavoritesActionItemClick();
    void onAddToWatchLaterActionItemClick();
    void onCopyChannelLinkActionItemClick();
    void onCopyRssLinkActionItemClick();
    void onCopyVideoLinkActionItemClick();
    void onOpenVideoInBrowserActionItemClick();
    void onPlayAudioOnlyActionItemClick();
    void onContinueActionItemClick();
    void onOpenChannelInBrowserActionItemClick();
private:
    const SearchListItemProvider *provider;
    SearchListItemActionSetBuilder *actionSetBuilder;
    ImageView *thumbnail;
    ImageView *favoriteIcon;
    ImageView *watchLaterIcon;
    ImageView *historyIcon;
    Label *duration;
    Label *title;
    Label *subtitle;
    CustomListView *listView;
    Container *thumbnailContainer;
    Container *durationContainer;
    Container *viewedPercentContainer;
    Container *showMoreContainer;
    Container *itemContainer;
    QVariantList indexPath;
    ActionSet *actionSet;

    bb::cascades::ActionItem *channelActionItem;
    bb::cascades::ActionItem *addToChannelsActionItem;
    bb::cascades::ActionItem *addToFavoritesActionItem;
    bb::cascades::ActionItem *addToWatchLaterActionItem;
    bb::cascades::ActionItem *copyChannelLinkActionItem;
    bb::cascades::ActionItem *copyRssLinkActionItem;
    bb::cascades::ActionItem *copyVideoLinkActionItem;
    bb::cascades::ActionItem *openChannelInBrowserActionItem;
    bb::cascades::ActionItem *openVideoInBrowserActionItem;
    bb::cascades::ActionItem *playAudioOnlyActionItem;
    bb::cascades::ActionItem *continueActionItem;
    bb::cascades::InvokeActionItem *shareChannelActionItem;
    bb::cascades::InvokeActionItem *shareVideoActionItem;

    VideoListItemModel* getModelByPath();
};

class SearchListItemProvider: public ListItemProvider
{
Q_OBJECT
protected:
    CustomListView* list;

    bool *getFlagByPlaylistType(VideoListItemModel *listItem,
            PlaylistListItemModel::Type playlistType)
    {
        switch (playlistType) {
            case PlaylistListItemModel::Favorites:
                return &listItem->isFavorite;
            case PlaylistListItemModel::WatchLater:
                return &listItem->isWatchLater;
            case PlaylistListItemModel::History:
                return &listItem->isHistory;
            default:
                return 0;
        }
    }
public:
    QMap<QString, QByteArray> imagesCache;
    SearchListItemActionSetBuilder *actionSetBuilder;

    SearchListItemProvider(CustomListView* list, SearchListItemActionSetBuilder *actionSetBuilder) :
            list(list), actionSetBuilder(actionSetBuilder)
    {
    }
    virtual ~SearchListItemProvider()
    {
    }

    virtual VisualNode * createItem(ListView* list, const QString &type) = 0;
    virtual void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data) = 0;

signals:
    void showMore(QVariantList);
    void channelActionItemClick(QVariantList);
    void playAudioOnlyActionItemClick(QVariantList);
protected slots:
    void onShowMore(QVariantList indexPath)
    {
        emit showMore(indexPath);
    }
    void onChannelActionItemClick(QVariantList indexPath)
    {
        emit channelActionItemClick(indexPath);
    }
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath)
    {
        emit playAudioOnlyActionItemClick(indexPath);
    }
};

class SearchFlatListItemProvider: public SearchListItemProvider
{
Q_OBJECT
private slots:
    void onChannelItemAdded(ChannelListItemModel *item)
    {
        QVariantList indexPath = getIndexPath(item->channelId);
        if (indexPath.count() == 0) {
            return;
        }

        UpdatableDataModel<VideoListItemModel*> *dataModel =
                (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();
        VideoListItemModel *listItem = dataModel->data(indexPath).value<VideoListItemModel*>();

        listItem->isFavorite = true;
        dataModel->updateItem(indexPath);
    }

    void onChannelItemDeleted(QString channelId)
    {
        QVariantList indexPath = getIndexPath(channelId);
        if (indexPath.count() == 0) {
            return;
        }

        UpdatableDataModel<VideoListItemModel*> *dataModel =
                (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();
        VideoListItemModel *listItem = dataModel->data(indexPath).value<VideoListItemModel*>();

        listItem->isFavorite = false;
        dataModel->updateItem(indexPath);
    }

    void onPlaylistVideoAdded(PlaylistVideoModel* item)
    {
        QVariantList indexPath = getIndexPath(item->videoId);
        if (indexPath.count() == 0) {
            return;
        }

        UpdatableDataModel<VideoListItemModel*> *dataModel =
                (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();
        VideoListItemModel *listItem = dataModel->data(indexPath).value<VideoListItemModel*>();

        bool *flag = getFlagByPlaylistType(listItem,
                (PlaylistListItemModel::Type) item->playlistId);
        if (flag) {
            *flag = true;
            dataModel->updateItem(indexPath);
        }
    }

    void onPlaylistVideoDeleted(QString videoId, PlaylistListItemModel::Type playlistType)
    {
        QVariantList indexPath = getIndexPath(videoId);
        if (indexPath.count() == 0) {
            return;
        }

        UpdatableDataModel<VideoListItemModel*> *dataModel =
                (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();
        VideoListItemModel *listItem = dataModel->data(indexPath).value<VideoListItemModel*>();

        bool *flag = getFlagByPlaylistType(listItem, playlistType);
        if (flag) {
            *flag = false;
            dataModel->updateItem(indexPath);
        }
    }

    void onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType)
    {
        UpdatableDataModel<VideoListItemModel*> *dataModel =
                (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();

        if (!dataModel) {
            return;
        }

        for (int i = 0; i < dataModel->size(); i++) {
            VideoListItemModel *item = dataModel->value(i);
            bool *flag = getFlagByPlaylistType(item, playlistType);
            if (flag) {
                *flag = false;
            }
        }

        this->itemUpdatesNeeded();
    }

    void onVideoViewedPercentUpdated(QString videoId, int value)
    {
        QVariantList indexPath = getIndexPath(videoId);
        if (indexPath.count() == 0) {
            return;
        }

        UpdatableDataModel<VideoListItemModel*> *dataModel =
                        (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();

        dataModel->updateItem(indexPath);
    }

    void onVideoViewedPercentDeletedAll()
    {
        this->itemUpdatesNeeded();
    }
private:
    QVariantList getIndexPath(QString id)
    {
        UpdatableDataModel<VideoListItemModel*> *dataModel =
                (UpdatableDataModel<VideoListItemModel*> *) list->dataModel();

        if (!dataModel) {
            return QVariantList();
        }

        for (int i = 0; i < dataModel->size(); i++) {
            VideoListItemModel *item = dataModel->value(i);
            if (item->id == id) {
                return QVariantList() << i;
            }
        }

        return QVariantList();
    }
public:
    SearchFlatListItemProvider(CustomListView* list, SearchListItemActionSetBuilder *actionSetBuilder) :
            SearchListItemProvider(list, actionSetBuilder)
    {
        QObject::connect(ChannelListProxy::getInstance(), SIGNAL(added(ChannelListItemModel*)),
                this, SLOT(onChannelItemAdded(ChannelListItemModel*)));
        QObject::connect(ChannelListProxy::getInstance(), SIGNAL(deleted(QString)), this,
                SLOT(onChannelItemDeleted(QString)));
        QObject::connect(PlaylistVideoProxy::getInstance(), SIGNAL(added(PlaylistVideoModel*)),
                this, SLOT(onPlaylistVideoAdded(PlaylistVideoModel*)));
        QObject::connect(PlaylistVideoProxy::getInstance(),
                SIGNAL(deleted(QString,PlaylistListItemModel::Type)), this,
                SLOT(onPlaylistVideoDeleted(QString, PlaylistListItemModel::Type)));
        QObject::connect(PlaylistVideoProxy::getInstance(),
                SIGNAL(deleted(QString,PlaylistListItemModel::Type)), this,
                SLOT(onPlaylistVideoDeleted(QString, PlaylistListItemModel::Type)));
        QObject::connect(PlaylistVideoProxy::getInstance(),
                SIGNAL(deletedAll(PlaylistListItemModel::Type)), this,
                SLOT(onPlaylistVideoDeletedAll(PlaylistListItemModel::Type)));
        QObject::connect(VideoViewedPercentProxy::getInstance(), SIGNAL(updated(QString, int)),
                this, SLOT(onVideoViewedPercentUpdated(QString, int)));
        QObject::connect(VideoViewedPercentProxy::getInstance(), SIGNAL(deletedAll()), this,
                SLOT(onVideoViewedPercentDeletedAll()));
    }
    virtual ~SearchFlatListItemProvider()
    {
    }

    virtual VisualNode * createItem(ListView* list, const QString &type)
    {
        SearchListItem *item = new SearchListItem((CustomListView*)list, this, actionSetBuilder);

        QObject::connect(item, SIGNAL(showMore(QVariantList)), this,
                SLOT(onShowMore(QVariantList)));
        QObject::connect(item, SIGNAL(channelActionItemClick(QVariantList)), this,
                SLOT(onChannelActionItemClick(QVariantList)));
        QObject::connect(item, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
                SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));

        return item;
    }

    virtual void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        VideoListItemModel *item = data.value<VideoListItemModel*>();
        static_cast<SearchListItem*>(listItem)->updateItem(item, indexPath);
    }

};

class SearchGrouppedListItemProvider: public SearchListItemProvider
{
Q_OBJECT
private slots:
    void onChannelItemAdded(ChannelListItemModel *item)
    {
        QVariantList indexPath = getIndexPath(item->channelId);
        if (indexPath.count() == 0) {
            return;
        }

        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();
        VideoListItemModel *listItem = qobject_cast<VideoListItemModel*>(
                dataModel->data(indexPath).value<QObject*>());

        listItem->isFavorite = true;
        dataModel->updateItem(indexPath, listItem);
    }

    void onChannelItemDeleted(QString channelId)
    {
        QVariantList indexPath = getIndexPath(channelId);
        if (indexPath.count() == 0) {
            return;
        }

        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();
        VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
                dataModel->data(indexPath).value<QObject*>());

        item->isFavorite = false;
        dataModel->updateItem(indexPath, item);
    }

    void onPlaylistVideoAdded(PlaylistVideoModel* item)
    {
        QVariantList indexPath = getIndexPath(item->videoId);
        if (indexPath.count() == 0) {
            return;
        }

        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();
        VideoListItemModel *listItem = qobject_cast<VideoListItemModel*>(
                dataModel->data(indexPath).value<QObject*>());

        bool *flag = getFlagByPlaylistType(listItem,
                (PlaylistListItemModel::Type) item->playlistId);
        if (flag) {
            *flag = true;
            dataModel->updateItem(indexPath, listItem);
        }
    }

    void onPlaylistVideoDeleted(QString videoId, PlaylistListItemModel::Type playlistType)
    {
        QVariantList indexPath = getIndexPath(videoId);
        if (indexPath.count() == 0) {
            return;
        }

        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();
        VideoListItemModel *listItem = qobject_cast<VideoListItemModel*>(
                dataModel->data(indexPath).value<QObject*>());

        bool *flag = getFlagByPlaylistType(listItem, playlistType);
        if (flag) {
            *flag = false;
            dataModel->updateItem(indexPath, listItem);
        }
    }

    void onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType)
    {
        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();

        if (!dataModel) {
            return;
        }

        QVariantList headerIndexPath;
        QVariantList indexPath;

        for (int i = 0; i < dataModel->childCount(QVariantList()); i++) {
            headerIndexPath.append(i);
            indexPath.append(i);

            for (int j = 0; j < dataModel->childCount(headerIndexPath); j++) {
                indexPath.append(j);

                VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
                        dataModel->data(indexPath).value<QObject*>());
                bool *flag = getFlagByPlaylistType(item, playlistType);
                if (flag) {
                    *flag = false;
                }

                indexPath.removeLast();
            }

            indexPath.removeLast();
            headerIndexPath.removeLast();
        }

        this->itemUpdatesNeeded();
    }

    void onVideoViewedPercentUpdated(QString videoId, int value)
    {
        QVariantList indexPath = getIndexPath(videoId);
        if (indexPath.count() == 0) {
            return;
        }

        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();
        VideoListItemModel *listItem = qobject_cast<VideoListItemModel*>(
                dataModel->data(indexPath).value<QObject*>());

        dataModel->updateItem(indexPath, listItem);
    }

    void onVideoViewedPercentDeletedAll()
    {
        this->itemUpdatesNeeded();
    }
private:
    QVariantList getIndexPath(QString id)
    {
        bb::cascades::GroupDataModel *dataModel = (bb::cascades::GroupDataModel*) list->dataModel();

        if (!dataModel) {
            return QVariantList();
        }

        QVariantList headerIndexPath;
        QVariantList indexPath;

        for (int i = 0; i < dataModel->childCount(QVariantList()); i++) {
            headerIndexPath.append(i);
            indexPath.append(i);

            for (int j = 0; j < dataModel->childCount(headerIndexPath); j++) {
                indexPath.append(j);

                VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
                        dataModel->data(indexPath).value<QObject*>());
                if (item->id == id) {
                    return indexPath;
                }

                indexPath.removeLast();
            }

            indexPath.removeLast();
            headerIndexPath.removeLast();
        }

        return QVariantList();
    }
public:
    SearchGrouppedListItemProvider(CustomListView* list, SearchListItemActionSetBuilder *actionSetBuilder) :
            SearchListItemProvider(list, actionSetBuilder)
    {
        QObject::connect(ChannelListProxy::getInstance(), SIGNAL(added(ChannelListItemModel*)),
                this, SLOT(onChannelItemAdded(ChannelListItemModel*)));
        QObject::connect(ChannelListProxy::getInstance(), SIGNAL(deleted(QString)), this,
                SLOT(onChannelItemDeleted(QString)));
        QObject::connect(PlaylistVideoProxy::getInstance(), SIGNAL(added(PlaylistVideoModel*)),
                this, SLOT(onPlaylistVideoAdded(PlaylistVideoModel*)));
        QObject::connect(PlaylistVideoProxy::getInstance(),
                SIGNAL(deleted(QString,PlaylistListItemModel::Type)), this,
                SLOT(onPlaylistVideoDeleted(QString, PlaylistListItemModel::Type)));
        QObject::connect(PlaylistVideoProxy::getInstance(),
                SIGNAL(deletedAll(PlaylistListItemModel::Type)), this,
                SLOT(onPlaylistVideoDeletedAll(PlaylistListItemModel::Type)));
        QObject::connect(VideoViewedPercentProxy::getInstance(), SIGNAL(updated(QString, int)),
                this, SLOT(onVideoViewedPercentUpdated(QString, int)));
        QObject::connect(VideoViewedPercentProxy::getInstance(), SIGNAL(deletedAll()), this,
                SLOT(onVideoViewedPercentDeletedAll()));
    }
    virtual ~SearchGrouppedListItemProvider()
    {
    }

    virtual VisualNode * createItem(ListView* list, const QString &type)
    {
        if (type.compare(GroupDataModel::Header) == 0) {
            return new Header();
        } else {
            SearchListItem *item = new SearchListItem((CustomListView*)list, this, actionSetBuilder);

            QObject::connect(item, SIGNAL(showMore(QVariantList)), this,
                    SLOT(onShowMore(QVariantList)));
            QObject::connect(item, SIGNAL(channelActionItemClick(QVariantList)), this,
                    SLOT(onChannelActionItemClick(QVariantList)));
            QObject::connect(item, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
                    SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));

            return item;
        }
    }

    virtual void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        if (type.compare(GroupDataModel::Header) == 0) {
            QString caption = data.value<QString>();
            caption = caption.mid(caption.indexOf(":") + 1);
            static_cast<Header*>(listItem)->setTitle(caption);
        } else {
            VideoListItemModel *item = qobject_cast<VideoListItemModel *>(data.value<QObject*>());
            static_cast<SearchListItem*>(listItem)->updateItem(item, indexPath);
        }
    }
};

class SearchListItemActionSetBuilder
{
protected:
    void buildCommonVideoActionSet(const VideoListItemModel* item, SearchListItem* listItem)
    {
        listItem->addAddToFavoritesActionItem();
        listItem->addAddToWatchLaterActionItem();
        listItem->addShareVideoActionItem();
        listItem->addOpenVideoInBrowserActionItem();
        listItem->addCopyVideoLinkActionItem();
    }
    void buildCommonChannelActionSet(const VideoListItemModel* item, SearchListItem* listItem)
    {
        listItem->addShareChannelActionItem();
        listItem->addOpenChannelInBrowserActionItem();
        listItem->addCopyChannelLinkActionItem();
        listItem->addCopyRssLinkActionItem();
    }
public:
    virtual ~SearchListItemActionSetBuilder()
    {
    }

    virtual void buildActionSet(const VideoListItemModel*, SearchListItem*) = 0;
};

#endif /* SEARCHLISTITEMPROVIDER_HPP_ */
