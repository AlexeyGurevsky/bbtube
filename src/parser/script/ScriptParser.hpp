#ifndef SCRIPTPARSER_HPP_
#define SCRIPTPARSER_HPP_

#include "ScriptData.hpp"

class ScriptParser
{
public:
    static ScriptData parse(QString script);
};


#endif /* SCRIPTPARSER_HPP_ */
