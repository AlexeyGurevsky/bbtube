#ifndef UpdatableDataModel_HPP_
#define UpdatableDataModel_HPP_
#include <bb/cascades/QListDataModel>
#include <QVariantList>

template<class T>
class UpdatableDataModel: public bb::cascades::QListDataModel<T>
{
public:
    UpdatableDataModel() :
            bb::cascades::QListDataModel<T>()
    {
    }
    UpdatableDataModel(const QList<T> &other) :
            bb::cascades::QListDataModel<T>(other)
    {
    }

    void updateItem(QVariantList indexPath)
    {
        emit
        this->itemUpdated(indexPath);
    }
};
#endif /* UpdatableDataModel_HPP_ */
