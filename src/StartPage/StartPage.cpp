#include "StartPage.hpp"
#include "src/PlayerPage/PlayerPage.hpp"
#include "src/ChannelPage/ChannelPage.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/SearchData.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/SuggestionsList/SuggestionsListItemProvider.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/utils/MiniPlayer.hpp"
#include "src/applicationui.hpp"
#include "src/db/DbHelper.cpp"
#include "src/utils/ChannelListProxy.hpp"
#include "src/utils/PlaylistVideoProxy.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/TextField>
#include <bb/cascades/TextInputProperties>
#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ListView>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/DockLayout>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/UIConfig>
#include <bb/cascades/TouchEvent>
#include <bb/cascades/DropDown>
#include <bb/cascades/Option>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ActionBarAutoHideBehavior>
#include <bb/cascades/SystemDefaults>

#include <QVariantList>

StartPage::StartPage(bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane)
{
    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);

    inputField = bb::cascades::TextField::create();
    inputField->setHintText("Search YouTube or enter video URL");
    inputField->input()->setSubmitKey(bb::cascades::SubmitKey::Go);
    container->add(inputField);

    suggestionsList = bb::cascades::ListView::create();
    suggestionsList->setListItemProvider(new SuggestionsListItemProvider());
    suggestionsList->setVisible(false);
    suggestionsList->setMinHeight(ui->du(50));
    suggestionsList->setMaxHeight(ui->du(50));
    container->add(suggestionsList);

    filterContainer = bb::cascades::Container::create();
    filterContainerFirstRow = bb::cascades::Container::create().layout(
            bb::cascades::StackLayout::create().orientation(LayoutOrientation::LeftToRight));
    filterContainerSecondRow =
            bb::cascades::Container::create().layout(
                    bb::cascades::StackLayout::create().orientation(LayoutOrientation::LeftToRight)).topMargin(
                    ui->du(1));
    for (int i = 0; i < filterCount; i++) {
        filterDropDowns[i] = bb::cascades::DropDown::create().layoutProperties(
                bb::cascades::StackLayoutProperties::create().spaceQuota(1));
        if (i < 2) {
            filterContainerFirstRow->add(filterDropDowns[i]);
        } else {
            filterContainerSecondRow->add(filterDropDowns[i]);
        }
        QObject::connect(filterDropDowns[i], SIGNAL(selectedOptionChanged(bb::cascades::Option*)),
                this, SLOT(onSelectedOptionChanged(bb::cascades::Option*)));
    }
    filterContainer->add(filterContainerFirstRow);
    filterContainer->add(filterContainerSecondRow);
    filterContainer->setVisible(false);
    container->add(filterContainer);
    showFiltersActionItem = new bb::cascades::ActionItem();
    showFiltersActionItem->setEnabled(false);
    showFiltersActionItem->setTitle("Show Filters");
    showFiltersActionItem->addShortcut(bb::cascades::Shortcut::create().key("f"));
    this->addAction(showFiltersActionItem, bb::cascades::ActionBarPlacement::InOverflow);
    this->setActionBarAutoHideBehavior(bb::cascades::ActionBarAutoHideBehavior::HideOnScroll);
    QObject::connect(showFiltersActionItem, SIGNAL(triggered()), this,
            SLOT(onShowFiltersActionItemClick()));
    onOrientationChanged();

    searchResultsList = new CustomListView();
    SearchGrouppedListItemProvider *searchResultsListProvider = new SearchGrouppedListItemProvider(searchResultsList,
            new StartPageSearchListItemActionSetBuilder());
    searchResultsList->setListItemProvider(searchResultsListProvider);
    searchResultsList->setVisible(false);
    container->add(searchResultsList);

    noResultsContainer = bb::cascades::Container::create().visible(false).layoutProperties(
            bb::cascades::StackLayoutProperties::create().spaceQuota(1));
    noResultsContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    noResultsContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    noResultsContainer->setLayout(new bb::cascades::DockLayout());
    bb::cascades::Container *noResultsLabelContainer = new bb::cascades::Container();
    noResultsLabelContainer->setLeftPadding(ui->du(5));
    noResultsLabelContainer->setRightPadding(ui->du(5));
    noResultsLabelContainer->setVerticalAlignment(bb::cascades::VerticalAlignment::Center);
    noResultsLabelContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    bb::cascades::Label *noResultsLabel = bb::cascades::Label::create().text("No results found");
    noResultsLabel->textStyle()->setBase(bb::cascades::SystemDefaults::TextStyles::titleText());
    noResultsLabel->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    noResultsLabelContainer->add(noResultsLabel);
    noResultsContainer->add(noResultsLabelContainer);
    container->add(noResultsContainer);

    root->add(container);

    root->add(overlay);

    this->setContent(root);

    QObject::connect(inputField, SIGNAL(textChanging(QString)), this,
            SLOT(onInputFieldChanging(QString)));
    QObject::connect(inputField->input(), SIGNAL(submitted(bb::cascades::AbstractTextControl *)),
            this, SLOT(onInputFieldSubmit()));
    QObject::connect(youtubeClient, SIGNAL(suggestionsReceived(QStringList)), this,
            SLOT(onSuggestionsReceived(QStringList)));
    QObject::connect(youtubeClient, SIGNAL(searchDataReceived(SearchData)), this,
            SLOT(onSearchDataReceived(SearchData)));
    QObject::connect(youtubeClient, SIGNAL(channelDataReceived(ChannelPageData)), this,
            SLOT(onChannelDataReceived(ChannelPageData)));
    QObject::connect(suggestionsList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onSuggestionSelected(QVariantList)));
    QObject::connect(searchResultsList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onSearchResultsListItemClick(QVariantList)));
    QObject::connect(searchResultsListProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));
    QObject::connect(searchResultsListProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)),
            this, SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(playerContext, SIGNAL(orientationChanged()), this,
            SLOT(onOrientationChanged()));

    inputField->requestFocus();
}

