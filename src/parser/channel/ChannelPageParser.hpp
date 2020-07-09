#ifndef CHANNELPAGEPARSER_HPP_
#define CHANNELPAGEPARSER_HPP_

#include "src/parser/models/ChannelData.hpp"

class ChannelPageParser
{
public:
    static void parse(ChannelPageData *channelData, QString *json);
    static void parseNextBatch(ChannelPageData *channelData, QString *json);
};


#endif /* CHANNELPAGEPARSER_HPP_ */
