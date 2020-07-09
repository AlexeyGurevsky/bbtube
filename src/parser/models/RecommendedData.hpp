#ifndef RECOMMENDEDDATA_HPP_
#define RECOMMENDEDDATA_HPP_

#include "VideoMetadata.hpp"

class RecommendedData
{
public:
    QString clientVersion;
    QList<SingleVideoMetadata> videos;
    QString ctoken;
};

#endif /* RECOMMENDEDDATA_HPP_ */
