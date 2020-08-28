#ifndef ITEMRENDERERPARSER_HPP_
#define ITEMRENDERERPARSER_HPP_

#include "src/parser/models/ChannelData.hpp"
#include "src/parser/models/SearchData.hpp"
#include "src/parser/models/VideoMetadata.hpp"

#include <bb/data/JsonDataAccess>
#include <QStringList>

class ItemRendererParser
{
private:
    static ChannelData getChannel(QVariantMap *map)
    {
        ChannelData data;

        data.channelId = (*map)["channelId"].toString();
        data.thumbnailUrl =
                (*map)["thumbnail"].toMap()["thumbnails"].toList()[0].toMap()["url"].toString();
        data.title = (*map)["title"].toMap()["simpleText"].toString();

        return data;
    }

    static SingleVideoMetadata getFromCompactVideo(QVariantMap *map)
    {
        SingleVideoMetadata data;

        data.videoId = (*map)["videoId"].toString();
        data.channelId = (*map)["channelId"].toString();
        data.channelTitle =
                (*map)["shortBylineText"].toMap()["runs"].toList()[0].toMap()["text"].toString();
        data.dateUploadedAgo = (*map)["publishedTimeText"].toMap()["simpleText"].toString();
        data.thumbnailUrl =
                (*map)["thumbnail"].toMap()["thumbnails"].toList()[0].toMap()["url"].toString();
        data.title = (*map)["title"].toMap()["simpleText"].toString();
        data.viewsCount = (*map)["viewCountText"].toMap()["simpleText"].toString();
        data.shortViewsCount = (*map)["shortViewCountText"].toMap()["simpleText"].toString();
        data.lengthText = (*map)["lengthText"].toMap()["simpleText"].toString();

        return data;
    }

    static SearchDataSection getSection(QVariantMap *map)
    {
        SearchDataSection data;

        data.title = (*map)["title"].toMap()["simpleText"].toString();
        QVariantList items =
                (*map)["content"].toMap()["verticalListRenderer"].toMap()["items"].toList();

        for (int i = 0; i < items.size(); i++) {
            QVariantMap tempMap = items[i].toMap()["videoRenderer"].toMap();
            data.videos.append(getVideo(&tempMap));
        }
        return data;
    }

    static QString getTitleFromRuns(QVariantList runs)
    {
        QStringList parts;

        for (int i = 0; i < runs.count(); i++) {
            parts.append(runs[i].toMap()["text"].toString());
        }

        return parts.join("");
    }
public:
    static void populateSearchData(SearchData *searchData, const QString *json)
    {
        bb::data::JsonDataAccess jda;
        QVariantMap map = jda.loadFromBuffer(*json).toMap();
        QVariantMap sectionListRenderer =
                map["contents"].toMap()["twoColumnSearchResultsRenderer"].toMap()["primaryContents"].toMap()["sectionListRenderer"].toMap();
        QVariantList contentsList = sectionListRenderer["contents"].toList();
        QVariantList contents;

        for (int i = 0; i < contentsList.size(); i++) {
            QVariantMap contentsItem = contentsList[i].toMap();

            if (contentsItem.contains("itemSectionRenderer")) {
                contents = contentsItem["itemSectionRenderer"].toMap()["contents"].toList();
            }
        }

        for (int i = 0; i < contents.size(); i++) {
            QVariantMap item = contents[i].toMap();
            QString key = item.keys()[0];
            QVariantMap tempMap = item[key].toMap();
            if (key == "channelRenderer") {
                searchData->channels.append(getChannel(&tempMap));
            } else if (key == "shelfRenderer") {
                searchData->sections.append(getSection(&tempMap));
            } else if (key == "videoRenderer") {
                SingleVideoMetadata video = getVideo(&tempMap);

                searchData->videos.append(video);
            }
        }

        QVariantList searchParamsGroups =
                sectionListRenderer["subMenu"].toMap()["searchSubMenuRenderer"].toMap()["groups"].toList();
        for (int i = 0; i < searchParamsGroups.count(); i++) {
            QVariantMap renderer =
                    searchParamsGroups[i].toMap()["searchFilterGroupRenderer"].toMap();
            SearchParamGroup group;
            QVariantList filters = renderer["filters"].toList();

            group.title = renderer["title"].toMap()["simpleText"].toString();
            group.isRemovable = false;
            for (int j = 0; j < filters.count(); j++) {
                QVariantMap filterRenderer = filters[j].toMap()["searchFilterRenderer"].toMap();
                SearchParamOption option;

                option.title = filterRenderer["label"].toMap()["simpleText"].toString();

                if (filterRenderer["status"].toString() == "FILTER_STATUS_DISABLED") {
                    option.enabled = false;
                    option.selected = false;
                    option.urlParams = "";
                } else if (filterRenderer["status"].toString() == "FILTER_STATUS_SELECTED") {
                    option.enabled = true;
                    option.selected = true;
                    option.urlParams = "";
                    if (filterRenderer.contains("navigationEndpoint")) {
                        group.paramsToReset =
                                filterRenderer["navigationEndpoint"].toMap()["searchEndpoint"].toMap()["params"].toString();
                        group.isRemovable = true;
                    }
                } else {
                    option.enabled = true;
                    option.selected = false;
                    if (filterRenderer.contains("navigationEndpoint")) {
                        option.urlParams =
                                filterRenderer["navigationEndpoint"].toMap()["searchEndpoint"].toMap()["params"].toString();
                    }
                }

                group.options.append(option);
            }

            searchData->searchParamGroups.append(group);
        }
    }

