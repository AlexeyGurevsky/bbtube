#ifndef TimecodeListItemProvider_HPP_
#define TimecodeListItemProvider_HPP_

#include "TimecodeListItemModel.hpp"

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>
#include <bb/cascades/StandardListItem>

using namespace bb::cascades;

class TimecodeListItemProvider: public ListItemProvider
{
public:
    TimecodeListItemProvider()
    {
    }
    virtual ~TimecodeListItemProvider()
    {
    }

    VisualNode * createItem(ListView* list, const QString &type)
    {
        return new StandardListItem();
    }

    void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        Q_UNUSED(list);
        Q_UNUSED(indexPath);

        TimecodeListItemModel *model = data.value<TimecodeListItemModel*>();
        StandardListItem *item = static_cast<StandardListItem*>(listItem);

        item->setTitle(model->text);
        item->setDescription(model->isPlaying ? "Playing" : "");
    }
};

#endif /* TimecodeListItemProvider_HPP_ */
