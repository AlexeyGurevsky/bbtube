#ifndef TrendingPageParser_HPP_
#define TrendingPageParser_HPP_

#include "src/parser/models/TrendingData.hpp"

class TrendingPageParser
{
public:
    static void parse(TrendingData *trendingData, QString *json);
};


#endif /* TrendingPageParser_HPP_ */
