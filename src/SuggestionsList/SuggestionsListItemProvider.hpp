#ifndef SuggestionsListItemProvider_HPP_
#define SuggestionsListItemProvider_HPP_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>
#include <bb/cascades/Header>
#include <bb/cascades/StandardListItem>

using namespace bb::cascades;

class SuggestionsListItemProvider: public ListItemProvider
{
public:
    SuggestionsListItemProvider()
    {
    }
    virtual ~SuggestionsListItemProvider()
    {
    }

    VisualNode * createItem(ListView* list, const QString &type)
    {
        Q_UNUSED(list);

        if (type.compare(GroupDataModel::Header) == 0) {
            return new Header();
        } else {
            return new StandardListItem();
        }
    }

    void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        Q_UNUSED(list);
        Q_UNUSED(indexPath);

        if (type.compare(GroupDataModel::Header) == 0) {
            static_cast<Header*>(listItem)->setTitle("Suggestions (tap here to hide)");
        } else {
            QVariantMap map = data.value<QVariantMap>();
            static_cast<StandardListItem*>(listItem)->setTitle(map["item"].toString());
        }
    }
};

#endif /* SuggestionsListItemProvider_HPP_ */