void StartPage::onInputFieldSubmit()
{
    inputField->loseFocus();
    QString text = inputField->text().trimmed();

    if (text == "") {
        return;
    }

    overlay->setVisible(true);
    suggestionsList->setVisible(false);
    filterContainer->setVisible(false);
    showFiltersActionItem->setTitle("Show Filters");
    youtubeClient->process(text);
}

void StartPage::onInputFieldChanging(QString text)
{
    if (!inputField->isFocused()) {
        return;
    }

    if (text == "") {
        suggestionsList->setVisible(false);

        return;
    }

    youtubeClient->suggestions(text);
}

void StartPage::onSuggestionSelected(QVariantList indexPath)
{
    suggestionsList->setVisible(false);

    if (indexPath.count() > 1) {
        QVariantMap map = suggestionsList->dataModel()->data(indexPath).toMap();

        inputField->setText(map["item"].toString());
        onInputFieldSubmit();
    }
}

void StartPage::onSuggestionsReceived(QStringList list)
{
    if (list.count() > 0) {
        bb::cascades::GroupDataModel *model = new bb::cascades::GroupDataModel(
                QStringList() << "category" << "item");
        QVariantMap map;

        for (int i = 0; i < list.count(); i++) {
            map["category"] = "category";
            map["item"] = list[i];
            model->insert(map);
        }

        suggestionsList->setDataModel(model);
        suggestionsList->setVisible(true);
    } else {
        suggestionsList->setVisible(false);
    }
}

void StartPage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    BasePage::onMetadataReceived(videoMetadata, storageData);
    searchResultsList->setEnabled(true);
}

void StartPage::onSearchDataReceived(SearchData searchData)
{
    overlay->setVisible(false);
    bb::cascades::GroupDataModel *groupModel = new bb::cascades::GroupDataModel(
            QStringList() << "category" << "sortOrder");
    groupModel->setGrouping(bb::cascades::ItemGrouping::ByFullValue);

    QList<VideoListItemModel*> list;
    int sortOrder = 0;
    int categorySortOrder = 1;
    for (int i = 0; i < searchData.channels.count(); i++) {
        VideoListItemModel *item = VideoListItemModel::mapChannel(&searchData.channels[i]);
        item->setCategory(QString::number(categorySortOrder) + ":Channels");
        item->setSortOrder(sortOrder++);


        groupModel->insert(item);
    }

    categorySortOrder = 2;

    for (int i = 0; i < searchData.videos.count(); i++) {
        VideoListItemModel *item = VideoListItemModel::mapVideo(&searchData.videos[i]);
        item->setCategory(QString::number(categorySortOrder) + ":Videos");
        item->setSortOrder(sortOrder++);
        groupModel->insert(item);
    }
    for (int i = 0; i < searchData.sections.count(); i++) {
        SearchDataSection section = searchData.sections[i];
        categorySortOrder++;
        for (int j = 0; j < section.videos.count(); j++) {
            VideoListItemModel *item = VideoListItemModel::mapVideo(&section.videos[j]);
            item->setCategory(QString::number(categorySortOrder) + ":" + section.title);
            item->setSortOrder(sortOrder++);
            groupModel->insert(item);
        }
    }

    bool hasResults = groupModel->size() > 0;

    searchResultsList->setDataModel(groupModel);
    searchResultsList->setVisible(hasResults);
    noResultsContainer->setVisible(!hasResults);

    isBuildingDropdowns = true;
    for (int i = 0; i < searchData.searchParamGroups.count(); i++) {
        SearchParamGroup group = searchData.searchParamGroups[i];
        int newIndex = 0;

        switch (i) {
            case 0:
            case 1:
            case 2:
                newIndex = i;
                break;
            case 4:
                newIndex = 3;
                break;
            default:
                newIndex = -1;
                break;
        }

        if (newIndex >= 0) {
            buildSearchFilterDropDown(filterDropDowns[newIndex], &group);
        }
    }
    isBuildingDropdowns = false;
    showFiltersActionItem->setEnabled(true);
}

