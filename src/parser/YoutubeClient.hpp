#ifndef YOUTUBECLIENT_HPP_
#define YOUTUBECLIENT_HPP_

#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/SearchData.hpp"
#include "src/parser/models/RecommendedData.hpp"
#include "src/parser/models/TrendingData.hpp"
#include "src/parser/script/ScriptData.hpp"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QStringList>

class YoutubeClient: public QObject
{
    Q_OBJECT
public:
    YoutubeClient(QObject *parent = 0) :
            QObject(parent)
    {
    }

    static QString getVideoId(QString text);
    void process(QString text);
    void suggestions(QString text);
    void search(QString text, QString searchParams);
    void channel(QString channelId, QString originalChannelId = "");
    void channelVideosNextBatch(ChannelPageData *channelData);
    void recommended();
    void recommendedNextBatch(RecommendedData *recommendedData);
    void trending(QString categoryKey = "");
signals:
    void error(QString message);
    void metadataReceived(VideoMetadata videoMetadata, StorageData storageData);
    void searchDataReceived(SearchData data);
    void suggestionsReceived(QStringList suggestions);
    void channelDataReceived(ChannelPageData channelData);
    void channelVideosNextBatchReceived(ChannelPageData channelData);
    void recommendedDataReceived(RecommendedData recommendedData);
    void trendingDataReceived(TrendingData trendingData);
    void recommendedNextBatchReceived(RecommendedData recommendedData);
private slots:
    void onGetHtmlFinished();
    void onSearchFinished();
    void onSuggestionsFinished();
    void onChannelFinished();
    void onChannelVideosNextBatchFinished();
    void onRecommendedFinished();
    void onRecommendedNextBatchFinished();
    void onTrendingFinished();
private:
    static QMap<QString, ScriptData> cachedScripts;
    void search(QString text);
    void parse(QString videoId);
    QNetworkRequest prepareRequest(QString url);
    QString getJson(QString response);
    void onGetBaseJsFinished(QNetworkReply *reply);
};

#endif /* YOUTUBECLIENT_HPP_ */
