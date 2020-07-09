#ifndef RecommendedPageParser_HPP_
#define RecommendedPageParser_HPP_

#include "src/parser/models/RecommendedData.hpp"

class RecommendedPageParser
{
public:
    static void parse(RecommendedData *recommendedData, QString *json);
    static void parseNextBatch(RecommendedData *recommendedData, QString *json);
};


#endif /* RecommendedPageParser_HPP_ */
