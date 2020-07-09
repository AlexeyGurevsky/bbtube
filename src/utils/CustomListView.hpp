#ifndef CustomListView_HPP_
#define CustomListView_HPP_
#include <bb/cascades/ListView>
#include <QVariantList>

class CustomListView: public bb::cascades::ListView
{
public:
    CustomListView() :
            bb::cascades::ListView()
    {
    }

    void trigger(QVariantList indexPath)
    {
        emit
        this->triggered(indexPath);
    }
};
#endif /* CustomListView_HPP_ */
