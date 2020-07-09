#ifndef EqualizerListItemProvider_HPP_
#define EqualizerListItemProvider_HPP_

#include "EqualizerListItem.hpp"
#include "EqualizerListItemModel.hpp"

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>

using namespace bb::cascades;

class EqualizerListItemProvider: public ListItemProvider
{
public:
    EqualizerListItemProvider()
    {
    }
    virtual ~EqualizerListItemProvider()
    {
    }

    VisualNode * createItem(ListView* list, const QString &type)
    {
        Q_UNUSED(list);

        return new EqualizerListItem();
    }

    void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        Q_UNUSED(list);
        Q_UNUSED(indexPath);

        EqualizerListItemModel *model = data.value<EqualizerListItemModel *>();
        static_cast<EqualizerListItem*>(listItem)->update(model);
    }
};

#endif /* EqualizerListItemProvider_HPP_ */
