#include "PlaylistPage.hpp"
#include "PlaylistVideoListItemProvider.hpp"
#include "src/PlayerPage/PlayerPage.hpp"
#include "src/ChannelPage/ChannelPage.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/utils/ActionItemService.hpp"
#include "src/utils/MiniPlayer.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/db/DbHelper.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "SearchVideosSheet.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include "src/applicationui.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Label>
#include <bb/cascades/Divider>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ListView>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/DockLayout>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/UIConfig>
#include <bb/cascades/ActionItem>
#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/Dropdown>
#include <bb/cascades/Option>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ScrollPosition>
#include <bb/cascades/ScrollAnimation>
#include <bb/system/SystemUiResult>
#include <bb/system/SystemDialog>

#include <QVariantList>

PlaylistPage::PlaylistPage(int playlistId, bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane), playlistId(playlistId)
{
    playerContext = ApplicationUI::playerContext;
    playlist = DbHelper::getPlaylistById(playlistId);

    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);
    container->add(getTitleContainer());

    videoListContainer = bb::cascades::Container::create();
    sortControls = bb::cascades::Container::create().visible(false);
    sortControls->setTopPadding(ui->du(2));
    bb::cascades::Container *sortControlsInner = bb::cascades::Container::create().layout(
            bb::cascades::StackLayout::create().orientation(LayoutOrientation::LeftToRight));
    sortByDropdown = bb::cascades::DropDown::create().title("Sort by").layoutProperties(
            bb::cascades::StackLayoutProperties::create().spaceQuota(1));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Date Added").value(
                    (int) PlaylistVideoModel::DateAdded).selected(true));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Name").value((int) PlaylistVideoModel::Name));
    sortByDropdown->add(
            bb::cascades::Option::create().text("Date Viewed").value(
                    (int) PlaylistVideoModel::DateViewed));
    sortOrderDropdown = bb::cascades::DropDown::create().title("Order").layoutProperties(
            bb::cascades::StackLayoutProperties::create().spaceQuota(1));
    sortOrderDropdown->add(
            bb::cascades::Option::create().text("Ascending").value((int) PlaylistVideoModel::Asc));
    sortOrderDropdown->add(
            bb::cascades::Option::create().text("Descending").value((int) PlaylistVideoModel::Desc).selected(
                    true));
    sortControlsInner->add(sortByDropdown);
    sortControlsInner->add(sortOrderDropdown);
    sortControls->add(sortControlsInner);
    sortControls->add(bb::cascades::Divider::create());
    videoListContainer->add(sortControls);
    videoList = new CustomListView;
    PlaylistVideoListItemProvider *listProvider = new PlaylistVideoListItemProvider(videoList);
    videoList->setListItemProvider(listProvider);
    videoList->setTopMargin(ui->du(1));
    videoListContainer->add(videoList);
    container->add(videoListContainer);

    noVideosContainer = bb::cascades::Container::create().layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    noVideosContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    noVideosContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    noVideosContainer->setLayout(new bb::cascades::DockLayout());
    bb::cascades::Container *noVideosLabelContainer = new bb::cascades::Container();
    noVideosLabelContainer->setLeftPadding(ui->du(5));
    noVideosLabelContainer->setRightPadding(ui->du(5));
    noVideosLabelContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Center);
    noVideosLabelContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    bb::cascades::Label *noVideosLabel = bb::cascades::Label::create().text("No videos");
    noVideosLabel->textStyle()->setBase(bb::cascades::SystemDefaults::TextStyles::titleText());
    noVideosLabel->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    noVideosLabelContainer->add(noVideosLabel);
    noVideosContainer->add(noVideosLabelContainer);
    container->add(noVideosContainer);

    root->add(container);
    root->add(overlay);
    this->setContent(root);

    QObject::connect(youtubeClient, SIGNAL(channelDataReceived(ChannelPageData)), this,
            SLOT(onChannelDataReceived(ChannelPageData)));
    QObject::connect(videoList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onVideoListItemClick(QVariantList)));
    QObject::connect(PlaylistVideoProxy::getInstance(), SIGNAL(added(PlaylistVideoModel*)), this,
            SLOT(onPlaylistVideoAdded(PlaylistVideoModel*)));
    QObject::connect(PlaylistVideoProxy::getInstance(), SIGNAL(updated(PlaylistVideoModel*)), this,
            SLOT(onPlaylistVideoUpdated(PlaylistVideoModel*)));
    QObject::connect(PlaylistVideoProxy::getInstance(),
            SIGNAL(deleted(QString,PlaylistListItemModel::Type)), this,
            SLOT(onPlaylistVideoDeleted(QString, PlaylistListItemModel::Type)));
    QObject::connect(PlaylistVideoProxy::getInstance(),
            SIGNAL(deletedAll(PlaylistListItemModel::Type)), this,
            SLOT(onPlaylistVideoDeletedAll(PlaylistListItemModel::Type)));
    QObject::connect(listProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
            SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(deleteActionItemClick(QVariantList)), this,
            SLOT(onDeleteActionItemClick(QVariantList)));

    QObject::connect(sortByDropdown, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSortDropdownSelectedIndexChanged(int)));
    QObject::connect(sortOrderDropdown, SIGNAL(selectedIndexChanged(int)), this,
            SLOT(onSortDropdownSelectedIndexChanged(int)));

    sortActionItem = bb::cascades::ActionItem::create();
    sortActionItem->setImageSource(QString("asset:///images/ic_sort.png"));
    sortActionItem->setTitle("Sort");
    sortActionItem->addShortcut(bb::cascades::Shortcut::create().key("o"));
    searchActionItem = bb::cascades::ActionItem::create();
    searchActionItem->setImageSource(QString("asset:///images/ic_search.png"));
    searchActionItem->setTitle("Search");
    searchActionItem->addShortcut(bb::cascades::Shortcut::create().key("s"));
    toTopActionItem = bb::cascades::ActionItem::create();
    toTopActionItem->setImageSource(QString("asset:///images/ic_to_top.png"));
    toTopActionItem->setTitle("To Top");
    toTopActionItem->addShortcut(bb::cascades::Shortcut::create().key("t"));
    toBottomActionItem = bb::cascades::ActionItem::create();
    toBottomActionItem->setImageSource(QString("asset:///images/ic_to_bottom.png"));
    toBottomActionItem->setTitle("To Bottom");
    toBottomActionItem->addShortcut(bb::cascades::Shortcut::create().key("b"));
    deleteAllActionItem = bb::cascades::DeleteActionItem::create();
    deleteAllActionItem->setTitle("Clear All");
    playAllActionItem = bb::cascades::ActionItem::create();
    playAllActionItem->setImageSource(QString("asset:///images/ic_play.png"));
    playAllActionItem->setTitle("Play All");
    shuffleAllActionItem = bb::cascades::ActionItem::create();
    shuffleAllActionItem->setImageSource(QString("asset:///images/ic_shuffle.png"));
    shuffleAllActionItem->setTitle("Shuffle All");

    this->addAction(toTopActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(toBottomActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(playAllActionItem, bb::cascades::ActionBarPlacement::Signature);
    this->addAction(shuffleAllActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->addAction(searchActionItem, bb::cascades::ActionBarPlacement::OnBar);
    this->addAction(sortActionItem, bb::cascades::ActionBarPlacement::OnBar);
    this->addAction(deleteAllActionItem, bb::cascades::ActionBarPlacement::InOverflow);

    QObject::connect(sortActionItem, SIGNAL(triggered()), this, SLOT(onSortActionItemClick()));
    QObject::connect(searchActionItem, SIGNAL(triggered()), this, SLOT(onSearchActionItemClick()));
    QObject::connect(toTopActionItem, SIGNAL(triggered()), this, SLOT(onToTopActionItemClick()));
    QObject::connect(toBottomActionItem, SIGNAL(triggered()), this,
            SLOT(onToBottomActionItemClick()));
    QObject::connect(playAllActionItem, SIGNAL(triggered()), this,
            SLOT(onPlayAllActionItemClick()));
    QObject::connect(shuffleAllActionItem, SIGNAL(triggered()), this,
            SLOT(onShuffleAllActionItemClick()));
    QObject::connect(deleteAllActionItem, SIGNAL(triggered()), this,
            SLOT(onDeleteAllActionItemClick()));

    UpdatableDataModel<PlaylistVideoModel*> *model = new UpdatableDataModel<PlaylistVideoModel*>(
            DbHelper::getListByPlaylistId(playlistId));
    videoList->setDataModel(model);
    updateTitle();

    setAvailability();
    if (videoList->isVisible()) {
        videoList->requestFocus();
    }
}

void PlaylistPage::onVideoListItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = videoList->dataModel()->data(indexPath).value<PlaylistVideoModel*>();
    videoList->setEnabled(false);
    playVideo(item->videoId);
}

void PlaylistPage::onDeleteActionItemClick(QVariantList indexPath)
{
    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel();
    PlaylistVideoModel *item = dataModel->data(indexPath).value<PlaylistVideoModel*>();
    DbHelper::deletePlaylistVideo(item->videoId, playlistId);
    PlaylistVideoProxy::getInstance()->deleteById(item->videoId, playlist->type);

    if (playlist->type == PlaylistListItemModel::History) {
        DbHelper::deleteViewedPercent(item->videoId);
        VideoViewedPercentProxy::getInstance()->deleteById(item->videoId);
    }

    UIUtils::toastInfo("Deleted");
}

void PlaylistPage::onChannelDataReceived(ChannelPageData channelData)
{
    navigationPane->push(new ChannelPage(channelData, navigationPane));
    overlay->setVisible(false);
    videoList->setEnabled(true);
}

void PlaylistPage::onYoutubeError(QString message)
{
    videoList->setEnabled(true);
    BasePage::onYoutubeError(message);
}

void PlaylistPage::onPlaylistVideoAdded(PlaylistVideoModel* video)
{
    if (video->playlistId != playlistId) {
        return;
    }

    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel();
    PlaylistVideoModel *newItem = new PlaylistVideoModel(video->videoId, video->playlistId,
            video->title, video->channelId, video->channelTitle, video->dateAdded,
            video->dateLastPlayed, video->sortOrder);
    dataModel->insert(0, newItem);

    updateTitle();
    setAvailability();
}

void PlaylistPage::onPlaylistVideoUpdated(PlaylistVideoModel* video)
{
    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel();
    for (int i = 0; i < dataModel->size(); i++) {
        PlaylistVideoModel *itemToUpdate = dataModel->value(i);

        if (itemToUpdate->videoId == video->videoId) {
            itemToUpdate->title = video->title;
            itemToUpdate->channelTitle = video->channelTitle;
            itemToUpdate->dateLastPlayed = video->dateLastPlayed;

            QVariantList indexPath;
            indexPath.append(i);
            dataModel->updateItem(indexPath);

            break;
        }
    }
}

void PlaylistPage::onPlaylistVideoDeleted(QString videoId, PlaylistListItemModel::Type playlistType)
{
    if (playlistType != playlist->type) {
        return;
    }

    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel();

    for (int i = 0; i < dataModel->size(); i++) {
        if (dataModel->value(i)->videoId == videoId) {
            dataModel->removeAt(i);

            break;
        }
    }

    updateTitle();
    setAvailability();
}

void PlaylistPage::onPlaylistVideoDeletedAll(PlaylistListItemModel::Type playlistType)
{
    if (playlistType != playlist->type) {
        return;
    }

    videoList->setDataModel(new UpdatableDataModel<PlaylistVideoModel*>());

    updateTitle();
    setAvailability();
}

void PlaylistPage::setAvailability()
{
    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel();
    bool hasVideos = dataModel->size() > 0;

    videoListContainer->setVisible(hasVideos);
    noVideosContainer->setVisible(!hasVideos);
    sortActionItem->setEnabled(hasVideos);
    searchActionItem->setEnabled(hasVideos);
    toTopActionItem->setEnabled(hasVideos);
    toBottomActionItem->setEnabled(hasVideos);
    deleteAllActionItem->setEnabled(hasVideos);
    playAllActionItem->setEnabled(hasVideos);
    shuffleAllActionItem->setEnabled(hasVideos);
}

void PlaylistPage::updateTitle()
{
    UpdatableDataModel<PlaylistVideoModel*> *model =
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel();

    setTitle(QString("%1 (%2)").arg(playlist->title).arg(model->size()));
}

void PlaylistPage::onChannelActionItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = videoList->dataModel()->data(indexPath).value<PlaylistVideoModel*>();
    videoList->setEnabled(false);
    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void PlaylistPage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    BasePage::onMetadataReceived(videoMetadata, storageData);
    videoList->setEnabled(true);
}

