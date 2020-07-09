#ifndef PLAYLISTLISTITEMMODEL_HPP_
#define PLAYLISTLISTITEMMODEL_HPP_

class PlaylistListItemModel: public QObject
{
    Q_OBJECT
private:
    Q_DISABLE_COPY (PlaylistListItemModel);
public:
    enum Type
    {
        Favorites = 1, WatchLater, History, Local, Remote
    };
    PlaylistListItemModel(QObject *parent = 0) :
            QObject(parent)
    {
    }
    PlaylistListItemModel(int playlistId, Type type, QString title, int dateCreated,
            int dateLastPlayed, QString youtubeId, QString channelTitle, QObject *parent = 0) :
            playlistId(playlistId), type(type), title(title), dateCreated(dateCreated), dateLastPlayed(
                    dateLastPlayed), youtubeId(youtubeId), channelTitle(channelTitle), videoCount(
                    0), QObject(parent)
    {
    }

    bool isPredefined()
    {
        return type == Favorites || type == WatchLater || type == History;
    }

    int playlistId;
    Type type;
    QString title;
    int dateCreated;
    int dateLastPlayed;
    QString youtubeId;
    QString channelTitle;
    int videoCount;
};
Q_DECLARE_METATYPE( PlaylistListItemModel *);

#endif /* PLAYLISTLISTITEMMODEL_HPP_ */
