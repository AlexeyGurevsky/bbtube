#include "PlaylistSheet.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/PlaylistListItemModel.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/PlaylistPage/PlaylistVideoListItemProvider.hpp"
#include "src/db/DbHelper.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/applicationui.hpp"
#include "src/utils/BasePage.hpp"
#include "src/ChannelPage/ChannelPage.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ListView>
#include <bb/cascades/UIConfig>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ScrollPosition>
#include <bb/cascades/ScrollAnimation>
#include <bb/cascades/Shortcut>

PlaylistSheet::PlaylistSheet(NavigationPane *navigationPane) :
        BaseSheet(), navigationPane(navigationPane), audioOnly(false)
{
    playerContext = ApplicationUI::playerContext;
    playlist = DbHelper::getPlaylistById(playerContext->getPlaylistId());
    youtubeClient = new YoutubeClient(this);

    Page *content = new Page();
    Container *root = Container::create();
    root->setLayout(new DockLayout());

    Container *container = Container::create();
    UIConfig *ui = container->ui();
    container->setTopPadding(ui->du(2));
    container->setRightPadding(ui->du(1));
    container->setBottomPadding(ui->du(1));
    container->setLeftPadding(ui->du(1));
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    list = new CustomListView();
    PlaylistVideoListItemProvider *listProvider = new PlaylistVideoListItemProvider(list);
    list->setListItemProvider(listProvider);
    container->add(list);

    root->add(container);

    overlay = UIUtils::createOverlay();
    root->add(overlay);

    TitleBar *titleBar = new TitleBar(TitleBarKind::Default);
    ActionItem *closeAction = ActionItem::create().title("Close");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    titleBar->setDismissAction(closeAction);
    titleBar->setTitle(playlist->title);
    content->setTitleBar(titleBar);

    content->setContent(root);
    this->setContent(content);

    QObject::connect(list, SIGNAL(triggered(QVariantList)), this,
            SLOT(onListItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
            SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(deleteActionItemClick(QVariantList)), this,
            SLOT(onDeleteActionItemClick(QVariantList)));
    QObject::connect(youtubeClient, SIGNAL(channelDataReceived(ChannelPageData)), this,
            SLOT(onChannelDataReceived(ChannelPageData)));
    QObject::connect(playerContext, SIGNAL(metadataChanged()), this, SLOT(onMetadataChanged()));

    bb::cascades::ActionItem *toTopActionItem = bb::cascades::ActionItem::create();
    toTopActionItem->setImageSource(QString("asset:///images/ic_to_top.png"));
    toTopActionItem->setTitle("To Top");
    toTopActionItem->addShortcut(bb::cascades::Shortcut::create().key("t"));
    bb::cascades::ActionItem *toBottomActionItem = bb::cascades::ActionItem::create();
    toBottomActionItem->setImageSource(QString("asset:///images/ic_to_bottom.png"));
    toBottomActionItem->setTitle("To Bottom");
    toBottomActionItem->addShortcut(bb::cascades::Shortcut::create().key("b"));
    bb::cascades::ActionItem *shuffleActionItem = bb::cascades::ActionItem::create();
    shuffleActionItem->setImageSource(QString("asset:///images/ic_shuffle.png"));
    shuffleActionItem->setTitle("Shuffle");

    content->addAction(toTopActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    content->addAction(toBottomActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    content->addAction(shuffleActionItem, bb::cascades::ActionBarPlacement::Signature);

    QObject::connect(toTopActionItem, SIGNAL(triggered()), this, SLOT(onToTopActionItemClick()));
    QObject::connect(toBottomActionItem, SIGNAL(triggered()), this,
            SLOT(onToBottomActionItemClick()));
    QObject::connect(shuffleActionItem, SIGNAL(triggered()), this,
            SLOT(onShuffleActionItemClick()));

    QList<PlaylistVideoModel*> videos = DbHelper::getListByPlaylistId(playlist->playlistId);
    QMap<QString, PlaylistVideoModel*> videosMap;

    for (int i = 0; i < videos.count(); i++) {
        videosMap.insert(videos[i]->videoId, videos[i]);
    }

    QString nowPlaying = playerContext->getVideoMetadata().video.videoId;
    QList<PlaylistVideoModel*> transposition = playerContext->getPlaylistTransposition(&videosMap);
    int nowPlayingPosition = 0;

    for (int i = 0; i < transposition.count(); i++) {
        if (transposition[i]->videoId == nowPlaying) {
            transposition[i]->onAir = true;
            nowPlayingPosition = i;

            break;
        }
    }

    UpdatableDataModel<PlaylistVideoModel*> *model = new UpdatableDataModel<PlaylistVideoModel*>(
            transposition);
    list->setDataModel(model);
    if (nowPlayingPosition >= 1) { //just a magic number
        list->scrollToItem(QVariantList() << (nowPlayingPosition - 1),
                bb::cascades::ScrollAnimation::None);
    }

    open();
}

void PlaylistSheet::onListItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = list->dataModel()->data(indexPath).value<PlaylistVideoModel*>();
    playVideo(item->videoId);
}

void PlaylistSheet::onChannelActionItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = list->dataModel()->data(indexPath).value<PlaylistVideoModel*>();
    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void PlaylistSheet::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = list->dataModel()->data(indexPath).value<PlaylistVideoModel*>();
    audioOnly = true;
    playVideo(item->videoId);
}

void PlaylistSheet::onDeleteActionItemClick(QVariantList indexPath)
{
    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) list->dataModel();
    PlaylistVideoModel *item = dataModel->data(indexPath).value<PlaylistVideoModel*>();

    DbHelper::deletePlaylistVideo(item->videoId, playlist->playlistId);
    PlaylistVideoProxy::getInstance()->deleteById(item->videoId, playlist->type);
    dataModel->removeAt(indexPath.at(0).toInt());

    if (playlist->type == PlaylistListItemModel::History) {
        DbHelper::deleteViewedPercent(item->videoId);
        VideoViewedPercentProxy::getInstance()->deleteById(item->videoId);
    }

    if (dataModel->size() == 0) {
        closeSheet();
    }
}

