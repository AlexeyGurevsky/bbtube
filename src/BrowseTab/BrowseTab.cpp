#include "BrowseTab.hpp"
#include "src/StartPage/StartPage.hpp"

BrowseTab::BrowseTab()
{
    this->setTitle("Search");
    this->setImageSource(QString("asset:///images/ic_search.png"));

    navigationPane->push(new StartPage(navigationPane));
}

