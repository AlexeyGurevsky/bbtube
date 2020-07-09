#ifndef CHANNELLISTPROXY_HPP_
#define CHANNELLISTPROXY_HPP_

#include "src/models/ChannelListItemModel.hpp"
#include <QSet>

class ChannelListProxy : public QObject
{
    Q_OBJECT
private:
    QSet<QString> channelIds;
    static ChannelListProxy* instance;
    ChannelListProxy()
    {
    }
public:
    static ChannelListProxy* getInstance();
    void add(ChannelListItemModel *channel);
    void deleteById(QString channelId);
    bool contains(QString channelId);
    void updateChannelGeneralInfo(ChannelListItemModel *channel);
signals:
    void added(ChannelListItemModel *channel);
    void deleted(QString channelId);
    void updated(ChannelListItemModel *channel);
};

#endif /* CHANNELLISTPROXY_HPP_ */