    static void populateVideoMetadata(VideoMetadata *videoMetadata, const QString *json)
    {
        bb::data::JsonDataAccess jda;
        QVariantMap jsonMap = jda.loadFromBuffer(*json).toMap();

        if (!jsonMap.contains("contents")) {
            return;
        }

        QVariantMap map = jsonMap["contents"].toMap()["twoColumnWatchNextResults"].toMap();
        QVariantList primaryInfo = map["results"].toMap()["results"].toMap()["contents"].toList();

        int videoPrimaryIndex;
        int videoSecondaryIndex;

        for (int i = 0; i < primaryInfo.count(); i++) {
            QVariantMap tempMap = primaryInfo[i].toMap();
            if (tempMap.contains("videoPrimaryInfoRenderer")) {
                videoPrimaryIndex = i;
            } else if (tempMap.contains("videoSecondaryInfoRenderer")) {
                videoSecondaryIndex = i;
            }
        }

        videoMetadata->video.channelId =
                primaryInfo[videoSecondaryIndex].toMap()["videoSecondaryInfoRenderer"].toMap()["owner"].toMap()["videoOwnerRenderer"].toMap()["navigationEndpoint"].toMap()["browseEndpoint"].toMap()["browseId"].toString();
        videoMetadata->video.channelTitle =
                primaryInfo[videoSecondaryIndex].toMap()["videoSecondaryInfoRenderer"].toMap()["owner"].toMap()["videoOwnerRenderer"].toMap()["title"].toMap()["runs"].toList()[0].toMap()["text"].toString();
        videoMetadata->video.dateUploadedAgo =
                primaryInfo[videoPrimaryIndex].toMap()["videoPrimaryInfoRenderer"].toMap()["dateText"].toMap()["simpleText"].toString();
        videoMetadata->video.title =
                getTitleFromRuns(
                        primaryInfo[videoPrimaryIndex].toMap()["videoPrimaryInfoRenderer"].toMap()["title"].toMap()["runs"].toList());
        videoMetadata->video.videoId =
                jsonMap["currentVideoEndpoint"].toMap()["watchEndpoint"].toMap()["videoId"].toString();
        videoMetadata->video.viewsCount =
                primaryInfo[videoPrimaryIndex].toMap()["videoPrimaryInfoRenderer"].toMap()["viewCount"].toMap()["videoViewCountRenderer"].toMap()["viewCount"].toMap()["simpleText"].toString();
        videoMetadata->video.thumbnailUrl = "";
        videoMetadata->video.lengthText = "";
        videoMetadata->video.shortViewsCount = "";

        QVariantList secondaryResults =
                map["secondaryResults"].toMap()["secondaryResults"].toMap()["results"].toList();

        for (int i = 0; i < secondaryResults.size(); i++) {
            QVariantMap item = secondaryResults[i].toMap();
            QString key = item.keys()[0];
            if (key == "compactAutoplayRenderer") {
                QVariantMap tempMap =
                        item[key].toMap()["contents"].toList()[0].toMap()["compactVideoRenderer"].toMap();
                SingleVideoMetadata video = getFromCompactVideo(&tempMap);

                videoMetadata->relatedVideos.nextVideo = video;
            } else if (key == "compactVideoRenderer") {
                QVariantMap tempMap = item[key].toMap();
                SingleVideoMetadata video = getFromCompactVideo(&tempMap);

                videoMetadata->relatedVideos.otherVideos.append(video);
            }
        }

        QVariantList descriptionRunList =
                primaryInfo[videoSecondaryIndex].toMap()["videoSecondaryInfoRenderer"].toMap()["description"].toMap()["runs"].toList();
        int i = 0;
        while (i < descriptionRunList.count()) {
            QVariantMap descriptionRun = descriptionRunList[i].toMap();
            QVariantMap watchEndpoint =
                    descriptionRun["navigationEndpoint"].toMap()["watchEndpoint"].toMap();
            Timecode timecode;
            QStringList descriptionParts;

            if (watchEndpoint.contains("startTimeSeconds")) {
                timecode.seconds = watchEndpoint["startTimeSeconds"].toInt();
                timecode.time = descriptionRun["text"].toString();

                int j = i + 1;
                while (j < descriptionRunList.count()) {
                    QVariantMap nextDescriptionRun = descriptionRunList[j].toMap();
                    QString currText = nextDescriptionRun["text"].toString();
                    bool stop = currText.contains('\n');

                    descriptionParts.append(
                            currText.left(stop ? currText.indexOf('\n') : currText.length()));

                    if (stop || j == descriptionRunList.count() - 1) {
                        break;
                    }

                    j++;
                }

                timecode.description = descriptionParts.join("");
                if (timecode.description.length() > 1
                        && (timecode.description[0] == ')' || timecode.description[0] == ']')) {
                    timecode.description = timecode.description.mid(1);
                }

                videoMetadata->timecodes.append(timecode);
                descriptionParts.clear();
                i = j + 1;
            } else {
                i++;
            }
        }
    }

