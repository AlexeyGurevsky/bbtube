#include "PlaylistsPage.hpp"
#include "src/utils/MiniPlayer.hpp"
#include "PlaylistListItemProvider.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/db/DbHelper.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/PlaylistPage/PlaylistPage.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ListView>
#include <bb/cascades/DockLayout>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/UIConfig>
#include <bb/cascades/ActionItem>

PlaylistsPage::PlaylistsPage(bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane)
{
    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);
    container->add(getTitleContainer());
    setTitle("Playlists");

    playlistsList = bb::cascades::ListView::create();
    playlistsList->setListItemProvider(new PlaylistListItemProvider(this));
    UpdatableDataModel<PlaylistListItemModel*> *model = new UpdatableDataModel<
            PlaylistListItemModel*>(DbHelper::getPlaylistList());
    for (int i = 0; i < model->size(); i++) {
        PlaylistListItemModel* item = model->value(i);
        if (item->isPredefined()) {
            item->videoCount = PlaylistVideoProxy::getInstance()->count(item->type);
        }
    }
    playlistsList->setDataModel(model);

    container->add(playlistsList);
    root->add(container);

    root->add(overlay);

    this->setContent(root);

    QObject::connect(playlistsList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onPlaylistsListItemClick(QVariantList)));
    QObject::connect(PlaylistVideoProxy::getInstance(), SIGNAL(added(PlaylistVideoModel*)), this,
            SLOT(onPlaylistVideoAdded(PlaylistVideoModel*)));
    QObject::connect(PlaylistVideoProxy::getInstance(),
            SIGNAL(deleted(QString,PlaylistListItemModel::Type)), this,
            SLOT(onPlaylistVideoDeleted(QString, PlaylistListItemModel::Type)));
    QObject::connect(PlaylistVideoProxy::getInstance(),
            SIGNAL(deletedAll(PlaylistListItemModel::Type)), this,
            SLOT(onPlaylistVideoDeletedAll(PlaylistListItemModel::Type)));
}

void PlaylistsPage::onPlaylistsListItemClick(QVariantList indexPath)
{
    PlaylistListItemModel *item = playlistsList->dataModel()->data(indexPath).value<
            PlaylistListItemModel*>();
    navigationPane->push(new PlaylistPage(item->playlistId, navigationPane));
}

void PlaylistsPage::onPlaylistVideoAdded(PlaylistVideoModel* video)
{
    UpdatableDataModel<PlaylistListItemModel*> *dataModel = (UpdatableDataModel<
            PlaylistListItemModel*> *) playlistsList->dataModel();
    for (int i = 0; i < dataModel->size(); i++) {
        PlaylistListItemModel *itemToUpdate = dataModel->value(i);
        if (itemToUpdate->playlistId == video->playlistId) {
            itemToUpdate->videoCount++;

            QVariantList indexPath;
            indexPath.append(i);
            dataModel->updateItem(indexPath);

            break;
        }
    }
}

void PlaylistsPage::onPlaylistVideoDeleted(QString videoId,
        PlaylistListItemModel::Type playlistType)
{
    UpdatableDataModel<PlaylistListItemModel*> *dataModel = (UpdatableDataModel<
            PlaylistListItemModel*> *) playlistsList->dataModel();
    for (int i = 0; i < dataModel->size(); i++) {
        PlaylistListItemModel *itemToUpdate = dataModel->value(i);

        if (itemToUpdate->playlistId == (int) playlistType) {
            itemToUpdate->videoCount--;

            QVariantList indexPath;
            indexPath.append(i);
            dataModel->updateItem(indexPath);

            break;
        }
    }
}

void PlaylistsPage::onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType)
{
    UpdatableDataModel<PlaylistListItemModel*> *dataModel = (UpdatableDataModel<
            PlaylistListItemModel*> *) playlistsList->dataModel();
    for (int i = 0; i < dataModel->size(); i++) {
        PlaylistListItemModel *itemToUpdate = dataModel->value(i);

        if (itemToUpdate->playlistId == (int) playlistType) {
            itemToUpdate->videoCount = 0;

            QVariantList indexPath;
            indexPath.append(i);
            dataModel->updateItem(indexPath);

            break;
        }
    }
}

void PlaylistsPage::playVideo(QString videoId)
{
    overlay->setVisible(true);
    isPlaylist = true;
    youtubeClient->process("https://www.youtube.com/watch?v=" + videoId);
}
