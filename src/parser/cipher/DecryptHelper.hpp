#ifndef DECRYPTHELPER_HPP_
#define DECRYPTHELPER_HPP_

#include "src/parser/script/ScriptData.hpp"

class DecryptHelper
{
public:
    static QString decryptUrl(QString encryptedUrl, const ScriptData *scriptData);
};


#endif /* DECRYPTHELPER_HPP_ */
