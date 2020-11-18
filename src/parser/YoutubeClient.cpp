#include "YoutubeClient.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/models/RecommendedData.hpp"
#include "src/parser/models/TrendingData.hpp"

#include "src/applicationui.hpp"
#include "src/parser/script/ScriptData.hpp"
#include "src/parser/script/ScriptParser.hpp"
#include "src/parser/cipher/DecryptHelper.hpp"
#include "src/parser/storage/StorageParser.hpp"
#include "src/parser/models/SearchData.hpp"
#include "src/parser/search/ItemRendererParser.hpp"
#include "src/parser/search/SuggestionsParser.hpp"
#include "src/parser/channel/ChannelPageParser.hpp"
#include "src/parser/recommended/RecommendedPageParser.hpp"
#include "src/parser/trending/TrendingPageParser.hpp"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookie>

QMap<QString, ScriptData> YoutubeClient::cachedScripts;

QString YoutubeClient::getVideoId(QString text)
{
    QRegExp videoIdRegExp(".*(?:youtu.be\\/|v\\/|u\\/\\w\\/|embed\\/|watch\\?v=)([^#\\&\\?]*).*");
    videoIdRegExp.indexIn(text);

    if (videoIdRegExp.cap(1) != "") {
        return videoIdRegExp.cap(1);
    }

    QUrl url(text);
    if (!url.isValid()) {
        return "";
    }

    QString videoId = url.queryItemValue("v");
    if (videoId != "" && url.host().contains("youtube.com")) {
        return videoId;
    }

    return "";
}

void YoutubeClient::process(QString text)
{
    if (text == "") {
        return;
    }

    QString videoId = getVideoId(text);

    if (videoId != "") {
        parse(videoId);

        return;
    }

    search(text);
}

void YoutubeClient::parse(QString videoId)
{
    QNetworkRequest request = prepareRequest("https://www.youtube.com/watch?v=" + videoId);
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onGetHtmlFinished()));
}

void YoutubeClient::search(QString text)
{
    search(text, "");
}

void YoutubeClient::search(QString text, QString searchParams)
{
    QString url = "https://www.youtube.com/results?search_query=" + text;

    if (searchParams != "") {
        url += "&sp=" + searchParams;
    }

    QNetworkRequest request = prepareRequest(url);
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onSearchFinished()));
}

void YoutubeClient::suggestions(QString text)
{
    if (text == "") {
        return;
    }

    QNetworkRequest request = prepareRequest(
            "https://clients1.google.com/complete/search?client=youtube&ds=v&q=" + text);
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onSuggestionsFinished()));
}

void YoutubeClient::channel(QString channelId, QString originalChannelId)
{
    QNetworkRequest request = prepareRequest(
            "https://www.youtube.com/channel/" + channelId + "/videos?view=0");
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);

    reply->setProperty("channelId", channelId);
    reply->setProperty("originalChannelId", originalChannelId);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onChannelFinished()));
}

void YoutubeClient::channelVideosNextBatch(ChannelPageData *channelData)
{
    QNetworkRequest request = prepareRequest(
            "https://www.youtube.com/browse_ajax?ctoken=" + channelData->ctoken + "&continuation="
                    + channelData->ctoken);
    request.setRawHeader("X-YouTube-Client-Name", "1");
    request.setRawHeader("X-YouTube-Client-Version", channelData->clientVersion.toUtf8());

    QNetworkReply *reply = ApplicationUI::networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onChannelVideosNextBatchFinished()));
}

void YoutubeClient::recommended()
{
    QNetworkRequest request = prepareRequest("https://www.youtube.com");
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onRecommendedFinished()));
}

void YoutubeClient::recommendedNextBatch(RecommendedData *recommendedData)
{
    QNetworkRequest request = prepareRequest(
            "https://www.youtube.com/browse_ajax?ctoken=" + recommendedData->ctoken
                    + "&continuation=" + recommendedData->ctoken);
    request.setRawHeader("X-YouTube-Client-Name", "1");
    request.setRawHeader("X-YouTube-Client-Version", recommendedData->clientVersion.toUtf8());

    QNetworkReply *reply = ApplicationUI::networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onRecommendedNextBatchFinished()));
}

