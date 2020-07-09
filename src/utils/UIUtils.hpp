#ifndef UIUTILS_HPP_
#define UIUTILS_HPP_

#include <bb/cascades/Container>

using namespace bb::cascades;

class UIUtils
{
public:
    static Container* createOverlay();
    static void toastError(QString message);
    static void toastInfo(QString message);
};

#endif /* UIUTILS_HPP_ */
