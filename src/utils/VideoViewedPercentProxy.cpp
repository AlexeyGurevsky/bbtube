#include "VideoViewedPercentProxy.hpp"
#include "src/db/DbHelper.hpp"

VideoViewedPercentProxy* VideoViewedPercentProxy::instance = 0;

VideoViewedPercentProxy* VideoViewedPercentProxy::getInstance()
{
    if (!instance) {
        instance = new VideoViewedPercentProxy();
        instance->map = DbHelper::getViewedPercents();
    }

    return instance;
}

void VideoViewedPercentProxy::setViewedPercent(QString videoId, int value)
{
    if (value < 0) {
        return;
    }

    if (map.contains(videoId)) {
        int oldValue = map[videoId];

        if (oldValue >= value) {
            return;
        }
    }

    map[videoId] = value;
    emit updated(videoId, value);
}

void VideoViewedPercentProxy::deleteById(QString videoId)
{
    if (map.contains(videoId)) {
        map.remove(videoId);

        emit updated(videoId, 0);
    }
}

int VideoViewedPercentProxy::getPercent(QString videoId)
{
    if (map.contains(videoId)) {
        return map[videoId];
    }

    return 0;
}

void VideoViewedPercentProxy::deleteAll()
{
    map.clear();

    emit deletedAll();
}
