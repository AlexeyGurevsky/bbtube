#ifndef ChannelData_HPP_
#define ChannelData_HPP_

#include "VideoMetadata.hpp"

class ChannelData
{
public:
    QString channelId;
    QString title;
    QString thumbnailUrl;
    QString redirectChannelId;
};

class ChannelPageData : public ChannelData
{
public:
    QString clientVersion;
    QList<SingleVideoMetadata> videos;
    QString ctoken;
    QString apiKey;
};

#endif /* ChannelData_HPP_ */