void YoutubeClient::trending(QString categoryKey)
{
    QString url = "https://www.youtube.com/feed/trending";
    if (categoryKey != "") {
        url += "?bp=" + categoryKey;
    }
    QNetworkRequest request = prepareRequest(QUrl::fromEncoded(url.toUtf8()).toString());
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onTrendingFinished()));
}

void YoutubeClient::onGetHtmlFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());
    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    int scriptIndex = response.indexOf("src=\"/s/player/");
    int jsUrlStart = scriptIndex + 5;
    int jsUrlEnd = response.indexOf('"', jsUrlStart);
    QString baseJsUrl = "https://www.youtube.com" + response.mid(jsUrlStart, jsUrlEnd - jsUrlStart);
    QString json = getJson(response);

    VideoMetadata videoMetadata;
    ItemRendererParser::populateVideoMetadata(&videoMetadata, &json);

    if (videoMetadata.video.videoId == "") {
        emit error("Source unavailable");
        reply->deleteLater();

        return;
    }

    StorageData storageData;

    QString configKey = "ytplayer.config =";
    int startOfConfig = response.indexOf(configKey);
    if (startOfConfig >= 0) {
        int endOfConfig = response.indexOf(";ytplayer.load", startOfConfig);
        json = response.mid(startOfConfig + configKey.length(),
                endOfConfig - startOfConfig - configKey.length()).trimmed();

        StorageParser::parseFromHtml(&storageData, &json);
    } else {
        configKey = "{\"responseContext\":";
        startOfConfig = response.indexOf(configKey);
        if (startOfConfig >= 0) {
            int endOfConfig = response.indexOf("};", startOfConfig);
            json = response.mid(startOfConfig, endOfConfig + 1 - startOfConfig).trimmed();

            StorageParser::parseFromJson(&storageData, &json);
        } else {
            QEventLoop loop;
            QNetworkRequest getVideoInfoRequest(
                    "https://www.youtube-nocookie.com/get_video_info?video_id="
                            + videoMetadata.video.videoId);
            QNetworkReply *getVideoInfoReply = ApplicationUI::networkManager->get(
                    getVideoInfoRequest);
            QObject::connect(getVideoInfoReply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            QString getVideoInfoResponse = QString(getVideoInfoReply->readAll());
            QStringList split = getVideoInfoResponse.split('&');
            for (int i = 0; i < split.length(); i++) {
                if (split[i].startsWith("player_response=")) {
                    json = QUrl::fromPercentEncoding(
                            split[i].mid(QString("player_response=").length()).toUtf8());

                    StorageParser::parseFromJson(&storageData, &json);
                }
            }
        }
    }

    if (storageData.instances.count() > 0) {
        bool loadJs = false;

        for (int i = 0; i < storageData.instances.count(); i++) {
            if (storageData.instances[i].cipher != "") {
                loadJs = true;

                break;
            }
        }

        if (loadJs && !cachedScripts.contains(baseJsUrl)) {
            QEventLoop loop;
            QNetworkRequest baseJsRequest(baseJsUrl);
            QNetworkReply *baseJsReply = ApplicationUI::networkManager->get(baseJsRequest);
            QObject::connect(baseJsReply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            onGetBaseJsFinished(baseJsReply);
        }

        ScriptData scriptData = loadJs ? cachedScripts[baseJsUrl] : ScriptData();

        for (int i = 0; i < storageData.instances.count(); i++) {
            if (storageData.instances[i].cipher == "") {
                continue;
            }

            storageData.instances[i].url = DecryptHelper::decryptUrl(
                    storageData.instances[i].cipher, &scriptData);
        }

        if (storageData.audio.cipher != "") {
            storageData.audio.url = DecryptHelper::decryptUrl(storageData.audio.cipher,
                    &scriptData);
        }
    }

    emit metadataReceived(videoMetadata, storageData);

    reply->deleteLater();
}

void YoutubeClient::onSearchFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());

    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    QString json = getJson(response);

    SearchData searchData;
    ItemRendererParser::populateSearchData(&searchData, &json);
    emit searchDataReceived(searchData);

    reply->deleteLater();
}

