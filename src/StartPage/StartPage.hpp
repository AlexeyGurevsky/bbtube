#ifndef STARTPAGE_HPP_
#define STARTPAGE_HPP_

#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/SearchData.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/utils/BasePage.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/utils/CustomListView.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/TextField>
#include <bb/cascades/ListView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/DropDown>
#include <bb/cascades/ActionItem>

#include <QVariantList>

class StartPage: public BasePage
{
Q_OBJECT
private slots:
    void onInputFieldSubmit();
    void onInputFieldChanging(QString text);
    void onSuggestionsReceived(QStringList list);
    void onSuggestionSelected(QVariantList indexPath);
    void onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData);
    void onSearchDataReceived(SearchData searchData);
    void onSearchResultsListItemClick(QVariantList indexPath);
    void onChannelDataReceived(ChannelPageData);
    void onChannelActionItemClick(QVariantList);
    void onPlayAudioOnlyActionItemClick(QVariantList);
    void onYoutubeError(QString);
    void onSelectedOptionChanged(bb::cascades::Option*);
    void onOrientationChanged();
    void onShowFiltersActionItemClick();
private:
    bb::cascades::TextField *inputField;
    bb::cascades::ListView *suggestionsList;
    CustomListView *searchResultsList;
    bb::cascades::Container *noResultsContainer;

    bb::cascades::Container *filterContainer;
    bb::cascades::Container *filterContainerFirstRow;
    bb::cascades::Container *filterContainerSecondRow;
    static const int filterCount = 4;
    DropDown *filterDropDowns[filterCount];
    bool isBuildingDropdowns;
    bb::cascades::ActionItem *showFiltersActionItem;

    void buildSearchFilterDropDown(bb::cascades::DropDown *dropdown, SearchParamGroup *group);
public:
    StartPage(bb::cascades::NavigationPane *navigationPane);
    virtual ~StartPage()
    {
    }
    virtual void playVideoFromOutside(QString url);
    void searchFromOutside(QString text);
};

class StartPageSearchListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~StartPageSearchListItemActionSetBuilder()
    {
    }

    virtual void buildActionSet(const VideoListItemModel* item, SearchListItem *listItem)
    {
        if (item->type == VideoListItemModel::Video) {
            listItem->addContinueActionItem();
            if (!item->isLiveStream()) {
                listItem->addPlayAudioOnlyActionItem();
            }
            listItem->addChannelActionItem();
            buildCommonVideoActionSet(item, listItem);
        } else if (item->type == VideoListItemModel::Channel) {
            listItem->addAddToChannelsActionItem();
            buildCommonChannelActionSet(item, listItem);
        }
    }
};
#endif /* STARTPAGE_HPP_ */
