#ifndef BASETAB_HPP_
#define BASETAB_HPP_

#include "BasePage.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/NavigationPane>

using namespace bb::cascades;

class BaseTab: public Tab
{
Q_OBJECT
public:
    BaseTab()
    {
        navigationPane = bb::cascades::NavigationPane::create().peek(false);
        this->setContent(navigationPane);

        QObject::connect(navigationPane, SIGNAL(popTransitionEnded(bb::cascades::Page*)), this,
                SLOT(popFinished(bb::cascades::Page*)));
    }
    virtual ~BaseTab()
    {
    }
    BasePage* activePage()
    {
        return static_cast<BasePage*>(navigationPane->at(navigationPane->count() - 1));
    }
    void navigateToFirstPage()
    {
        for (int i = navigationPane->count() - 1; i >= 1; i--) {
            bb::cascades::Page *page = navigationPane->at(i);

            navigationPane->remove(page);
            page->deleteLater();
        }
    }
private slots:
    void popFinished(bb::cascades::Page* page)
    {
        delete page;

        BasePage *newPage = static_cast<BasePage*>(navigationPane->at(navigationPane->count() - 1));
        newPage->setVideoToPlayer();
    }
protected:
    NavigationPane *navigationPane;
};

#endif /* BASETAB_HPP_ */
