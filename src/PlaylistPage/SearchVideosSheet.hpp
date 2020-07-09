#ifndef SearchVideosSheet_HPP_
#define SearchVideosSheet_HPP_

#include "src/models/PlaylistVideoModel.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/utils/BaseSheet.hpp"
#include "PlaylistPage.hpp"
#include "src/utils/CustomListView.hpp"

#include <QObject>
#include <bb/cascades/Container>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ListView>

using namespace bb::cascades;

class SearchVideosSheet: public BaseSheet
{
Q_OBJECT
public:
    SearchVideosSheet(UpdatableDataModel<PlaylistVideoModel*> *videosDataModel, PlaylistPage *playlistPage);
    virtual ~SearchVideosSheet()
    {
    }
private slots:
    void onInputFieldChanging(QString);
    void onSearchResultsListItemClick(QVariantList);
    void onChannelActionItemClick(QVariantList);
    void onPlayAudioOnlyActionItemClick(QVariantList);
    void onDeleteActionItemClick(QVariantList);
private:
    CustomListView *searchResultsList;
    Container *noResultsContainer;
    Container *overlay;
    UpdatableDataModel<PlaylistVideoModel*> *videosDataModel;
    PlaylistPage *playlistPage;
};

#endif /* SearchVideosSheet_HPP_ */
