#ifndef VIDEOMETADATA_HPP_
#define VIDEOMETADATA_HPP_

class SingleVideoMetadata
{
public:
    QString title;
    QString videoId;
    QString thumbnailUrl;
    QString dateUploadedAgo;
    QString viewsCount;
    QString shortViewsCount;
    QString channelId;
    QString channelTitle;
    QString lengthText;

    bool isLiveStream() {
        return lengthText == "";
    }
};

class RelatedVideosData
{
public:
    SingleVideoMetadata nextVideo;
    QList<SingleVideoMetadata> otherVideos;
};

class Timecode
{
public:
    QString time;
    int seconds;
    QString description;
};

class VideoMetadata
{
public:
    SingleVideoMetadata video;
    RelatedVideosData relatedVideos;
    QList<Timecode> timecodes;
};
#endif /* VIDEOMETADATA_HPP_ */
