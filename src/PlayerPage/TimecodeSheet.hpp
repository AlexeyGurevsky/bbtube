#ifndef TimecodeSheet_HPP_
#define TimecodeSheet_HPP_

#include "src/utils/GlobalPlayerContext.hpp"
#include "src/utils/BaseSheet.hpp"

#include <QObject>
#include <bb/cascades/ListView>

using namespace bb::cascades;

class TimecodeSheet: public BaseSheet
{
    Q_OBJECT
public:
    TimecodeSheet();
    virtual ~TimecodeSheet()
    {
    }
private slots:
    void onMetadataChanged();
    void onListViewTriggered(QVariantList);
    void onPlayerPositionChanged(unsigned int position);
private:
    ListView *listView;
    GlobalPlayerContext *playerContext;

    void setModel(bool scrollToPlaying);
};


#endif /* TimecodeSheet_HPP_ */
