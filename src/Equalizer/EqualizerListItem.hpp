#ifndef EQUALIZERLISTITEM_HPP_
#define EQUALIZERLISTITEM_HPP_

#include "EqualizerListItemModel.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/CustomListItem>
#include <bb/cascades/ListView>
#include <bb/cascades/ImageView>
#include <bb/cascades/UIConfig>

using namespace bb::cascades;

class EqualizerListItem: public CustomListItem, public ListItemListener
{
    Q_OBJECT
public:
    EqualizerListItem(Container *parent = 0);
    virtual ~EqualizerListItem()
    {
    }

    void update(const EqualizerListItemModel *item);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);
private:
    Label *title;
    UIConfig *ui;
    ImageView *selectedIcon;
};

#endif /* EQUALIZERLISTITEM_HPP_ */
