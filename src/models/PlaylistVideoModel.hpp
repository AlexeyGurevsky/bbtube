#ifndef PlaylistVideoModel_HPP_
#define PlaylistVideoModel_HPP_

class PlaylistVideoModel: public QObject
{
    Q_OBJECT
private:
    Q_DISABLE_COPY (PlaylistVideoModel);
public:
    enum SortBy
    {
        DateAdded, Name, DateViewed
    };
    enum SortDirection
    {
        Asc, Desc
    };
    PlaylistVideoModel(QObject *parent = 0) : QObject(parent), onAir(false)
    {
    }
    PlaylistVideoModel(QString videoId, int playlistId, QString title, QString channelId,
            QString channelTitle, int dateAdded, int dateLastPlayed, int sortOrder,
            QObject *parent = 0) :
            QObject(parent), onAir(false), videoId(videoId), playlistId(playlistId), title(title), channelId(
                    channelId), channelTitle(channelTitle), dateAdded(dateAdded), dateLastPlayed(
                    dateLastPlayed), sortOrder(sortOrder)
    {
    }

    QString videoId;
    int playlistId;
    QString title;
    QString channelId;
    QString channelTitle;
    int dateAdded;
    int dateLastPlayed;
    int sortOrder;
    bool onAir;
};
Q_DECLARE_METATYPE( PlaylistVideoModel *);

#endif /* PlaylistVideoModel_HPP_ */
