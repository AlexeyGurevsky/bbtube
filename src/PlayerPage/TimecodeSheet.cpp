#include "TimecodeSheet.hpp"
#include "TimecodeListItemProvider.hpp"
#include "TimecodeListItemModel.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include "src/applicationui.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/parser/models/VideoMetadata.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ActionItem>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/ScrollAnimation>
#include <bb/cascades/ScrollPosition>

TimecodeSheet::TimecodeSheet() :
        BaseSheet()
{
    playerContext = ApplicationUI::playerContext;

    Page *content = new Page();
    Container *container = new Container();

    listView = new ListView();
    listView->setListItemProvider(new TimecodeListItemProvider());
    container->add(listView);

    TitleBar *_titleBar = new TitleBar(TitleBarKind::Default);
    ActionItem *closeAction = ActionItem::create().title("Close");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    QObject::connect(listView, SIGNAL(triggered(QVariantList)), this,
            SLOT(onListViewTriggered(QVariantList)));
    _titleBar->setTitle("Timestamps");
    _titleBar->setDismissAction(closeAction);

    content->setTitleBar(_titleBar);
    content->setContent(container);
    this->setContent(content);

    QObject::connect(playerContext, SIGNAL(metadataChanged()), this, SLOT(onMetadataChanged()));
    QObject::connect(playerContext, SIGNAL(positionChanged(unsigned int)), this,
            SLOT(onPlayerPositionChanged(unsigned int)));

    setModel(true);
    open();
}

void TimecodeSheet::onListViewTriggered(QVariantList indexPath)
{
    UpdatableDataModel<TimecodeListItemModel *> *dataModel = (UpdatableDataModel<
            TimecodeListItemModel *> *) listView->dataModel();
    TimecodeListItemModel *item = dataModel->data(indexPath).value<TimecodeListItemModel *>();

    closeSheet();
    playerContext->seekTime(item->seconds * 1000);
}

void TimecodeSheet::setModel(bool scrollToPlaying)
{
    QListDataModel<TimecodeListItemModel*> *model = new QListDataModel<TimecodeListItemModel *>();
    QList<Timecode> timecodes = playerContext->getVideoMetadata().timecodes;
    int scrollIndex = -1;

    for (int i = 0; i < timecodes.count(); i++) {
        int currentPosition = playerContext->getPosition() / 1000;
        TimecodeListItemModel *item = new TimecodeListItemModel();
        item->text = timecodes[i].time + timecodes[i].description;
        item->seconds = timecodes[i].seconds;

        if (scrollIndex == -1) {
            item->isPlaying =
                    i == timecodes.count() - 1 ?
                            currentPosition >= timecodes[i].seconds :
                            currentPosition >= timecodes[i].seconds
                                    && currentPosition < timecodes[i + 1].seconds;
            if (item->isPlaying) {
                scrollIndex = i;
            }
        } else {
            item->isPlaying = false;
        }

        model->append(item);
    }

    listView->setDataModel(model);

    if (scrollIndex >= 1 && scrollToPlaying) { //just a magic number
        listView->scrollToItem(QVariantList() << (scrollIndex - 1),
                bb::cascades::ScrollAnimation::None);
    }
}

void TimecodeSheet::onMetadataChanged()
{
    if (playerContext->getVideoMetadata().timecodes.count() > 0) {
        listView->scrollToPosition(bb::cascades::ScrollPosition::Beginning,
                bb::cascades::ScrollAnimation::None);
        setModel(false);

        return;
    }

    closeSheet();
}

void TimecodeSheet::onPlayerPositionChanged(unsigned int position)
{
    UpdatableDataModel<TimecodeListItemModel*> *model =
            (UpdatableDataModel<TimecodeListItemModel*> *) listView->dataModel();
    int oldPlaying = -1;
    int newPlaying = -1;
    int currentPosition = position / 1000;

    for (int i = 0; i < model->size(); i++) {
        TimecodeListItemModel *item = model->value(i);

        if (item->isPlaying) {
            oldPlaying = i;
            item->isPlaying = false;

            break;
        }
    }

    for (int i = 0; i < model->size(); i++) {
        TimecodeListItemModel *item = model->value(i);

        item->isPlaying =
                i == model->size() - 1 ?
                        currentPosition >= item->seconds :
                        currentPosition >= item->seconds
                                && currentPosition < model->value(i + 1)->seconds;

        if (item->isPlaying) {
            newPlaying = i;

            break;
        }
    }

    if (oldPlaying != newPlaying) {
        if (oldPlaying >= 0) {
            model->updateItem(QVariantList() << oldPlaying);
        }
        if (newPlaying >= 0) {
            model->updateItem(QVariantList() << newPlaying);
        }
    }
}
