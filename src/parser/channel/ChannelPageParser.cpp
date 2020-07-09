#include "src/parser/channel/ChannelPageParser.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/search/ItemRendererParser.hpp"

#include <bb/data/JsonDataAccess>

void ChannelPageParser::parse(ChannelPageData *channelData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariantMap map = jda.loadFromBuffer(*json).toMap();

    if (map.contains("onResponseReceivedActions")) {
        // handle channel redirect
        channelData->redirectChannelId =
                map["onResponseReceivedActions"].toList()[0].toMap()["navigateAction"].toMap()["endpoint"].toMap()["browseEndpoint"].toMap()["browseId"].toString();

        return;
    }

    if (!map.contains("contents")) {
        return;
    }

    QVariantMap headerMap = map["header"].toMap()["c4TabbedHeaderRenderer"].toMap();

    channelData->title = headerMap["title"].toString();
    channelData->thumbnailUrl =
            headerMap["avatar"].toMap()["thumbnails"].toList()[0].toMap()["url"].toString();

    QVariantList serviceTrackingParams =
            map["responseContext"].toMap()["serviceTrackingParams"].toList();

    for (int i = 0; i < serviceTrackingParams.count(); i++) {
        QVariantMap map1 = serviceTrackingParams[i].toMap();
        if (map1["service"].toString() != "CSI") {
            continue;
        }

        QVariantList paramsList = map1["params"].toList();

        for (int j = 0; j < paramsList.count(); j++) {
            QVariantMap map2 = paramsList[j].toMap();

            if (map2["key"].toString() == "cver") {
                channelData->clientVersion = map2["value"].toString();

                break;
            }
        }

        if (channelData->clientVersion != "") {
            break;
        }
    }

    QVariantList tabList =
            map["contents"].toMap()["twoColumnBrowseResultsRenderer"].toMap()["tabs"].toList();
    QVariantMap videosTab;

    for (int i = 0; i < tabList.count(); i++) {
        QVariantMap tab = tabList[i].toMap()["tabRenderer"].toMap();

        if (tab.contains("content")) {
            videosTab = tab;
        }
    }

    QVariantMap gridRendererMap =
            videosTab["content"].toMap()["sectionListRenderer"].toMap()["contents"].toList()[0].toMap()["itemSectionRenderer"].toMap()["contents"].toList()[0].toMap()["gridRenderer"].toMap();
    QVariantList videosList = gridRendererMap["items"].toList();

    for (int i = 0; i < videosList.count(); i++) {
        QVariantMap videoMap = videosList[i].toMap()["gridVideoRenderer"].toMap();
        SingleVideoMetadata video = ItemRendererParser::getFromChannelVideo(&videoMap);

        video.channelId = channelData->channelId;
        video.channelTitle = channelData->title;

        channelData->videos.append(video);
    }

    if (gridRendererMap.contains("continuations")) {
        channelData->ctoken =
                gridRendererMap["continuations"].toList()[0].toMap()["nextContinuationData"].toMap()["continuation"].toString();
    }
}

void ChannelPageParser::parseNextBatch(ChannelPageData *channelData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariantMap map = jda.loadFromBuffer(*json).toList()[1].toMap();
    QVariantMap gridMap =
            map["response"].toMap()["continuationContents"].toMap()["gridContinuation"].toMap();
    QVariantList videosList = gridMap["items"].toList();

    if (gridMap.contains("continuations")) {
        channelData->ctoken =
                gridMap["continuations"].toList()[0].toMap()["nextContinuationData"].toMap()["continuation"].toString();
    }

    for (int i = 0; i < videosList.count(); i++) {
        QVariantMap videoMap = videosList[i].toMap()["gridVideoRenderer"].toMap();
        SingleVideoMetadata video = ItemRendererParser::getFromChannelVideo(&videoMap);

        channelData->videos.append(video);
    }
}
