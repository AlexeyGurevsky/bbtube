#ifndef PlaylistsPage_HPP_
#define PlaylistsPage_HPP_

#include "src/utils/BasePage.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/PlaylistListItemModel.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <QVariantList>

class PlaylistsPage: public BasePage
{
Q_OBJECT
private slots:
    void onPlaylistsListItemClick(QVariantList indexPath);
    void onPlaylistVideoAdded(PlaylistVideoModel* video);
    void onPlaylistVideoDeleted(QString videoId, PlaylistListItemModel::Type playlistType);
    void onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType);
private:
    bb::cascades::ListView *playlistsList;
public:
    PlaylistsPage(bb::cascades::NavigationPane *navigationPane);
    virtual ~PlaylistsPage()
    {
    }
    void playVideo(QString videoId);
};

#endif /* PlaylistsPage_HPP_ */
