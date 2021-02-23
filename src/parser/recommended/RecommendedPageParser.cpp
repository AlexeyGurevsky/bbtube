#include "RecommendedPageParser.hpp"
#include "src/parser/models/RecommendedData.hpp"
#include "src/parser/search/ItemRendererParser.hpp"

#include <bb/data/JsonDataAccess>

void RecommendedPageParser::parse(RecommendedData *recommendedData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariantMap map = jda.loadFromBuffer(*json).toMap();

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
                recommendedData->clientVersion = map2["value"].toString();

                break;
            }
        }

        if (recommendedData->clientVersion != "") {
            break;
        }
    }

    QVariantMap gridRendererMap =
            map["contents"].toMap()["twoColumnBrowseResultsRenderer"].toMap()["tabs"].toList()[0].toMap()["tabRenderer"].toMap()["content"].toMap()["richGridRenderer"].toMap();
    QVariantList videosList = gridRendererMap["contents"].toList();

    for (int i = 0; i < videosList.count(); i++) {
        QVariantMap item = videosList[i].toMap();

        if (item.contains("richItemRenderer")) {
            QVariantMap videoMap =
                    item["richItemRenderer"].toMap()["content"].toMap()["videoRenderer"].toMap();
            if (!videoMap.contains("videoId")) {
                continue;
            }

            SingleVideoMetadata video = ItemRendererParser::getVideo(&videoMap);

            recommendedData->videos.append(video);
        } else if (item.contains("continuationItemRenderer")) {
            recommendedData->ctoken =
                    item["continuationItemRenderer"].toMap()["continuationEndpoint"].toMap()["continuationCommand"].toMap()["token"].toString();
        }
    }
}

void RecommendedPageParser::parseNextBatch(RecommendedData *recommendedData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariantList receivedActions =
            jda.loadFromBuffer(*json).toMap()["onResponseReceivedActions"].toList();

    if (receivedActions.count() == 0) {
        return;
    }

    QVariantList videosList =
            receivedActions[0].toMap()["appendContinuationItemsAction"].toMap()["continuationItems"].toList();

    for (int i = 0; i < videosList.count(); i++) {
        QVariantMap item = videosList[i].toMap();

        if (item.contains("richItemRenderer")) {
            QVariantMap videoMap =
                    item["richItemRenderer"].toMap()["content"].toMap()["videoRenderer"].toMap();
            if (!videoMap.contains("videoId")) {
                continue;
            }

            SingleVideoMetadata video = ItemRendererParser::getVideo(&videoMap);

            recommendedData->videos.append(video);
        } else if (item.contains("continuationItemRenderer")) {
            recommendedData->ctoken =
                    item["continuationItemRenderer"].toMap()["continuationEndpoint"].toMap()["continuationCommand"].toMap()["token"].toString();
        }
    }
}
