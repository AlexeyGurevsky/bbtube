#ifndef SuggestionsParser_HPP_
#define SuggestionsParser_HPP_

#include <bb/data/JsonDataAccess>
#include <QStringList>

class SuggestionsParser
{
public:
    static QStringList parseSuggestions(const QString *json)
    {
        bb::data::JsonDataAccess jda;
        QVariantList list = jda.loadFromBuffer(*json).toList()[1].toList();
        QStringList result;

        for (int i = 0; i < list.size(); i++) {
            result.append(list[i].toList()[0].toString());
        }

        return result;
    }
};

#endif /* SuggestionsParser_HPP_ */
