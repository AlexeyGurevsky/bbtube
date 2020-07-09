#ifndef STORAGEDATA_HPP_
#define STORAGEDATA_HPP_

class SingleVideoStorageData
{
public:
    QString url;
    QString cipher;
    QString quality;
    int duration;
    unsigned long long contentLength;
};

class AudioStorageData
{
public:
    QString url;
    QString cipher;
    unsigned long long contentLength;
};

class ClosedCaptionData
{
public:
    QString url;
    QString languageCode;
    QString languageName;
    bool isLoaded;
};

class StorageData
{
public:
    QList<SingleVideoStorageData> instances;
    AudioStorageData audio;
    QList<ClosedCaptionData> captions;
};

#endif /* STORAGEDATA_HPP_ */
