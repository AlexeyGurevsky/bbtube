#ifndef TrendingData_HPP_
#define TrendingData_HPP_

#include "VideoMetadata.hpp"

class TrendingDataCategory
{
public:
    QString title;
    QString categoryKey;
};

class TrendingData
{
public:
    QList<SingleVideoMetadata> videos;
    QList<TrendingDataCategory> categories;
};

#endif /* TrendingData_HPP_ */
