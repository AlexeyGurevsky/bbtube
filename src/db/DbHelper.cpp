#include "DbHelper.hpp"
#include <QtSql/QtSql>

QSqlDatabase DbHelper::db;

QSqlDatabase DbHelper::getDbContext()
{
    if (db.isOpen()) {
        return db;
    }

    QString dbName = "youtube.db";
    QString dataFolder = QDir::homePath();
    QString newFileName = dataFolder + "/" + dbName;
    QFile newFile(newFileName);

    if (!newFile.exists()) {
        QString appFolder(QDir::homePath());
        appFolder.chop(4);
        QString originalFileName = appFolder + "app/native/assets/" + dbName;
        QFile originalFile(originalFileName);

        if (originalFile.exists()) {
            originalFile.copy(newFileName);
        } else {
            qDebug() << "Failed to copy file, database file does not exist.";
        }
    }

    db = QSqlDatabase::addDatabase("QSQLITE", "youtube");
    db.setDatabaseName(newFileName);
    if (!db.open()) {
        qDebug() << "Can't open sql DB";
    }

    return db;
}

void DbHelper::transaction()
{
    if (!getDbContext().transaction()) {
        qWarning() << "Can't begin transaction";
    }
}

void DbHelper::commit()
{
    if (!getDbContext().commit()) {
        qWarning() << "Can't commit transaction";
    }
}

void DbHelper::runMigrations()
{
    createScriptsTableIfNeeded();
    applyScripts();
}

void DbHelper::createScriptsTableIfNeeded()
{
    QSqlQuery query(getDbContext());

    query.prepare("SELECT 1 FROM sqlite_master WHERE type='table' AND name = 'Scripts';");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();
    bool scriptsTableExists = query.next();
    qDebug() << "scriptsTableExists" << scriptsTableExists;

    if (!scriptsTableExists) {
        query.prepare("CREATE TABLE Scripts "
                "([Name] varchar(32), "
                "CONSTRAINT PK_Scripts PRIMARY KEY ([Name] ASC) );");
        query.exec();

        QSqlError err = query.lastError();

        if (err.isValid()) {
            qWarning() << "SQL reported an error : " << err.text();
        }
    }
}

void DbHelper::applyScripts()
{
    QSqlQuery query(getDbContext());
    QStringList filters;
    filters << "*.sql";

    QString appFolder(QDir::homePath());
    appFolder.chop(4);
    QDir assetsFolder = QDir(appFolder + "app/native/assets/sql");
    assetsFolder.setSorting(QDir::Name);
    assetsFolder.setNameFilters(filters);
    assetsFolder.setFilter(QDir::Files);

    QFileInfoList files = assetsFolder.entryInfoList();
    for (int i = 0; i < files.count(); i++) {
        QString scriptName = files[i].fileName();

        query.prepare("SELECT 1 FROM Scripts WHERE [Name] = :name;");
        query.bindValue(":name", scriptName);
        query.exec();

        QSqlError err = query.lastError();

        if (err.isValid()) {
            qWarning() << scriptName << "1: SQL reported an error : " << err.text();
            break;
        }

        QSqlRecord rec = query.record();
        bool needToRunScript = !query.next();

        if (needToRunScript) {
            qDebug() << "Applying:" << scriptName;
            QFile file(files[i].absoluteFilePath());

            if (!file.open(QFile::ReadOnly | QFile::Text)) {
                qDebug() << "Can't open file" << scriptName;
                break;
            }

            QTextStream in(&file);
            QString scriptSource = in.readAll();
            QStringList statements = scriptSource.split(';');

            for (int j = 0; j < statements.count(); j++) {
                QString statement = statements[j];
                if (statement.trimmed().isEmpty()) {
                    continue;
                }

                query.exec(statement);
                err = query.lastError();
                if (err.isValid()) {
                    qWarning() << scriptName << "2: SQL reported an error : " << err.text();
                    break;
                }
            }

            QSqlQuery query(getDbContext());
            query.prepare("INSERT INTO Scripts([Name]) VALUES(:name);");
            query.bindValue(":name", scriptName);
            query.exec();

            err = query.lastError();
            if (err.isValid()) {
                qWarning() << scriptName << "3: SQL reported an error : " << err.text();
                break;
            }
        }
    }
}

bool DbHelper::isAutoplay()
{
    QSqlQuery query(getDbContext());

    query.prepare("SELECT CAST([Value] AS int) as [Value] FROM Settings WHERE [Key] = 'autoplay';");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();
    query.next();

    return query.value(rec.indexOf("Value")).toString() == "1";
}

void DbHelper::setAutoplay(bool value)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE Settings SET [Value] = :value WHERE [Key] = 'autoplay';");
    query.bindValue(":value", value ? "1" : "0");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

