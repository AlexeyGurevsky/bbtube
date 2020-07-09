#ifndef PlaylistVideoListItemProvider_HPP_
#define PlaylistVideoListItemProvider_HPP_

#include "src/models/PlaylistVideoModel.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"
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
#include <bb/cascades/ActionSet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>
#include <bb/cascades/DeleteActionItem>

using namespace bb::cascades;

class PlaylistVideoListItemProvider;
class PlaylistVideoListItem;

class PlaylistVideoListItem: public CustomListItem, public ListItemListener
{
    Q_OBJECT

public:
    PlaylistVideoListItem(CustomListView *listView, const PlaylistVideoListItemProvider *provider,
            Container *parent = 0);
    virtual ~PlaylistVideoListItem()
    {
    }

    void updateItem(const PlaylistVideoModel *item, QVariantList indexPath);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);

    signals:
    void channelActionItemClick(QVariantList indexPath);
    void playAudioOnlyActionItemClick(QVariantList indexPath);
    void deleteActionItemClick(QVariantList indexPath);
private slots:
    void onChannelActionItemClick();
    void onCopyVideoLinkActionItemClick();
    void onOpenVideoInBrowserActionItemClick();
    void onPlayAudioOnlyActionItemClick();
    void onDeleteActionItemClick();
    void onContinueActionItemClick();
private:
    const PlaylistVideoListItemProvider *provider;
    ImageView *thumbnail;
    Label *title;
    Label *subtitle;
    CustomListView *listView;
    Container *thumbnailContainer;
    Container *viewedPercentContainer;
    Container *itemContainer;
    Container *onAirContainer;
    QVariantList indexPath;
    ActionSet *actionSet;

    bb::cascades::ActionItem *channelActionItem;
    bb::cascades::ActionItem *copyVideoLinkActionItem;
    bb::cascades::ActionItem *openVideoInBrowserActionItem;
    bb::cascades::ActionItem *continueActionItem;
    bb::cascades::ActionItem *playAudioOnlyActionItem;
    bb::cascades::InvokeActionItem *shareVideoActionItem;
    bb::cascades::DeleteActionItem *deleteActionItem;

    PlaylistVideoModel* getModelByPath();
};

class PlaylistVideoListItemProvider: public ListItemProvider
{
    Q_OBJECT
private:
    CustomListView* list;

    QVariantList getIndexPath(QString id)
    {
        UpdatableDataModel<PlaylistVideoModel*> *dataModel =
                (UpdatableDataModel<PlaylistVideoModel*> *) list->dataModel();

        if (!dataModel) {
            return QVariantList();
        }

        for (int i = 0; i < dataModel->size(); i++) {
            PlaylistVideoModel *item = dataModel->value(i);
            if (item->videoId == id) {
                return QVariantList() << i;
            }
        }

        return QVariantList();
    }
private slots:
    void onVideoViewedPercentUpdated(QString videoId, int value)
    {
        QVariantList indexPath = getIndexPath(videoId);
        if (indexPath.count() == 0) {
            return;
        }

        UpdatableDataModel<PlaylistVideoModel*> *dataModel =
        (UpdatableDataModel<PlaylistVideoModel*> *) list->dataModel();

        dataModel->updateItem(indexPath);
    }

    void onVideoViewedPercentDeletedAll()
    {
        this->itemUpdatesNeeded();
    }
public:
    QMap<QString, QByteArray> imagesCache;

    PlaylistVideoListItemProvider(CustomListView* list) :
            list(list)
    {
        QObject::connect(VideoViewedPercentProxy::getInstance(), SIGNAL(updated(QString, int)),
                this, SLOT(onVideoViewedPercentUpdated(QString, int)));
        QObject::connect(VideoViewedPercentProxy::getInstance(), SIGNAL(deletedAll()), this,
                SLOT(onVideoViewedPercentDeletedAll()));
    }
    virtual ~PlaylistVideoListItemProvider()
    {
    }

    virtual VisualNode * createItem(ListView* list, const QString &type)
    {
        PlaylistVideoListItem *item = new PlaylistVideoListItem((CustomListView*)list, this);

        QObject::connect(item, SIGNAL(channelActionItemClick(QVariantList)), this,
                SLOT(onChannelActionItemClick(QVariantList)));
        QObject::connect(item, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
                SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
        QObject::connect(item, SIGNAL(deleteActionItemClick(QVariantList)), this,
                SLOT(onDeleteActionItemClick(QVariantList)));

        return item;
    }

    virtual void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        PlaylistVideoModel *item = data.value<PlaylistVideoModel*>();
        static_cast<PlaylistVideoListItem*>(listItem)->updateItem(item, indexPath);
    }
    signals:
    void channelActionItemClick(QVariantList indexPath);
    void playAudioOnlyActionItemClick(QVariantList indexPath);
    void deleteActionItemClick(QVariantList indexPath);private slots:
    void onChannelActionItemClick(QVariantList indexPath)
    {
        emit channelActionItemClick(indexPath);
    }
    void onPlayAudioOnlyActionItemClick(QVariantList indexPath)
    {
        emit playAudioOnlyActionItemClick(indexPath);
    }
    void onDeleteActionItemClick(QVariantList indexPath)
    {
        emit deleteActionItemClick(indexPath);
    }
};

#endif /* PlaylistVideoListItemProvider_HPP_ */
