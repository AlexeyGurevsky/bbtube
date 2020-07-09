#include "TrendingTab.hpp"
#include "TrendingPage.hpp"

TrendingTab::TrendingTab()
{
    this->setTitle("Trending");
    this->setImageSource(QString("asset:///images/ic_trending.png"));

    navigationPane->push(new TrendingPage(navigationPane));
}