QString DbHelper::defaultTab()
{
    QSqlQuery query(getDbContext());

    query.prepare("SELECT [Value] FROM Settings WHERE [Key] = 'tab';");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();
    query.next();

    return query.value(rec.indexOf("Value")).toString();
}

void DbHelper::setDefautTab(QString tab)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE Settings SET [Value] = :value WHERE [Key] = 'tab';");
    query.bindValue(":value", tab);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

QString DbHelper::defaultQuality()
{
    QSqlQuery query(getDbContext());

    query.prepare("SELECT [Value] FROM Settings WHERE [Key] = 'quality';");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();
    query.next();

    return query.value(rec.indexOf("Value")).toString();
}

void DbHelper::setDefautQuality(QString quality)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE Settings SET [Value] = :value WHERE [Key] = 'quality';");
    query.bindValue(":value", quality);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

int DbHelper::getEqualizerPreset()
{
    QSqlQuery query(getDbContext());

    query.prepare("SELECT CAST([Value] AS int) as [Value] FROM Settings WHERE [Key] = 'eqpreset';");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();
    query.next();

    return query.value(rec.indexOf("Value")).toInt();
}

void DbHelper::setEqualizerPreset(int value)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE Settings SET [Value] = :value WHERE [Key] = 'eqpreset';");
    query.bindValue(":value", QString::number(value));
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

int DbHelper::getPlaybackTimeout()
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "SELECT CAST([Value] AS int) as [Value] FROM Settings WHERE [Key] = 'playtimeout';");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();
    query.next();

    return query.value(rec.indexOf("Value")).toInt();
}

void DbHelper::setPlaybackTimeout(int value)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE Settings SET [Value] = :value WHERE [Key] = 'playtimeout';");
    query.bindValue(":value", QString::number(value));
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::createChannel(const ChannelListItemModel *channel)
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "INSERT OR IGNORE INTO Channel(ChannelId, Title, ThumbnailUrl, DateAdded, DateLastVisited, LastVideoId, LastVideoTitle, DateLastActivity) "
                    "VALUES(:channelId, :title, :thumbnailUrl, :dateAdded, :dateLastVisited, :lastVideoId, :lastVideoTitle, :dateLastActivity);");
    query.bindValue(":channelId", channel->channelId);
    query.bindValue(":title", channel->title);
    query.bindValue(":thumbnailUrl", channel->thumbnailUrl);
    query.bindValue(":dateAdded", channel->dateAdded);
    query.bindValue(":dateLastVisited", channel->dateLastVisited);
    query.bindValue(":lastVideoId", channel->lastVideoId);
    query.bindValue(":lastVideoTitle", channel->lastVideoTitle);
    query.bindValue(":dateLastActivity", channel->dateLastActivity);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::updateChannelGeneralInfo(const ChannelListItemModel *channel)
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "UPDATE Channel SET Title = :title, ThumbnailUrl = :thumbnailUrl, DateLastVisited = :dateLastVisited WHERE channelId = :channelId; ");
    query.bindValue(":channelId", channel->channelId);
    query.bindValue(":title", channel->title);
    query.bindValue(":thumbnailUrl", channel->thumbnailUrl);
    query.bindValue(":dateLastVisited", channel->dateLastVisited);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::updateChannelActivityInfo(const ChannelListItemModel *channel)
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "UPDATE Channel SET LastVideoId = :lastVideoId, LastVideoTitle = :lastVideoTitle, DateLastActivity = :dateLastActivity WHERE channelId = :channelId; ");
    query.bindValue(":channelId", channel->channelId);
    query.bindValue(":lastVideoId", channel->lastVideoId);
    query.bindValue(":lastVideoTitle", channel->lastVideoTitle);
    query.bindValue(":dateLastActivity", channel->dateLastActivity);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::markAllChannelsAsRead(int currentTime)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE Channel SET DateLastVisited = :dateLastVisited; ");
    query.bindValue(":dateLastVisited", currentTime);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::deleteChannel(QString channelId)
{
    QSqlQuery query(getDbContext());

    query.prepare("DELETE FROM Channel WHERE channelId = :channelId; ");
    query.bindValue(":channelId", channelId);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

QList<ChannelListItemModel*> DbHelper::getChannelList(ChannelListItemModel::SortBy sortBy,
        ChannelListItemModel::SortDirection sortOrder)
{
    QList<ChannelListItemModel*> list;
    QSqlQuery query(getDbContext());

    QString sortString;
    switch (sortBy) {
        case ChannelListItemModel::LastActivity:
            sortString = "DateLastActivity";
            break;
        case ChannelListItemModel::Name:
            sortString = "UPPER(Title)";
            break;
        case ChannelListItemModel::DateVisited:
            sortString = "DateLastVisited";
            break;
        case ChannelListItemModel::DateAdded:
            sortString = "DateAdded";
            break;
    }
    sortString += QString(" ") + (sortOrder == ChannelListItemModel::Asc ? "ASC" : "DESC");
    query.prepare(
            QString(
                    "SELECT ChannelId, Title, ThumbnailUrl, DateAdded, DateLastVisited, LastVideoId, LastVideoTitle, DateLastActivity "
                            "FROM Channel ORDER BY %1, DateLastVisited DESC, DateAdded DESC").arg(
                    sortString));
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    while (query.next()) {
        list.append(
                new ChannelListItemModel(query.value(rec.indexOf("ChannelId")).toString(),
                        query.value(rec.indexOf("Title")).toString(),
                        query.value(rec.indexOf("ThumbnailUrl")).toString(),
                        query.value(rec.indexOf("DateAdded")).toInt(),
                        query.value(rec.indexOf("DateLastVisited")).toInt(),
                        query.value(rec.indexOf("LastVideoId")).toString(),
                        query.value(rec.indexOf("LastVideoTitle")).toString(),
                        query.value(rec.indexOf("DateLastActivity")).toInt()));
    }

    return list;
}

QStringList DbHelper::getChannelIdList()
{
    QStringList list;
    QSqlQuery query(getDbContext());

    query.prepare("SELECT ChannelId FROM Channel");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    while (query.next()) {
        list.append(query.value(rec.indexOf("ChannelId")).toString());
    }

    return list;
}

QList<PlaylistListItemModel*> DbHelper::getPlaylistList()
{
    QList<PlaylistListItemModel*> list;
    QSqlQuery query(getDbContext());

    query.prepare(
            "SELECT PlaylistId, Type, Title, DateCreated, DateLastPlayed, YoutubeId, ChannelTitle "
                    "FROM Playlist ORDER BY Type ASC, Title ASC;");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    while (query.next()) {
        list.append(
                new PlaylistListItemModel(query.value(rec.indexOf("PlaylistId")).toInt(),
                        (PlaylistListItemModel::Type) query.value(rec.indexOf("Type")).toInt(),
                        query.value(rec.indexOf("Title")).toString(),
                        query.value(rec.indexOf("DateCreated")).toInt(),
                        query.value(rec.indexOf("DateLastPlayed")).toInt(),
                        query.value(rec.indexOf("YoutubeId")).toString(),
                        query.value(rec.indexOf("ChannelTitle")).toString()));
    }

    return list;
}

PlaylistListItemModel* DbHelper::getPlaylistById(int playlistId)
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "SELECT PlaylistId, Type, Title, DateCreated, DateLastPlayed, YoutubeId, ChannelTitle "
                    "FROM Playlist WHERE PlaylistId = :playlistId;");
    query.bindValue(":playlistId", playlistId);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    if (query.next()) {
        return new PlaylistListItemModel(query.value(rec.indexOf("PlaylistId")).toInt(),
                (PlaylistListItemModel::Type) query.value(rec.indexOf("Type")).toInt(),
                query.value(rec.indexOf("Title")).toString(),
                query.value(rec.indexOf("DateCreated")).toInt(),
                query.value(rec.indexOf("DateLastPlayed")).toInt(),
                query.value(rec.indexOf("YoutubeId")).toString(),
                query.value(rec.indexOf("ChannelTitle")).toString());
    }

    return new PlaylistListItemModel();
}

