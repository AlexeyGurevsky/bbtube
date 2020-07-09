#ifndef ImportChannelsSheet_HPP_
#define ImportChannelsSheet_HPP_

#include "src/utils/BaseSheet.hpp"

#include <QObject>
#include <bb/cascades/TextArea>
#include <bb/cascades/Container>
#include <bb/cascades/ActionItem>

using namespace bb::cascades;

class ImportChannelsSheet: public BaseSheet
{
    Q_OBJECT
public:
    ImportChannelsSheet();
    virtual ~ImportChannelsSheet()
    {
    }
private slots:
    void onImportActionItemClick();
    void onDownloadActionItemClick();
private:
    TextArea *textArea;
    Container *overlay;
    bb::cascades::ActionItem *importActionItem;
};

#endif /* ImportChannelsSheet_HPP_ */
