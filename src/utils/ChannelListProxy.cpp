#include "ChannelListProxy.hpp"
#include "src/db/DbHelper.hpp"
#include "src/models/ChannelListItemModel.hpp"

ChannelListProxy* ChannelListProxy::instance = 0;

ChannelListProxy* ChannelListProxy::getInstance()
{
    if (!instance) {
        instance = new ChannelListProxy();
        instance->channelIds = DbHelper::getChannelIdList().toSet();
    }

    return instance;
}
void ChannelListProxy::add(ChannelListItemModel *channel)
{
    if (!channelIds.contains(channel->channelId)) {
        channelIds.insert(channel->channelId);
        emit added(channel);
    }
}
void ChannelListProxy::deleteById(QString channelId)
{
    if (channelIds.remove(channelId)) {
        emit deleted(channelId);
    }

}
bool ChannelListProxy::contains(QString channelId)
{
    return channelIds.contains(channelId);
}

void ChannelListProxy::updateChannelGeneralInfo(ChannelListItemModel *channel)
{
    if (channelIds.contains(channel->channelId)) {
        emit updated(channel);
    }
}
