#ifndef CHANNELLISTITEMMODEL_HPP_
#define CHANNELLISTITEMMODEL_HPP_

class ChannelListItemModel: public QObject
{
    Q_OBJECT
private:
    Q_DISABLE_COPY (ChannelListItemModel);
public:
    enum SortBy
    {
        LastActivity,
        Name,
        DateVisited,
        DateAdded
    };
    enum SortDirection
    {
        Asc,
        Desc
    };
    ChannelListItemModel(QObject *parent = 0) : QObject(parent)
    {
    }
    ChannelListItemModel(QString channelId, QString title, QString thumbnailUrl, int dateAdded,
            int dateLastVisited, QString lastVideoId, QString lastVideoTitle, int dateLastActivity,
            QObject *parent = 0) :
            channelId(channelId), title(title), thumbnailUrl(thumbnailUrl), dateAdded(dateAdded), dateLastVisited(
                    dateLastVisited), lastVideoId(lastVideoId), lastVideoTitle(lastVideoTitle), dateLastActivity(
                    dateLastActivity), QObject(parent)
    {
    }

    QString channelId;
    QString title;
    QString thumbnailUrl;
    int dateAdded;
    int dateLastVisited;
    QString lastVideoId;
    QString lastVideoTitle;
    int dateLastActivity;
};
Q_DECLARE_METATYPE( ChannelListItemModel *);
#endif /* CHANNELLISTITEMMODEL_HPP_ */
