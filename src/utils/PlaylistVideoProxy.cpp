#include "PlaylistVideoProxy.hpp"
#include "src/db/DbHelper.hpp"
#include "src/models/PlaylistVideoModel.hpp"

PlaylistVideoProxy* PlaylistVideoProxy::instance = 0;

PlaylistVideoProxy* PlaylistVideoProxy::getInstance()
{
    if (!instance) {
        instance = new PlaylistVideoProxy();
        QList<PlaylistVideoModel*> videos = DbHelper::getAllPlaylistVideoIds();

        for (int i = 0; i < videos.count(); i++) {
            QSet<QString> *list = instance->getListByType(
                    (PlaylistListItemModel::Type) videos[i]->playlistId);
            list->insert(videos[i]->videoId);
        }

        qDeleteAll(videos);
    }

    return instance;
}

void PlaylistVideoProxy::add(PlaylistVideoModel* video)
{
    QSet<QString> *list = getListByType((PlaylistListItemModel::Type) video->playlistId);

    if (!list->contains(video->videoId)) {
        list->insert(video->videoId);
        emit added(video);
    }
}

void PlaylistVideoProxy::deleteById(QString videoId, PlaylistListItemModel::Type type)
{
    QSet<QString> *list = getListByType(type);

    if (list->remove(videoId)) {
        emit deleted(videoId, type);
    }
}

void PlaylistVideoProxy::deleteAll(PlaylistListItemModel::Type type)
{
    QSet<QString> *list = getListByType(type);

    list->clear();
    emit deletedAll(type);
}

bool PlaylistVideoProxy::contains(QString videoId, PlaylistListItemModel::Type type)
{
    QSet<QString> *list = getListByType(type);

    return list->contains(videoId);
}

QSet<QString> * PlaylistVideoProxy::getListByType(PlaylistListItemModel::Type type)
{
    switch (type) {
        case PlaylistListItemModel::Favorites:
            return &favoriteIds;
        case PlaylistListItemModel::WatchLater:
            return &watchLaterIds;
        case PlaylistListItemModel::History:
            return &historyIds;
        default:
            return new QSet<QString>();
    }
}

int PlaylistVideoProxy::count(PlaylistListItemModel::Type type)
{
    return getListByType(type)->count();
}

void PlaylistVideoProxy::update(PlaylistVideoModel* video)
{
    emit updated(video);
}
