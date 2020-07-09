#ifndef SCRIPTDATA_HPP_
#define SCRIPTDATA_HPP_

#include "src/parser/cipher/CipherOperations.hpp"

class ScriptData
{
public:
    QList<CipherOperation*> cipherOperations;
};

#endif /* SCRIPTDATA_HPP_ */
