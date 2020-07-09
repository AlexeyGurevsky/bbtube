#ifndef INFOSHEET_HPP_
#define INFOSHEET_HPP_

#include "src/utils/BaseSheet.hpp"

#include <QObject>
#include <bb/cascades/Sheet>

using namespace bb::cascades;

class InfoSheet: public BaseSheet
{
    Q_OBJECT
public:
    InfoSheet();
    virtual ~InfoSheet()
    {
    }
private slots:
    void onCrackberryActionItemClick();
    void onDonateActionItemClick();
};


#endif /* INFOSHEET_HPP_ */
