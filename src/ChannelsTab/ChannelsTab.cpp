#include "ChannelsTab.hpp"
#include "src/ChannelListPage/ChannelListPage.hpp"

ChannelsTab::ChannelsTab()
{
    this->setTitle("Channels");
    this->setImageSource(QString("asset:///images/ic_favorite.png"));

    navigationPane->push(new ChannelListPage(navigationPane));
}



