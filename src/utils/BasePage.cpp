#include "BasePage.hpp"

#include "MiniPlayer.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "GlobalPlayerContext.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/applicationui.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/PlayerPage/PlayerPage.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/LabelAutoSizeProperties>
#include <bb/cascades/SystemDefaults>

BasePage::BasePage(bb::cascades::NavigationPane *navigationPane, bool addMiniPlayer) :
        bb::cascades::Page(navigationPane), navigationPane(navigationPane), audioOnly(false), isPlaylist(
                false)
{
    youtubeClient = new YoutubeClient(this);
    playerContext = ApplicationUI::playerContext;
    miniPlayer = addMiniPlayer ? new MiniPlayer(navigationPane) : 0;

    root = new bb::cascades::Container();
    ui = root->ui();
    root->setLayout(new bb::cascades::DockLayout());
    overlay = UIUtils::createOverlay();

    QObject::connect(youtubeClient, SIGNAL(metadataReceived(VideoMetadata, StorageData)), this,
            SLOT(onMetadataReceived(VideoMetadata, StorageData)));
    QObject::connect(youtubeClient, SIGNAL(error(QString)), this, SLOT(onYoutubeError(QString)));
}
void BasePage::onYoutubeError(QString message)
{
    overlay->setVisible(false);
    UIUtils::toastError(message);
}
void BasePage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    if (storageData.instances.count() == 0) {
        UIUtils::toastError("Source unavailable");
    } else {
        navigationPane->push(
                new PlayerPage(videoMetadata, storageData, this->navigationPane, audioOnly, isPlaylist));
    }

    overlay->setVisible(false);
    audioOnly = false;
    isPlaylist = false;
}
void BasePage::playVideoFromOutside(QString url)
{
    overlay->setVisible(true);
    isPlaylist = false;
    youtubeClient->process(url);
}
void BasePage::playVideoFromPlaylist(QString url)
{
    overlay->setVisible(true);
    isPlaylist = true;
    youtubeClient->process(url);
}
void BasePage::setVideoToPlayer()
{
    if (miniPlayer) {
        miniPlayer->setVideo();
    }
}
void BasePage::lazyLoad()
{
}

bb::cascades::Container* BasePage::getTitleContainer()
{
    bb::cascades::Container *headerContainer = new bb::cascades::Container();
    headerContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    headerContainer->setLeftPadding(ui->du(1));
    headerContainer->setTopPadding(ui->du(1));
    headerContainer->setRightPadding(ui->du(1));
    headerContainer->setBottomPadding(ui->du(1));
    headerContainer->setBackground(Color::fromARGB(0xff323232));
    headerContainer->setBottomMargin(ui->du(1));

    title = bb::cascades::Label::create();
    title->textStyle()->setBase(bb::cascades::SystemDefaults::TextStyles::titleText());
    title->setMultiline(true);
    title->autoSize()->setMaxLineCount(2);
    headerContainer->add(title);

    return headerContainer;
}

void BasePage::setTitle(QString titleText)
{
    title->setText(titleText);
}

void BasePage::setAudioOnly(bool value)
{
    audioOnly = value;
}

void BasePage::setIsPlaylist(bool value)
{
    isPlaylist = value;
}
