#include "EqualizerSheet.hpp"
#include "EqualizerListItemProvider.hpp"
#include "EqualizerListItemModel.hpp"
#include "src/utils/GlobalPlayerContext.hpp"
#include "src/applicationui.hpp"
#include "src/utils/UpdatableDataModel.hpp"
#include "src/db/DbHelper.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ActionItem>
#include <bb/multimedia/EqualizerPreset>
#include <bb/cascades/QListDataModel>

EqualizerSheet::EqualizerSheet() :
        BaseSheet()
{
    playerContext = ApplicationUI::playerContext;

    Page *content = new Page();
    Container *container = new Container();

    listView = new ListView();
    setModel(playerContext->getEqualizerPreset());
    listView->setListItemProvider(new EqualizerListItemProvider());
    container->add(listView);

    TitleBar *_titleBar = new TitleBar(TitleBarKind::Default);
    ActionItem *closeAction = ActionItem::create().title("Cancel");
    ActionItem *saveAction = ActionItem::create().title("Done");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(saveActionClick()));
    QObject::connect(listView, SIGNAL(triggered(QVariantList)), this,
            SLOT(onListViewTriggered(QVariantList)));
    _titleBar->setTitle("Equalizer");
    _titleBar->setDismissAction(closeAction);
    _titleBar->setAcceptAction(saveAction);

    content->setTitleBar(_titleBar);
    content->setContent(container);
    this->setContent(content);

    open();
}

void EqualizerSheet::saveActionClick()
{
    EqualizerListItemModel *model = listView->dataModel()->data(selectedIndexPath).value<
            EqualizerListItemModel *>();
    playerContext->setEqualizerPreset(model->type);
    DbHelper::setEqualizerPreset((int)model->type);

    closeSheet();
}

void EqualizerSheet::onListViewTriggered(QVariantList indexPath)
{
    if (selectedIndexPath == indexPath) {
        return;
    }

    UpdatableDataModel<EqualizerListItemModel *> *dataModel = (UpdatableDataModel<
            EqualizerListItemModel *> *) listView->dataModel();
    EqualizerListItemModel *oldValue = dataModel->data(selectedIndexPath).value<
            EqualizerListItemModel *>();
    EqualizerListItemModel *newValue = dataModel->data(indexPath).value<EqualizerListItemModel *>();

    oldValue->isSelected = false;
    dataModel->updateItem(selectedIndexPath);
    newValue->isSelected = true;
    dataModel->updateItem(indexPath);

    selectedIndexPath = indexPath;
}

void EqualizerSheet::setModel(bb::multimedia::EqualizerPreset::Type currentPreset)
{
    UpdatableDataModel<EqualizerListItemModel *> *model = new UpdatableDataModel<
            EqualizerListItemModel *>();
    QStringList titles;
    titles << "Off" << "Airplane" << "Bass Boost" << "Treble Boost" << "Bass Lower"
            << "Treble Lower" << "Voice Lower" << "Acoustic" << "Dance" << "Electronic" << "Hip Hop"
            << "Jazz" << "Lounge" << "Piano" << "R&B" << "Rock" << "Spoken Word";
    for (int i = 0; i < titles.count(); i++) {
        bb::multimedia::EqualizerPreset::Type preset = (bb::multimedia::EqualizerPreset::Type) i;
        EqualizerListItemModel *item = new EqualizerListItemModel(preset, titles[i], currentPreset == preset);
        model->append(item);

        if (currentPreset == preset) {
            selectedIndexPath = (QVariantList() << i);
        }
    }

    listView->setDataModel(model);
}
