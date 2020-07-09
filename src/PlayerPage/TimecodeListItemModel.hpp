#ifndef EQUALIZERLISTITEMMODEL_HPP_
#define EQUALIZERLISTITEMMODEL_HPP_

class TimecodeListItemModel : public QObject
{
    Q_OBJECT
public:
    TimecodeListItemModel()
    {
    }
    QString text;
    int seconds;
    bool isPlaying;
};
Q_DECLARE_METATYPE( TimecodeListItemModel *);

#endif /* EQUALIZERLISTITEMMODEL_HPP_ */
