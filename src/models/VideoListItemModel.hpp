#ifndef VIDEOLISTITEMMODEL_HPP_
#define VIDEOLISTITEMMODEL_HPP_

#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/utils/ChannelListProxy.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"

#include <QStringList>

class VideoListItemModel: public QObject
{
Q_OBJECT

Q_PROPERTY(QString category READ category)
Q_PROPERTY(QString title READ title)
Q_PROPERTY(QString sortOrder READ sortOrder)
public:
    enum ItemType
    {
        Channel, Video, ShowMore
    };

    VideoListItemModel(QObject *parent = 0) :
            QObject(parent), _sortOrder(0), isFavorite(false), isWatchLater(false), isHistory(
                    false), liveStream(false)
    {
    }

    QString subtitle;
    QString id;
    QString thumbnailUrl;
    QString lengthText;
    int published;
    ItemType type;
    QString channelId;
    QString channelTitle;
    bool isFavorite;
    bool isWatchLater;
    bool isHistory;
    bool liveStream;

    QString title() const
    {
        return _title;
    }
    void setTitle(QString title)
    {
        _title = title;
    }
    QString category() const
    {
        return _category;
    }
    void setCategory(QString category)
    {
        _category = category;
    }
    int sortOrder() const
    {
        return _sortOrder;
    }
    void setSortOrder(int sortOrder)
    {
        _sortOrder = sortOrder;
    }
    bool isLiveStream() const
    {
        return liveStream;
    }
    static VideoListItemModel *mapVideo(const SingleVideoMetadata *video)
    {
        VideoListItemModel *item = new VideoListItemModel();

        item->setTitle(video->title);

        QStringList subtitleParts;
        if (video->channelTitle != "") {
            subtitleParts.append(video->channelTitle);
        }
        if (video->shortViewsCount != "") {
            subtitleParts.append(video->shortViewsCount);
        }
        if (video->dateUploadedAgo != "") {
            subtitleParts.append(video->dateUploadedAgo);
        }

        item->subtitle = subtitleParts.join(" • ");
        item->thumbnailUrl = video->thumbnailUrl;
        item->id = video->videoId;
        item->type = VideoListItemModel::Video;
        item->lengthText = video->lengthText;
        item->channelId = video->channelId;
        item->channelTitle = video->channelTitle;
        item->liveStream = item->lengthText == "";
        item->isFavorite = PlaylistVideoProxy::getInstance()->contains(item->id,
                PlaylistListItemModel::Favorites);
        item->isWatchLater = PlaylistVideoProxy::getInstance()->contains(item->id,
                        PlaylistListItemModel::WatchLater);
        item->isHistory = PlaylistVideoProxy::getInstance()->contains(item->id,
                                PlaylistListItemModel::History);

        return item;
    }
    static VideoListItemModel *mapChannel(const ChannelData *channel)
    {
        VideoListItemModel *item = new VideoListItemModel();

        item->setTitle(channel->title);
        item->thumbnailUrl = channel->thumbnailUrl;
        item->id = channel->channelId;
        item->type = VideoListItemModel::Channel;
        item->channelId = channel->channelId;
        item->isFavorite = ChannelListProxy::getInstance()->contains(item->id);
        item->channelTitle = channel->title;

        return item;
    }
private:
    QString _category;
    int _sortOrder;
    QString _title;Q_DISABLE_COPY (VideoListItemModel)
};
Q_DECLARE_METATYPE( VideoListItemModel *);

#endif /* VIDEOLISTITEMMODEL_HPP_ */