void YoutubeClient::onSuggestionsFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());

    if (reply->error()) {
        //emit error(reply->errorString()); fail silently
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    QString configKey = "window.google.ac.h(";
    int startOfConfig = response.indexOf(configKey);
    QString json = response.mid(startOfConfig + configKey.length(),
            response.length() - startOfConfig - configKey.length() - 1).trimmed();
    QStringList list = SuggestionsParser::parseSuggestions(&json);

    emit suggestionsReceived(list);

    reply->deleteLater();
}

void YoutubeClient::onGetBaseJsFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    ScriptData scriptData = ScriptParser::parse(response);

    cachedScripts.insert(reply->request().url().toString(), scriptData);

    reply->deleteLater();
}

void YoutubeClient::onChannelFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());

    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    QString json = getJson(response);

    ChannelPageData channelData;

    if (reply->property("originalChannelId").toString() != "") {
        channelData.channelId = reply->property("originalChannelId").toString();
    } else {
        channelData.channelId = reply->property("channelId").toString();
    }

    ChannelPageParser::parse(&channelData, &json);

    if (channelData.redirectChannelId != "") {
        channel(channelData.redirectChannelId, channelData.channelId);
        reply->deleteLater();

        return;
    }

    if (channelData.title != "") {
        emit channelDataReceived(channelData);
    } else {
        emit error("Channel not found");
    }

    reply->deleteLater();
}

void YoutubeClient::onChannelVideosNextBatchFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());
    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    ChannelPageData channelData;
    ChannelPageParser::parseNextBatch(&channelData, &response);

    emit channelVideosNextBatchReceived(channelData);

    reply->deleteLater();
}

void YoutubeClient::onRecommendedFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());

    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    QString json = getJson(response);

    RecommendedData recommendedData;
    RecommendedPageParser::parse(&recommendedData, &json);
    emit recommendedDataReceived(recommendedData);

    reply->deleteLater();
}

void YoutubeClient::onRecommendedNextBatchFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());
    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    RecommendedData recommendedData;

    RecommendedPageParser::parseNextBatch(&recommendedData, &response);
    emit recommendedNextBatchReceived(recommendedData);

    reply->deleteLater();
}

void YoutubeClient::onTrendingFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(QObject::sender());

    if (reply->error()) {
        emit error(reply->errorString());
        reply->deleteLater();

        return;
    }

    QString response = QString(reply->readAll());
    QString json = getJson(response);

    TrendingData trendingData;
    TrendingPageParser::parse(&trendingData, &json);
    emit trendingDataReceived(trendingData);

    reply->deleteLater();
}

QNetworkRequest YoutubeClient::prepareRequest(QString url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent",
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");

    return request;
}

QString YoutubeClient::getJson(QString response)
{
    QString json;
    QString configKey = "var ytInitialData = ";
    int startOfConfig = response.indexOf(configKey);

    if (startOfConfig >= 0) {
        int endOfConfig = response.indexOf("};", startOfConfig);
        json = response.mid(startOfConfig + configKey.length(),
                endOfConfig + 1 - startOfConfig - configKey.length()).trimmed();
    } else {
        configKey = "window[\"ytInitialData\"] =";
        int startOfConfig = response.indexOf(configKey);
        int endOfConfig = response.indexOf("window[\"ytInitialPlayerResponse\"]", startOfConfig);
        json = response.mid(startOfConfig + configKey.length(),
                endOfConfig - startOfConfig - configKey.length()).trimmed();
    }

    return json;
}
