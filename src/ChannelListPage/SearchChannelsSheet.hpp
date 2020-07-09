#ifndef SearchChannelsSheet_HPP_
#define SearchChannelsSheet_HPP_

#include "src/models/VideoListItemModel.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/utils/BaseSheet.hpp"
#include "ChannelListPage.hpp"
#include "src/utils/CustomListView.hpp"

#include <QObject>
#include <bb/cascades/TextArea>
#include <bb/cascades/Container>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ListView>

using namespace bb::cascades;

class SearchChannelsSheet: public BaseSheet
{
Q_OBJECT
public:
    SearchChannelsSheet(UpdatableDataModel<ChannelListItemModel*> *channelsDataModel,
            UpdatableDataModel<VideoListItemModel*> *feedDataModel,
            ChannelListPage *channelListPage);
    virtual ~SearchChannelsSheet()
    {
    }
private slots:
    void onInputFieldChanging(QString);
    void onSearchResultsListItemClick(QVariantList);
    void onChannelActionItemClick(QVariantList);
    void onPlayAudioOnlyActionItemClick(QVariantList);
private:
    CustomListView *searchResultsList;
    Container *noResultsContainer;
    Container *overlay;
    UpdatableDataModel<ChannelListItemModel*> *channelsDataModel;
    UpdatableDataModel<VideoListItemModel*> *feedDataModel;
    ChannelListPage *channelListPage;
};

class SearchChannelsSheetListItemActionSetBuilder: public SearchListItemActionSetBuilder
{
public:
    virtual ~SearchChannelsSheetListItemActionSetBuilder()
    {
    }

    virtual void buildActionSet(const VideoListItemModel* item, SearchListItem *listItem)
    {
        if (item->type == VideoListItemModel::Video) {
            listItem->addContinueActionItem();
            listItem->addPlayAudioOnlyActionItem();
            listItem->addChannelActionItem();
            buildCommonVideoActionSet(item, listItem);
        } else if (item->type == VideoListItemModel::Channel) {
            buildCommonChannelActionSet(item, listItem);
        }
    }
};

#endif /* SearchChannelsSheet_HPP_ */
