#ifndef PlaylistVideoProxy_HPP_
#define PlaylistVideoProxy_HPP_

#include "src/models/PlaylistListItemModel.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include <QSet>

class PlaylistVideoProxy : public QObject
{
    Q_OBJECT
private:
    QSet<QString> favoriteIds;
    QSet<QString> watchLaterIds;
    QSet<QString> historyIds;

    static PlaylistVideoProxy* instance;
    PlaylistVideoProxy()
    {
    }

    QSet<QString> * getListByType(PlaylistListItemModel::Type type);
public:
    static PlaylistVideoProxy* getInstance();
    void add(PlaylistVideoModel *video);
    void deleteById(QString videoId, PlaylistListItemModel::Type type);
    bool contains(QString videoId, PlaylistListItemModel::Type type);
    int count(PlaylistListItemModel::Type type);
    void update(PlaylistVideoModel* video);
    void deleteAll(PlaylistListItemModel::Type type);
signals:
    void added(PlaylistVideoModel *channel);
    void deleted(QString videoId, PlaylistListItemModel::Type type);
    void deletedAll(PlaylistListItemModel::Type type);
    void updated(PlaylistVideoModel *channel);
};

#endif /* PlaylistVideoProxy_HPP_ */
