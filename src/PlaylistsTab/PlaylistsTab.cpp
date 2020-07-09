#include "PlaylistsTab.hpp"
#include "src/PlaylistsPage/PlaylistsPage.hpp"

PlaylistsTab::PlaylistsTab()
{
    this->setTitle("Playlists");
    this->setImageSource(QString("asset:///images/ic_playlists.png"));

    navigationPane->push(new PlaylistsPage(navigationPane));
}



