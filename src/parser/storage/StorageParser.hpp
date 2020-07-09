#ifndef STORAGEPARSER_HPP_
#define STORAGEPARSER_HPP_

#include "src/parser/models/StorageData.hpp"

class StorageParser
{
private:
    static void parseFromJsonInternal(StorageData *storageData, const QVariantMap *playerResponseMap);
public:
    static void parseFromHtml(StorageData *storageData, QString *json);
    static void parseFromJson(StorageData *storageData, QString *json);
};


#endif /* STORAGEPARSER_HPP_ */
