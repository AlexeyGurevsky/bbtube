#ifndef BaseSheet_HPP_
#define BaseSheet_HPP_

#include <QObject>
#include <bb/cascades/Sheet>
#include <bb/cascades/Container>

using namespace bb::cascades;

class BaseSheet: public bb::cascades::Sheet
{
    Q_OBJECT
public:
    BaseSheet();
    virtual ~BaseSheet()
    {
    }
    void closeSheet();
protected slots:
    void closeActionClick();
};

#endif /* BaseSheet_HPP_ */
