#include "SearchChannelsSheet.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/utils/UpdatableDataModel.hpp"

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
#include <bb/cascades/GroupDataModel>

SearchChannelsSheet::SearchChannelsSheet(
        UpdatableDataModel<ChannelListItemModel*> *channelsDataModel,
        UpdatableDataModel<VideoListItemModel*> *feedDataModel, ChannelListPage *channelListPage) :
        BaseSheet(), channelsDataModel(channelsDataModel), feedDataModel(feedDataModel), channelListPage(
                channelListPage)
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
    SearchGrouppedListItemProvider *searchResultsListProvider = new SearchGrouppedListItemProvider(searchResultsList,
            new SearchChannelsSheetListItemActionSetBuilder());
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
    kindProperties->textField()->setHintText("Search channels and feed");
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

    open();
    kindProperties->textField()->requestFocus();
}

void SearchChannelsSheet::onInputFieldChanging(QString text)
{
    if (text == "") {
        searchResultsList->setVisible(false);

        return;
    }

    GroupDataModel *groupModel = new GroupDataModel(QStringList() << "category" << "title");
    groupModel->setGrouping(bb::cascades::ItemGrouping::ByFullValue);

    int categorySortOrder = 1;
    for (int i = 0; i < channelsDataModel->size(); i++) {
        ChannelListItemModel *channelItem = channelsDataModel->value(i);

        if (channelItem->title.contains(text, Qt::CaseInsensitive)) {
            VideoListItemModel *item = new VideoListItemModel();
            item->channelId = channelItem->channelId;
            item->id = channelItem->channelId;
            item->setTitle(channelItem->title);
            item->thumbnailUrl = channelItem->thumbnailUrl;
            item->type = VideoListItemModel::Channel;
            item->setCategory(QString::number(categorySortOrder) + ":Channels");
            item->isFavorite = true;
            item->channelTitle = channelItem->title;

            groupModel->insert(item);
        }
    }

    if (feedDataModel) {
        categorySortOrder = 2;

        for (int i = 0; i < feedDataModel->size(); i++) {
            VideoListItemModel *feedItem = feedDataModel->value(i);

            if (feedItem->title().contains(text, Qt::CaseInsensitive)) {
                VideoListItemModel *item = new VideoListItemModel();
                item->channelId = feedItem->channelId;
                item->id = feedItem->id;
                item->setTitle(feedItem->title());
                item->subtitle = feedItem->subtitle;
                item->type = VideoListItemModel::Video;
                item->setCategory(QString::number(categorySortOrder) + ":Videos");
                item->channelTitle = feedItem->channelTitle;
                item->isFavorite = feedItem->isFavorite;
                item->isWatchLater = feedItem->isWatchLater;
                item->isHistory = feedItem->isHistory;

                groupModel->insert(item);
            }
        }
    }

    bool hasResults = groupModel->size() > 0;

    searchResultsList->setDataModel(groupModel);
    searchResultsList->setVisible(hasResults);
    noResultsContainer->setVisible(!hasResults);
}

void SearchChannelsSheet::onSearchResultsListItemClick(QVariantList indexPath)
{
    if (indexPath.count() > 1) {
        VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
                searchResultsList->dataModel()->data(indexPath).value<QObject*>());

        if (item->type == VideoListItemModel::Video) {
            channelListPage->playVideo(item->id);
        } else if (item->type == VideoListItemModel::Channel) {
            channelListPage->openChannel(item->id);
        }

        closeSheet();
    }
}

void SearchChannelsSheet::onChannelActionItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
            searchResultsList->dataModel()->data(indexPath).value<QObject*>());

    channelListPage->openChannel(item->channelId);

    closeSheet();
}

void SearchChannelsSheet::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
            searchResultsList->dataModel()->data(indexPath).value<QObject*>());

    channelListPage->playAudioOnly(item->id);

    closeSheet();
}