void PlaylistPage::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    audioOnly = true;
    onVideoListItemClick(indexPath);
}

void PlaylistPage::onSortActionItemClick()
{
    sortControls->setVisible(!sortControls->isVisible());
}

void PlaylistPage::onSortDropdownSelectedIndexChanged(int index)
{
    sortControls->setVisible(false);

    UpdatableDataModel<PlaylistVideoModel*> *model =
            new UpdatableDataModel<PlaylistVideoModel*>(
                    DbHelper::getListByPlaylistId(playlistId,
                            (PlaylistVideoModel::SortBy) sortByDropdown->selectedValue().toInt(),
                            (PlaylistVideoModel::SortDirection) sortOrderDropdown->selectedValue().toInt()));
    videoList->setDataModel(model);
}

void PlaylistPage::onSearchActionItemClick()
{
    SearchVideosSheet *sheet = new SearchVideosSheet(
            (UpdatableDataModel<PlaylistVideoModel*> *) videoList->dataModel(), this);
}

void PlaylistPage::playVideo(QString videoId)
{
    overlay->setVisible(true);
    isPlaylist = true;
    setPlaylist();
    youtubeClient->process("https://www.youtube.com/watch?v=" + videoId);
}

void PlaylistPage::playAll(bool shuffle)
{
    overlay->setVisible(true);
    isPlaylist = true;
    setPlaylist();
    if (shuffle) {
        playerContext->shufflePlaylist();
    }

    youtubeClient->process(
            "https://www.youtube.com/watch?v=" + playerContext->getVideoFromPlaylistByIndex(0));
}

