#include "SearchVideosSheet.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/models/PlaylistVideoModel.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "PlaylistVideoListItemProvider.hpp"
#include "src/db/DbHelper.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"
#include "src/utils/VideoViewedPercentProxy.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <bb/cascades/TextField>
#include <bb/cascades/ListView>
#include <bb/cascades/UIConfig>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>

SearchVideosSheet::SearchVideosSheet(UpdatableDataModel<PlaylistVideoModel*> *videosDataModel,
        PlaylistPage *playlistPage) :
        BaseSheet(), videosDataModel(videosDataModel), playlistPage(playlistPage)
{
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
    searchResultsList = new CustomListView();
    PlaylistVideoListItemProvider *searchResultsListProvider = new PlaylistVideoListItemProvider(searchResultsList);
    searchResultsList->setListItemProvider(searchResultsListProvider);
    searchResultsList->setVisible(false);
    container->add(searchResultsList);

    noResultsContainer = Container::create().visible(false).layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    noResultsContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    noResultsContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    noResultsContainer->setLayout(new bb::cascades::DockLayout());
    Container *noResultsLabelContainer = new Container();
    noResultsLabelContainer->setLeftPadding(ui->du(5));
    noResultsLabelContainer->setRightPadding(ui->du(5));
    noResultsLabelContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Center);
    noResultsLabelContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    Label *noResultsLabel = Label::create().text("Can't find a match");
    noResultsLabel->textStyle()->setBase(bb::cascades::SystemDefaults::TextStyles::titleText());
    noResultsLabel->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    noResultsLabelContainer->add(noResultsLabel);
    noResultsContainer->add(noResultsLabelContainer);
    container->add(noResultsContainer);

    root->add(container);

    overlay = UIUtils::createOverlay();
    root->add(overlay);

    TitleBar *titleBar = new TitleBar(TitleBarKind::TextField);
    ActionItem *closeAction = ActionItem::create().title("Cancel");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    titleBar->setDismissAction(closeAction);

    TextFieldTitleBarKindProperties *kindProperties = new TextFieldTitleBarKindProperties();
    kindProperties->textField()->setHintText("Search videos");
    titleBar->setKindProperties(kindProperties);
    QObject::connect(kindProperties->textField(), SIGNAL(textChanging(QString)), this,
            SLOT(onInputFieldChanging(QString)));

    content->setTitleBar(titleBar);
    content->setContent(root);
    this->setContent(content);

    QObject::connect(searchResultsList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onSearchResultsListItemClick(QVariantList)));
    QObject::connect(searchResultsListProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));
    QObject::connect(searchResultsListProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)),
            this, SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(searchResultsListProvider, SIGNAL(deleteActionItemClick(QVariantList)), this,
            SLOT(onDeleteActionItemClick(QVariantList)));

    open();
    kindProperties->textField()->requestFocus();
}

void SearchVideosSheet::onInputFieldChanging(QString text)
{
    if (text == "") {
        searchResultsList->setVisible(false);

        return;
    }

    UpdatableDataModel<PlaylistVideoModel*> *model = new UpdatableDataModel<PlaylistVideoModel*>();
    for (int i = 0; i < videosDataModel->size(); i++) {
        PlaylistVideoModel *videoItem = videosDataModel->value(i);

        if (videoItem->title.contains(text, Qt::CaseInsensitive)) {
            PlaylistVideoModel *item = new PlaylistVideoModel();
            item->channelId = videoItem->channelId;
            item->videoId = videoItem->videoId;
            item->title = videoItem->title;
            item->playlistId = videoItem->playlistId;
            item->channelTitle = videoItem->channelTitle;
            item->dateAdded = videoItem->dateAdded;
            item->dateLastPlayed = videoItem->dateLastPlayed;
            item->sortOrder = videoItem->sortOrder;

            model->append(item);
        }
    }

    bool hasResults = model->size() > 0;

    searchResultsList->setDataModel(model);
    searchResultsList->setVisible(hasResults);
    noResultsContainer->setVisible(!hasResults);
}

void SearchVideosSheet::onSearchResultsListItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = searchResultsList->dataModel()->data(indexPath).value<
            PlaylistVideoModel*>();

    playlistPage->playVideo(item->videoId);

    closeSheet();
}

void SearchVideosSheet::onChannelActionItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = searchResultsList->dataModel()->data(indexPath).value<
            PlaylistVideoModel*>();

    playlistPage->openChannel(item->channelId);

    closeSheet();
}

void SearchVideosSheet::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    PlaylistVideoModel *item = searchResultsList->dataModel()->data(indexPath).value<
            PlaylistVideoModel*>();

    playlistPage->playAudioOnly(item->videoId);

    closeSheet();
}

void SearchVideosSheet::onDeleteActionItemClick(QVariantList indexPath)
{
    UpdatableDataModel<PlaylistVideoModel*> *dataModel =
            (UpdatableDataModel<PlaylistVideoModel*> *) searchResultsList->dataModel();
    PlaylistVideoModel *item = searchResultsList->dataModel()->data(indexPath).value<
            PlaylistVideoModel*>();

    DbHelper::deletePlaylistVideo(item->videoId, playlistPage->getPlaylist()->playlistId);
    PlaylistVideoProxy::getInstance()->deleteById(item->videoId, playlistPage->getPlaylist()->type);
    dataModel->removeAt(indexPath.at(0).toInt());

    if (playlistPage->getPlaylist()->type == PlaylistListItemModel::History) {
        DbHelper::deleteViewedPercent(item->videoId);
        VideoViewedPercentProxy::getInstance()->deleteById(item->videoId);
    }
}