void StartPage::buildSearchFilterDropDown(bb::cascades::DropDown *dropdown, SearchParamGroup *group)
{
    dropdown->removeAll();
    dropdown->setTitle(group->title);

    if (group->isRemovable) {
        dropdown->add(
                bb::cascades::Option::create().text("--Not set--").enabled(true).selected(false).value(
                        group->paramsToReset));
    }
    for (int i = 0; i < group->options.count(); i++) {
        SearchParamOption option = group->options[i];

        dropdown->add(
                bb::cascades::Option::create().text(option.title).enabled(option.enabled).selected(
                        option.selected).value(option.urlParams));
    }
}

void StartPage::onSelectedOptionChanged(bb::cascades::Option* option)
{
    if (isBuildingDropdowns) {
        return;
    }

    overlay->setVisible(true);
    suggestionsList->setVisible(false);
    youtubeClient->search(inputField->text(), option->value().toString());
}

void StartPage::onSearchResultsListItemClick(QVariantList indexPath)
{
    if (indexPath.count() > 1) {
        suggestionsList->setVisible(false);
        searchResultsList->setEnabled(false);

        VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
                searchResultsList->dataModel()->data(indexPath).value<QObject*>());

        if (item->type == VideoListItemModel::Video) {
            overlay->setVisible(true);
            youtubeClient->process("https://www.youtube.com/watch?v=" + item->id);
        } else if (item->type == VideoListItemModel::Channel) {
            overlay->setVisible(true);
            youtubeClient->channel(item->id);
        }
    }
}

void StartPage::onChannelDataReceived(ChannelPageData channelData)
{
    navigationPane->push(new ChannelPage(channelData, this->navigationPane));
    overlay->setVisible(false);
    searchResultsList->setEnabled(true);
}

void StartPage::onChannelActionItemClick(QVariantList indexPath)
{
    suggestionsList->setVisible(false);

    VideoListItemModel *item = qobject_cast<VideoListItemModel*>(
            searchResultsList->dataModel()->data(indexPath).value<QObject*>());

    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void StartPage::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    audioOnly = true;
    onSearchResultsListItemClick(indexPath);
}

void StartPage::onYoutubeError(QString message)
{
    searchResultsList->setEnabled(true);
    BasePage::onYoutubeError(message);
}

void StartPage::playVideoFromOutside(QString url)
{
    inputField->loseFocus();
    this->setAudioOnly(false);
    BasePage::playVideoFromOutside(url);
}
void StartPage::searchFromOutside(QString text)
{
    inputField->setText(text);
    onInputFieldSubmit();
}
void StartPage::onOrientationChanged()
{
    bb::cascades::LayoutOrientation::Type layoutOrientation;
    UIConfig *ui = filterContainer->ui();

    if (playerContext->getScreenWidth() < 800) {
        layoutOrientation = bb::cascades::LayoutOrientation::TopToBottom;
        filterContainerSecondRow->resetLeftPadding();
    } else {
        layoutOrientation = bb::cascades::LayoutOrientation::LeftToRight;
        filterContainerSecondRow->setLeftPadding(ui->du(2));
    }

    filterContainer->setLayout(bb::cascades::StackLayout::create().orientation(layoutOrientation));
}

void StartPage::onShowFiltersActionItemClick()
{
    bool isFilterVisible = filterContainer->isVisible();
    filterContainer->setVisible(!isFilterVisible);
    showFiltersActionItem->setTitle(isFilterVisible ? "Show Filters" : "Hide Filters");
}