void PlaylistPage::playAudioOnly(QString videoId)
{
    audioOnly = true;
    playVideo(videoId);
}

void PlaylistPage::openChannel(QString channelId)
{
    overlay->setVisible(true);
    youtubeClient->channel(channelId);
}

void PlaylistPage::onToTopActionItemClick()
{
    videoList->scrollToPosition(bb::cascades::ScrollPosition::Beginning,
            bb::cascades::ScrollAnimation::None);
}

void PlaylistPage::onToBottomActionItemClick()
{
    videoList->scrollToPosition(bb::cascades::ScrollPosition::End,
            bb::cascades::ScrollAnimation::None);
}

void PlaylistPage::onPlayAllActionItemClick()
{
    playAll(false);
}

void PlaylistPage::onShuffleAllActionItemClick()
{
    playAll(true);
}

void PlaylistPage::setPlaylist()
{
    QList<PlaylistVideoModel*> videos = DbHelper::getListByPlaylistId(playlistId,
            (PlaylistVideoModel::SortBy) sortByDropdown->selectedValue().toInt(),
            (PlaylistVideoModel::SortDirection) sortOrderDropdown->selectedValue().toInt());
    QList<QString> sequence;

    for (int i = 0; i < videos.count(); i++) {
        sequence.append(videos[i]->videoId);
    }

    playerContext->setPlaylist(playlistId, sequence);
}

void PlaylistPage::onDeleteAllActionItemClick()
{
    bb::system::SystemDialog *confirmDialog = new bb::system::SystemDialog("Continue", "Cancel");

    confirmDialog->setTitle("Clear All");
    confirmDialog->setBody("All videos will be deleted from the playlist.");

    bool success = connect(confirmDialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), this,
            SLOT(onDeleteAllDialogFinished(bb::system::SystemUiResult::Type)));

    if (success) {
        confirmDialog->show();
    } else {
        confirmDialog->deleteLater();
    }
}

void PlaylistPage::onDeleteAllDialogFinished(bb::system::SystemUiResult::Type type)
{
    bb::system::SystemDialog *confirmDialog = qobject_cast<bb::system::SystemDialog *>(
            QObject::sender());

    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        DbHelper::deleteAllFromPlaylist(playlist->playlistId);

        if (playlist->isPredefined()) {
            PlaylistVideoProxy::getInstance()->deleteAll((PlaylistListItemModel::Type) playlistId);
        }

        if (playlist->type == PlaylistListItemModel::History) {
            DbHelper::deleteAllViewedPercents();
            VideoViewedPercentProxy::getInstance()->deleteAll();
        }
    }

    confirmDialog->deleteLater();
}