    static SingleVideoMetadata getFromChannelVideo(QVariantMap *map)
    {
        SingleVideoMetadata data;

        data.videoId = (*map)["videoId"].toString();

        QVariantMap titleMap = (*map)["title"].toMap();

        if (titleMap.contains("simpleText")) {
            data.title = titleMap["simpleText"].toString();
        } else {
            data.title = getTitleFromRuns(titleMap["runs"].toList());
        }

        data.dateUploadedAgo = (*map)["publishedTimeText"].toMap()["simpleText"].toString();
        data.shortViewsCount = (*map)["shortViewCountText"].toMap()["simpleText"].toString();
        data.lengthText =
                (*map)["thumbnailOverlays"].toList()[0].toMap()["thumbnailOverlayTimeStatusRenderer"].toMap()["text"].toMap()["simpleText"].toString();

        return data;
    }

    static SingleVideoMetadata getVideo(QVariantMap *map)
    {
        SingleVideoMetadata data;

        data.videoId =
                (*map)["navigationEndpoint"].toMap()["watchEndpoint"].toMap()["videoId"].toString();
        data.channelId =
                (*map)["ownerText"].toMap()["runs"].toList()[0].toMap()["navigationEndpoint"].toMap()["browseEndpoint"].toMap()["browseId"].toString();
        data.channelTitle =
                (*map)["ownerText"].toMap()["runs"].toList()[0].toMap()["text"].toString();
        data.dateUploadedAgo = (*map)["publishedTimeText"].toMap()["simpleText"].toString();
        data.thumbnailUrl =
                (*map)["thumbnail"].toMap()["thumbnails"].toList()[0].toMap()["url"].toString();
        data.title = (*map)["title"].toMap()["runs"].toList()[0].toMap()["text"].toString();
        data.viewsCount = (*map)["viewCountText"].toMap()["simpleText"].toString();
        data.shortViewsCount = (*map)["shortViewCountText"].toMap()["simpleText"].toString();
        data.lengthText = (*map)["lengthText"].toMap()["simpleText"].toString();

        return data;
    }
};

#endif /* ITEMRENDERERPARSER_HPP_ */
