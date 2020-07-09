#ifndef PlaylistListItemProvider_HPP_
#define PlaylistListItemProvider_HPP_

#include "src/models/PlaylistListItemModel.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/applicationui.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include "src/db/DbHelper.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "PlaylistsPage.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/ActionItem>
#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/ActionSet>
#include <bb/system/SystemDialog>

using namespace bb::cascades;

class PlaylistListItem: public StandardListItem
{
Q_OBJECT
private slots:
    void onPlayAllActionItemClick()
    {
        setPlaylist();
        playVideo(playerContext->getVideoFromPlaylistByIndex(0));
    }

    void onShuffleAllActionItemClick()
    {
        setPlaylist();
        playerContext->shufflePlaylist();
        playVideo(playerContext->getVideoFromPlaylistByIndex(0));
    }

    void onDeleteAllActionItemClick()
    {
        bb::system::SystemDialog *confirmDialog = new bb::system::SystemDialog("Continue",
                "Cancel");

        confirmDialog->setTitle("Clear All");
        confirmDialog->setBody("All videos will be deleted from the playlist.");

        bool success = connect(confirmDialog, SIGNAL(finished(bb::system::SystemUiResult::Type)),
                this, SLOT(onDeleteAllDialogFinished(bb::system::SystemUiResult::Type)));

        if (success) {
            confirmDialog->show();
        } else {
            confirmDialog->deleteLater();
        }
    }

    void onDeleteAllDialogFinished(bb::system::SystemUiResult::Type type)
    {
        bb::system::SystemDialog *confirmDialog = qobject_cast<bb::system::SystemDialog *>(
                QObject::sender());
        PlaylistListItemModel *playlist = list->dataModel()->data(indexPath).value<
                PlaylistListItemModel*>();

        if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
            DbHelper::deleteAllFromPlaylist(playlist->playlistId);

            if (playlist->isPredefined()) {
                PlaylistVideoProxy::getInstance()->deleteAll(
                        (PlaylistListItemModel::Type) playlist->playlistId);
            }

            if (playlist->type == PlaylistListItemModel::History) {
                DbHelper::deleteAllViewedPercents();
                VideoViewedPercentProxy::getInstance()->deleteAll();
            }
        }

        confirmDialog->deleteLater();
    }
private:
    QVariantList indexPath;
    GlobalPlayerContext *playerContext;
    ListView *list;
    PlaylistsPage *page;

    void setPlaylist()
    {
        PlaylistListItemModel *playlist = list->dataModel()->data(indexPath).value<
                PlaylistListItemModel*>();
        QList<PlaylistVideoModel*> videos = DbHelper::getListByPlaylistId(playlist->playlistId);
        QList<QString> sequence;

        for (int i = 0; i < videos.count(); i++) {
            sequence.append(videos[i]->videoId);
        }

        playerContext->setPlaylist(playlist->playlistId, sequence);
    }
    void playVideo(QString videoId)
    {
        page->playVideo(videoId);
    }
public:
    PlaylistListItem(ListView* list, PlaylistsPage *page) :
            StandardListItem(), list(list), page(page)
    {
        playerContext = ApplicationUI::playerContext;

        ActionSet *actionSet = ActionSet::create().parent(this);

        bb::cascades::DeleteActionItem *deleteAllActionItem =
                bb::cascades::DeleteActionItem::create();
        deleteAllActionItem->setTitle("Clear All");
        bb::cascades::ActionItem *playAllActionItem = bb::cascades::ActionItem::create();
        playAllActionItem->setImageSource(QString("asset:///images/ic_play.png"));
        playAllActionItem->setTitle("Play All");
        bb::cascades::ActionItem *shuffleAllActionItem = bb::cascades::ActionItem::create();
        shuffleAllActionItem->setImageSource(QString("asset:///images/ic_shuffle.png"));
        shuffleAllActionItem->setTitle("Shuffle All");

        actionSet->add(playAllActionItem);
        actionSet->add(shuffleAllActionItem);
        actionSet->add(deleteAllActionItem);
        this->addActionSet(actionSet);

        QObject::connect(playAllActionItem, SIGNAL(triggered()), this,
                SLOT(onPlayAllActionItemClick()));
        QObject::connect(shuffleAllActionItem, SIGNAL(triggered()), this,
                SLOT(onShuffleAllActionItemClick()));
        QObject::connect(deleteAllActionItem, SIGNAL(triggered()), this,
                SLOT(onDeleteAllActionItemClick()));
    }
    ~PlaylistListItem()
    {
    }
    void setIndexPath(QVariantList path)
    {
        indexPath = path;
    }
    void setActions()
    {
        PlaylistListItemModel *playlist = list->dataModel()->data(indexPath).value<
                PlaylistListItemModel*>();

        ActionSet *actionSet = this->actionSetAt(0);
        actionSet->setTitle(this->title());
        actionSet->setSubtitle(this->description());
        for (int i = 0; i < actionSet->count(); i++) {
            actionSet->at(i)->setEnabled(playlist->videoCount > 0);
        }
    }
};
class PlaylistListItemProvider: public ListItemProvider
{
private:
    PlaylistsPage *page;
public:
    PlaylistListItemProvider(PlaylistsPage *page) :
            page(page)
    {

    }
    virtual ~PlaylistListItemProvider()
    {
    }

    VisualNode * createItem(ListView* list, const QString &type)
    {
        return new PlaylistListItem(list, page);
    }

    void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        Q_UNUSED(list);
        Q_UNUSED(indexPath);

        PlaylistListItemModel *model = data.value<PlaylistListItemModel*>();
        PlaylistListItem *item = static_cast<PlaylistListItem*>(listItem);
        QString imageSource = "";

        switch (model->type) {
            case PlaylistListItemModel::WatchLater:
                imageSource = "ic_later";
                break;
            case PlaylistListItemModel::Favorites:
                imageSource = "ic_favorite";
                break;
            case PlaylistListItemModel::History:
                imageSource = "ic_done";
                break;
            case PlaylistListItemModel::Local:
                imageSource = "ic_local";
                break;
            case PlaylistListItemModel::Remote:
                imageSource = "ic_remote";
                break;
            default:
                break;
        }

        item->setIndexPath(indexPath);
        item->setTitle(model->title);
        item->setImageSource(QUrl(QString("asset:///images/%1.png").arg(imageSource)));
        item->setDescription(QString("Videos: %1").arg(model->videoCount));
        item->setActions();
    }
};

#endif /* PlaylistListItemProvider_HPP_ */
