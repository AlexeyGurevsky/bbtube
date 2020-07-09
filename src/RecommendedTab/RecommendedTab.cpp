#include "RecommendedTab.hpp"
#include "RecommendedPage.hpp"

RecommendedTab::RecommendedTab()
{
    this->setTitle("Recommended");
    this->setImageSource(QString("asset:///images/ic_recommended.png"));

    navigationPane->push(new RecommendedPage(navigationPane));
}



