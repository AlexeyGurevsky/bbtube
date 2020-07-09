#ifndef SEARCHDATA_HPP_
#define SEARCHDATA_HPP_

#include "ChannelData.hpp"
#include "VideoMetadata.hpp"

class SearchDataSection
{
public:
    QString title;
    QList<SingleVideoMetadata> videos;
};

class SearchParamOption
{
public:
    QString title;
    bool enabled;
    bool selected;
    QString urlParams;
};

class SearchParamGroup
{
public:
    QString title;
    QString paramsToReset;
    bool isRemovable;
    QList<SearchParamOption> options;
};

class SearchData
{
public:
    QList<SingleVideoMetadata> videos;
    QList<ChannelData> channels;
    QList<SearchDataSection> sections;
    QList<SearchParamGroup> searchParamGroups;
};

#endif /* SEARCHDATA_HPP_ */
