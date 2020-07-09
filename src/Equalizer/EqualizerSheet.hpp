#ifndef EQUALIZERSHEET_HPP_
#define EQUALIZERSHEET_HPP_

#include "src/utils/GlobalPlayerContext.hpp"
#include "src/utils/BaseSheet.hpp"

#include <QObject>
#include <bb/cascades/ListView>
#include <bb/multimedia/EqualizerPreset>

using namespace bb::cascades;

class EqualizerSheet: public BaseSheet
{
    Q_OBJECT
public:
    EqualizerSheet();
    virtual ~EqualizerSheet()
    {
    }
private slots:
    void saveActionClick();
    void onListViewTriggered(QVariantList);
private:
    ListView *listView;
    GlobalPlayerContext *playerContext;
    QVariantList selectedIndexPath;

    void setModel(bb::multimedia::EqualizerPreset::Type currentPreset);
};


#endif /* EQUALIZERSHEET_HPP_ */
