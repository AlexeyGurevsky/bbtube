#include "applicationui.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/BrowseTab/BrowseTab.hpp"
#include "src/StartPage/StartPage.hpp"
#include "src/ChannelsTab/ChannelsTab.hpp"
#include "src/Cover/AppCover.hpp"
#include "src/menu/AppMenu.hpp"
#include "src/db/DbHelper.hpp"
#include "src/settings/AppSettings.hpp"
#include "src/utils/PlaybackTimeoutHandler.hpp"
#include "src/utils/ChannelListProxy.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/Page>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/cascades/TabbedPane>

using namespace bb::cascades;

QNetworkAccessManager* ApplicationUI::networkManager = 0;
GlobalPlayerContext* ApplicationUI::playerContext = 0;
AppSettings* ApplicationUI::appSettings = 0;
BaseSheet* ApplicationUI::activeSheet = 0;

ApplicationUI::ApplicationUI() :
        QObject()
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    DbHelper::runMigrations();

    networkManager = new QNetworkAccessManager(this);
    playerContext = new GlobalPlayerContext(this);
    appSettings = new AppSettings(this);
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this,
            SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();

    // initialize singletons here to avoid any concurrency issues
    ChannelListProxy *proxy1 = ChannelListProxy::getInstance();
    PlaylistVideoProxy *proxy2 = PlaylistVideoProxy::getInstance();
    VideoViewedPercentProxy *proxy3 = VideoViewedPercentProxy::getInstance();

    root = new bb::cascades::TabbedPane;
    root->setShowTabsOnActionBar(false);

    browseTab = new BrowseTab;
    root->add(browseTab);
    channelsTab = new ChannelsTab;
    root->add(channelsTab);
    playlistsTab = new PlaylistsTab;
    root->add(playlistsTab);
    recommendedTab = new RecommendedTab;
    root->add(recommendedTab);
    trendingTab = new TrendingTab;
    root->add(trendingTab);

    BaseTab *activeTab;

    if (appSettings->defaultTab() == "Search") {
        activeTab = browseTab;
    } else if (appSettings->defaultTab() == "Channels") {
        activeTab = channelsTab;
    } else if (appSettings->defaultTab() == "Playlists") {
        activeTab = playlistsTab;
    } else if (appSettings->defaultTab() == "Recommended") {
        activeTab = recommendedTab;
    } else if (appSettings->defaultTab() == "Trending") {
        activeTab = trendingTab;
    }

    root->setActiveTab(activeTab);

    QObject::connect(root, SIGNAL(activeTabChanged(bb::cascades::Tab*)), this,
            SLOT(onActiveTabChanged(bb::cascades::Tab*)));
    bb::system::InvokeManager *invokeManager = new bb::system::InvokeManager(this);
    QObject::connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this,
            SLOT(onInvoke(const bb::system::InvokeRequest&)));

    Application::instance()->setCover(new AppCover());
    Application::instance()->setScene(root);
    Application::instance()->setMenu(new AppMenu());

    PlaybackTimeoutHandler *playbackTimeoutHandler = new PlaybackTimeoutHandler(this);

    QObject::connect(Application::instance(), SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    QString locale_string = QLocale().name();
    QString file_name = QString("Youtube_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::onInvoke(const bb::system::InvokeRequest& request)
{
    if (request.target() == "com.gurevski.bbtube.app"
            && (request.action() == "bb.action.OPEN" || request.action() == "bb.action.VIEW")) {
        QString url = request.uri().toString().trimmed();

        if (YoutubeClient::getVideoId(url) == "") {
            return;
        }

        BaseTab *activeTab = static_cast<BaseTab*>(root->activeTab());
        BasePage *page = activeTab->activePage();

        if (activeSheet) {
            activeSheet->closeSheet();
        }

        page->setAudioOnly(false);
        page->playVideoFromOutside(url);
    }
    if ((request.target() == "com.gurevski.bbtube.search"
            && request.action() == "bb.action.SEARCH.EXTENDED")
            || (request.target() == "com.gurevski.bbtube.share"
                    && request.action() == "bb.action.SHARE")) {
        BaseTab *activeTab = static_cast<BaseTab*>(root->activeTab());

        if (activeSheet) {
            activeSheet->closeSheet();
        }

        activeTab->navigateToFirstPage();
        root->setActiveTab(browseTab);
        StartPage *startPage = static_cast<StartPage*>(browseTab->activePage());
        startPage->setVideoToPlayer();
        startPage->searchFromOutside(
                QString::fromUtf8(request.data()).replace('\n', ' ').trimmed());
    }
}

void ApplicationUI::onActiveTabChanged(bb::cascades::Tab* tab)
{
    BaseTab *activeTab = static_cast<BaseTab*>(tab);
    BasePage *page = activeTab->activePage();

    page->setVideoToPlayer();
}

void ApplicationUI::onAboutToQuit()
{
    playerContext->setViewedPercent();
}