void PlaylistSheet::onChannelDataReceived(ChannelPageData channelData)
{
    ChannelPage *channelPage = new ChannelPage(channelData, navigationPane);
    navigationPane->insert(1, channelPage);

    for (int i = navigationPane->count() - 1; i >= 2; i--) {
        bb::cascades::Page *page = navigationPane->at(i);

        navigationPane->remove(page);
        page->deleteLater();
    }

    navigationPane->navigateTo(channelPage);
    overlay->setVisible(false);
    closeSheet();
}

void PlaylistSheet::onToTopActionItemClick()
{
    list->scrollToPosition(bb::cascades::ScrollPosition::Beginning,
            bb::cascades::ScrollAnimation::None);
}

void PlaylistSheet::onToBottomActionItemClick()
{
    list->scrollToPosition(bb::cascades::ScrollPosition::End, bb::cascades::ScrollAnimation::None);
}

void PlaylistSheet::onShuffleActionItemClick()
{
    playerContext->shufflePlaylist();

    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) list->dataModel();
    QMap<QString, PlaylistVideoModel*> videosMap;

    for (int i = 0; i < dataModel->size(); i++) {
        PlaylistVideoModel* item = dataModel->value(i);

        videosMap.insert(item->videoId, item);
    }

    UpdatableDataModel<PlaylistVideoModel*> *model = new UpdatableDataModel<PlaylistVideoModel*>(
            playerContext->getPlaylistTransposition(&videosMap));
    list->setDataModel(model);
}

void PlaylistSheet::onMetadataChanged()
{
    QString nowPlaying = playerContext->getVideoMetadata().video.videoId;
    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) list->dataModel();
    int oldPlayingPosition = -1;
    int newPlayingPosition = -1;

    for (int i = 0; i < dataModel->size(); i++) {
        PlaylistVideoModel* item = dataModel->value(i);

        if (item->onAir && oldPlayingPosition == -1) {
            oldPlayingPosition = i;
            item->onAir = false;
        }
        if (item->videoId == nowPlaying && newPlayingPosition == -1) {
            newPlayingPosition = i;
            item->onAir = true;
        }
    }

    if (oldPlayingPosition >= 0) {
        dataModel->updateItem(QVariantList() << oldPlayingPosition);
    }
    if (newPlayingPosition >= 0) {
        dataModel->updateItem(QVariantList() << newPlayingPosition);
    }
}

void PlaylistSheet::playVideo(QString videoId)
{
    closeSheet();
    BasePage* page = (BasePage*) navigationPane->at(navigationPane->count() - 1);
    page->setAudioOnly(audioOnly);
    page->playVideoFromPlaylist("https://www.youtube.com/watch?v=" + videoId);
    audioOnly = false;
}