QList<PlaylistVideoModel*> DbHelper::getAllPlaylistVideoIds()
{
    QList<PlaylistVideoModel*> list;
    QSqlQuery query(getDbContext());

    query.prepare("SELECT VideoId, PlaylistId FROM PlaylistVideo;");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    while (query.next()) {
        PlaylistVideoModel *model = new PlaylistVideoModel();

        model->videoId = query.value(rec.indexOf("VideoId")).toString();
        model->playlistId = query.value(rec.indexOf("PlaylistId")).toInt();

        list.append(model);
    }

    return list;
}

QList<PlaylistVideoModel*> DbHelper::getListByPlaylistId(int playlistId,
        PlaylistVideoModel::SortBy sortBy, PlaylistVideoModel::SortDirection sortOrder)
{
    QList<PlaylistVideoModel*> list;
    QSqlQuery query(getDbContext());

    QString sortString;
    switch (sortBy) {
        case PlaylistVideoModel::DateAdded:
            sortString = "DateAdded";
            break;
        case PlaylistVideoModel::Name:
            sortString = "UPPER(Title)";
            break;
        case PlaylistVideoModel::DateViewed:
            sortString = "DateLastPlayed";
            break;
    }
    sortString += QString(" ") + (sortOrder == PlaylistVideoModel::Asc ? "ASC" : "DESC");
    query.prepare(
            QString(
                    "SELECT VideoId, PlaylistId, Title, ChannelId, ChannelTitle, DateAdded, DateLastPlayed, SortOrder "
                            "FROM PlaylistVideo WHERE PlaylistId = :playlistId ORDER BY %1, videoId ASC;").arg(
                    sortString));
    query.bindValue(":playlistId", playlistId);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    while (query.next()) {
        PlaylistVideoModel *model = new PlaylistVideoModel();

        model->videoId = query.value(rec.indexOf("VideoId")).toString();
        model->playlistId = query.value(rec.indexOf("PlaylistId")).toInt();
        model->title = query.value(rec.indexOf("Title")).toString();
        model->channelId = query.value(rec.indexOf("ChannelId")).toString();
        model->channelTitle = query.value(rec.indexOf("ChannelTitle")).toString();
        model->dateAdded = query.value(rec.indexOf("DateAdded")).toInt();
        model->dateLastPlayed = query.value(rec.indexOf("DateLastPlayed")).toInt();
        model->sortOrder = query.value(rec.indexOf("SortOrder")).toInt();

        list.append(model);
    }

    return list;
}

