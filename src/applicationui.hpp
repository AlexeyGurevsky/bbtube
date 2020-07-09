#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "src/utils/GlobalPlayerContext.hpp"
#include "src/settings/AppSettings.hpp"
#include "src/BrowseTab/BrowseTab.hpp"
#include "src/ChannelsTab/ChannelsTab.hpp"
#include "src/PlaylistsTab/PlaylistsTab.hpp"
#include "src/RecommendedTab/RecommendedTab.hpp"
#include "src/TrendingTab/TrendingTab.hpp"
#include "src/utils/BaseSheet.hpp"

#include <QtNetwork/QNetworkAccessManager>
#include <QObject>
#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/system/InvokeRequest>
#include <bb/cascades/TabbedPane>

namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI() {}
    static QNetworkAccessManager *networkManager;
    static GlobalPlayerContext *playerContext;
    static AppSettings *appSettings;
    static BaseSheet *activeSheet;
private slots:
    void onSystemLanguageChanged();
    void onInvoke(const bb::system::InvokeRequest& request);
    void onActiveTabChanged(bb::cascades::Tab*);
    void onAboutToQuit();
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    bb::cascades::TabbedPane *root;
    BrowseTab *browseTab;
    ChannelsTab *channelsTab;
    PlaylistsTab *playlistsTab;
    RecommendedTab *recommendedTab;
    TrendingTab *trendingTab;
};

#endif /* ApplicationUI_HPP_ */
