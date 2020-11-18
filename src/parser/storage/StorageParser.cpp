#include "StorageParser.hpp"
#include "src/parser/models/StorageData.hpp"

#include <bb/data/JsonDataAccess>
#include <QUrl>
void StorageParser::parseFromHtml(StorageData *storageData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariant parsed = jda.loadFromBuffer(*json);
    QVariantMap map = parsed.toMap();
    QString playerResponse = map["args"].toMap()["player_response"].toString();
    QVariantMap playerResponseMap = jda.loadFromBuffer(playerResponse).toMap();

    parseFromJsonInternal(storageData, &playerResponseMap);
}

void StorageParser::parseFromJson(StorageData *storageData, QString *json)
{
    bb::data::JsonDataAccess jda;
    QVariant parsed = jda.loadFromBuffer(*json);
    QVariantMap playerResponseMap = parsed.toMap();

    parseFromJsonInternal(storageData, &playerResponseMap);
}

void StorageParser::parseFromJsonInternal(StorageData *storageData,
        const QVariantMap *playerResponseMap)
{
    QVariantMap streamingData = (*playerResponseMap)["streamingData"].toMap();

    if (streamingData.contains("hlsManifestUrl")) { // live stream
        SingleVideoStorageData video;

        video.url = QUrl::fromPercentEncoding(streamingData["hlsManifestUrl"].toByteArray());
        video.duration = 0;

        storageData->instances.append(video);

        return;
    }

    QVariantList formats = streamingData["formats"].toList();

    for (int i = 0; i < formats.count(); i++) {
        QVariantMap format = formats[i].toMap();
        SingleVideoStorageData video;

        video.quality = format["qualityLabel"].toString();
        video.duration = format["approxDurationMs"].toInt();
        video.contentLength = format["contentLength"].toULongLong();

        if (format.contains("url")) {
            video.url = QUrl::fromPercentEncoding(format["url"].toByteArray());
        } else if (format.contains("cipher")) {
            video.cipher = format["cipher"].toString();
        } else if (format.contains("signatureCipher")) {
            video.cipher = format["signatureCipher"].toString();
        }
        storageData->instances.append(video);
    }

    QVariantList adaptiveFormats = streamingData["adaptiveFormats"].toList();

    for (int i = 0; i < adaptiveFormats.count(); i++) {
        QVariantMap format = adaptiveFormats[i].toMap();

        if (format["mimeType"].toString().indexOf("audio/mp4") < 0) {
            continue;
        }

        AudioStorageData audio;

        if (format.contains("url")) {
            audio.url = QUrl::fromPercentEncoding(format["url"].toByteArray());
        } else if (format.contains("cipher")) {
            audio.cipher = format["cipher"].toString();
        } else if (format.contains("signatureCipher")) {
            audio.cipher = format["signatureCipher"].toString();
        }

        audio.contentLength = format["contentLength"].toULongLong();

        storageData->audio = audio;
    }

    QVariantList captionList =
            (*playerResponseMap)["captions"].toMap()["playerCaptionsTracklistRenderer"].toMap()["captionTracks"].toList();
    for (int i = 0; i < captionList.count(); i++) {
        QVariantMap captionMap = captionList[i].toMap();
        ClosedCaptionData cc;

        cc.isLoaded = false;
        cc.languageCode = captionMap["vssId"].toString().replace(".", "");
        cc.languageName = captionMap["name"].toMap()["simpleText"].toString();
        cc.url = captionMap["baseUrl"].toString() + "&fmt=ttml";

        storageData->captions.append(cc);
    }
}
