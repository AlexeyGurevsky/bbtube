#ifndef EQUALIZERLISTITEMMODEL_HPP_
#define EQUALIZERLISTITEMMODEL_HPP_

#include <bb/multimedia/EqualizerPreset>

class EqualizerListItemModel : public QObject
{
    Q_OBJECT
public:
    EqualizerListItemModel(bb::multimedia::EqualizerPreset::Type type, QString title, bool isSelected)
        : type(type), title(title), isSelected(isSelected)
    {
    }
    bb::multimedia::EqualizerPreset::Type type;
    QString title;
    bool isSelected;
};
Q_DECLARE_METATYPE( EqualizerListItemModel *);

#endif /* EQUALIZERLISTITEMMODEL_HPP_ */
