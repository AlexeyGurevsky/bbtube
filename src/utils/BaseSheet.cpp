#include "BaseSheet.hpp"
#include "src/applicationui.hpp"

#include <bb/cascades/Sheet>

BaseSheet::BaseSheet() : Sheet()
{
    ApplicationUI::activeSheet = this;
}

void BaseSheet::closeActionClick()
{
    ApplicationUI::activeSheet = 0;
    this->close();
    this->deleteLater();
}

void BaseSheet::closeSheet()
{
    closeActionClick();
}