void DbHelper::createPlaylistVideo(const PlaylistVideoModel *video)
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "INSERT OR IGNORE INTO PlaylistVideo(VideoId, PlaylistId, Title, ChannelId, ChannelTitle, DateAdded, DateLastPlayed, SortOrder) "
                    "VALUES(:videoId, :playlistId, :title, :channelId, :channelTitle, :dateAdded, :dateLastPlayed, :sortOrder);");
    query.bindValue(":videoId", video->videoId);
    query.bindValue(":playlistId", video->playlistId);
    query.bindValue(":title", video->title);
    query.bindValue(":channelId", video->channelId);
    query.bindValue(":channelTitle", video->channelTitle);
    query.bindValue(":dateAdded", video->dateAdded);
    query.bindValue(":dateLastPlayed", video->dateLastPlayed);
    query.bindValue(":sortOrder", video->sortOrder);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::updatePlaylistVideo(const PlaylistVideoModel *video)
{
    QSqlQuery query(getDbContext());

    query.prepare("UPDATE PlaylistVideo SET "
            "Title = :title, "
            "ChannelTitle = :channelTitle, "
            "DateLastPlayed = :dateLastPlayed "
            "WHERE VideoId = :videoId;");
    query.bindValue(":videoId", video->videoId);
    query.bindValue(":title", video->title);
    query.bindValue(":channelTitle", video->channelTitle);
    query.bindValue(":dateLastPlayed", video->dateLastPlayed);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::deletePlaylistVideo(QString videoId, int playlistId)
{
    QSqlQuery query(getDbContext());

    query.prepare(
            "DELETE FROM PlaylistVideo WHERE videoId = :videoId and playlistId = :playlistId;");
    query.bindValue(":videoId", videoId);
    query.bindValue(":playlistId", playlistId);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::deleteAllFromPlaylist(int playlistId)
{
    QSqlQuery query(getDbContext());

    query.prepare("DELETE FROM PlaylistVideo WHERE playlistId = :playlistId;");
    query.bindValue(":playlistId", playlistId);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

QMap<QString, int> DbHelper::getViewedPercents()
{
    QMap<QString, int> map;
    QSqlQuery query(getDbContext());

    query.prepare("SELECT VideoId, [Percent] FROM VideoViewedPercent;");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    QSqlRecord rec = query.record();

    while (query.next()) {
        map.insert(query.value(rec.indexOf("VideoId")).toString(),
                query.value(rec.indexOf("Percent")).toInt());
    }

    return map;
}

void DbHelper::setViewedPercent(QString videoId, int value)
{
    if (value < 0) {
        return;
    }

    QSqlQuery query(getDbContext());

    query.prepare("INSERT OR IGNORE INTO VideoViewedPercent(VideoId, [Percent]) "
            "VALUES(:videoId, :percent);");
    query.bindValue(":videoId", videoId);
    query.bindValue(":percent", value);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "INSERT: SQL reported an error : " << err.text();
    }

    QSqlQuery updquery(getDbContext());
    updquery.prepare(QString("UPDATE VideoViewedPercent SET [Percent] = %1 WHERE [Percent] < %1 AND VideoId = :videoId;").arg(value));
    updquery.bindValue(":videoId", videoId);
    updquery.exec();

    err = updquery.lastError();

    if (err.isValid()) {
        qWarning() << "UPDATE: SQL reported an error : " << err.text() << videoId << value;
    }
}

void DbHelper::deleteViewedPercent(QString videoId)
{
    QSqlQuery query(getDbContext());

    query.prepare("DELETE FROM VideoViewedPercent "
            "WHERE VideoId = :videoId;");
    query.bindValue(":videoId", videoId);
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}

void DbHelper::deleteAllViewedPercents()
{
    QSqlQuery query(getDbContext());

    query.prepare("DELETE FROM VideoViewedPercent;");
    query.exec();

    QSqlError err = query.lastError();

    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }
}
