#include "TrendingPage.hpp"
#include "src/PlayerPage/PlayerPage.hpp"
#include "src/ChannelPage/ChannelPage.hpp"
#include "src/parser/YoutubeClient.hpp"
#include "src/parser/models/StorageData.hpp"
#include "src/parser/models/VideoMetadata.hpp"
#include "src/parser/models/ChannelData.hpp"
#include "src/parser/models/TrendingData.hpp"
#include "src/models/VideoListItemModel.hpp"
#include "src/VideoList/SearchListItemProvider.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/utils/MiniPlayer.hpp"

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
#include <bb/cascades/Shortcut>
#include <bb/cascades/Option>

#include <QVariantList>

TrendingPage::TrendingPage(bb::cascades::NavigationPane *navigationPane) :
        BasePage(navigationPane), isLoaded(false)
{
    bb::cascades::Container *container = new bb::cascades::Container();
    container->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    container->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);

    container->add(miniPlayer);
    container->add(getTitleContainer());
    setTitle("Trending");

    segmented = bb::cascades::SegmentedControl::create();
    segmented->setVisible(false);
    container->add(segmented);

    videoList = new CustomListView();
    SearchFlatListItemProvider *listProvider = new SearchFlatListItemProvider(videoList,
            new TrendingPageListItemActionSetBuilder());
    videoList->setListItemProvider(listProvider);
    container->add(videoList);
    root->add(container);

    root->add(overlay);

    this->setContent(root);

    QObject::connect(youtubeClient, SIGNAL(trendingDataReceived(TrendingData)), this,
            SLOT(onTrendingDataReceived(TrendingData)));
    QObject::connect(youtubeClient, SIGNAL(channelDataReceived(ChannelPageData)), this,
            SLOT(onChannelDataReceived(ChannelPageData)));
    QObject::connect(videoList, SIGNAL(triggered(QVariantList)), this,
            SLOT(onListItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(playAudioOnlyActionItemClick(QVariantList)), this,
            SLOT(onPlayAudioOnlyActionItemClick(QVariantList)));
    QObject::connect(listProvider, SIGNAL(channelActionItemClick(QVariantList)), this,
            SLOT(onChannelActionItemClick(QVariantList)));

    bb::cascades::ActionItem *refreshActionItem = bb::cascades::ActionItem::create();
    refreshActionItem->setImageSource(QString("asset:///images/ic_reload.png"));
    refreshActionItem->setTitle("Refresh");
    refreshActionItem->addShortcut(bb::cascades::Shortcut::create().key("r"));
    this->addAction(refreshActionItem, bb::cascades::ActionBarPlacement::Signature);
    QObject::connect(refreshActionItem, SIGNAL(triggered()), this,
            SLOT(onRefreshActionItemClick()));

    overlay->setVisible(true);
}

void TrendingPage::onMetadataReceived(VideoMetadata videoMetadata, StorageData storageData)
{
    BasePage::onMetadataReceived(videoMetadata, storageData);
    videoList->setEnabled(true);
}

void TrendingPage::onListItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = videoList->dataModel()->data(indexPath).value<VideoListItemModel*>();

    if (item->type == VideoListItemModel::Video) {
        videoList->setEnabled(false);
        overlay->setVisible(true);
        youtubeClient->process("https://www.youtube.com/watch?v=" + item->id);
    }
}

void TrendingPage::onYoutubeError(QString message)
{
    videoList->setEnabled(true);
    BasePage::onYoutubeError(message);
}

void TrendingPage::onPlayAudioOnlyActionItemClick(QVariantList indexPath)
{
    audioOnly = true;
    onListItemClick(indexPath);
}

void TrendingPage::onChannelActionItemClick(QVariantList indexPath)
{
    VideoListItemModel *item = videoList->dataModel()->data(indexPath).value<VideoListItemModel*>();
    overlay->setVisible(true);
    youtubeClient->channel(item->channelId);
}

void TrendingPage::onChannelDataReceived(ChannelPageData data)
{
    navigationPane->push(new ChannelPage(data, this->navigationPane));
    overlay->setVisible(false);
    videoList->setEnabled(true);
}

void TrendingPage::onRefreshActionItemClick()
{
    overlay->setVisible(true);
    modelsMap.clear();
    isLoaded = true;
    youtubeClient->trending();
}

void TrendingPage::onSegmentedSelectedIndexChanged(int index)
{
    if (modelsMap.contains(index)) {
        videoList->setDataModel(modelsMap[index]);
        videoList->requestFocus();

        return;
    }

    overlay->setVisible(true);
    youtubeClient->trending(categories[index - 1].categoryKey);
}

void TrendingPage::lazyLoad()
{
    if (!isLoaded) {
        onRefreshActionItemClick();
    }
}

void TrendingPage::onTrendingDataReceived(TrendingData data)
{
    QListDataModel<VideoListItemModel*> *model = new QListDataModel<VideoListItemModel*>();
    for (int i = 0; i < data.videos.count(); i++) {
        VideoListItemModel *item = VideoListItemModel::mapVideo(&data.videos[i]);
        model->append(item);
    }

    modelsMap.insert((data.categories.count() ? 0 : segmented->selectedIndex()), model);
    videoList->setDataModel(model);
    videoList->requestFocus();
    overlay->setVisible(false);
    videoList->setVisible(true);

    if (data.categories.count() > 0) {
        categories = data.categories;
        QObject::disconnect(segmented, SIGNAL(selectedIndexChanged(int)), 0, 0);
        segmented->removeAll();
        segmented->add(bb::cascades::Option::create().text("All").selected(true));
        for (int i = 0; i < data.categories.count(); i++) {
            segmented->add(bb::cascades::Option::create().text(data.categories[i].title));
        }
        segmented->setVisible(true);
        QObject::connect(segmented, SIGNAL(selectedIndexChanged(int)), this,
                SLOT(onSegmentedSelectedIndexChanged(int)));
    }
}
