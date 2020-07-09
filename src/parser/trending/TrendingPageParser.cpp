#include "TrendingPageParser.hpp"
#include "src/parser/models/TrendingData.hpp"
#include "src/parser/search/ItemRendererParser.hpp"

#include <bb/data/JsonDataAccess>

void TrendingPageParser::parse(TrendingData *trendingData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariantMap map = jda.loadFromBuffer(*json).toMap();

    QVariantMap sectionListRendererMap =
            map["contents"].toMap()["twoColumnBrowseResultsRenderer"].toMap()["tabs"].toList()[0].toMap()["tabRenderer"].toMap()["content"].toMap()["sectionListRenderer"].toMap();
    QVariantList contentsList = sectionListRendererMap["contents"].toList();

    for (int i = 0; i < contentsList.count(); i++) {
        QVariantList videoList =
                contentsList[i].toMap()["itemSectionRenderer"].toMap()["contents"].toList()[0].toMap()["shelfRenderer"].toMap()["content"].toMap()["expandedShelfContentsRenderer"].toMap()["items"].toList();

        for (int j = 0; j < videoList.count(); j++) {
            QVariantMap videoMap = videoList[j].toMap()["videoRenderer"].toMap();
            if (!videoMap.contains("videoId")) {
                continue;
            }

            SingleVideoMetadata video = ItemRendererParser::getVideo(&videoMap);

            trendingData->videos.append(video);
        }
    }

    if (sectionListRendererMap.contains("subMenu")) {
        QVariantList categoryList = sectionListRendererMap["subMenu"].toMap()["channelListSubMenuRenderer"].toMap()["contents"].toList();

        for (int i = 0; i < categoryList.count(); i++) {
            QVariantMap categoryMap = categoryList[i].toMap()["channelListSubMenuAvatarRenderer"].toMap();
            TrendingDataCategory category;

            category.title = categoryMap["title"].toMap()["simpleText"].toString();
            category.categoryKey = categoryMap["navigationEndpoint"].toMap()["browseEndpoint"].toMap()["params"].toString();

            trendingData->categories.append(category);
        }
    }
}
