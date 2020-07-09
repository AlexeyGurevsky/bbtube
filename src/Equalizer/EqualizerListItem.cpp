#include "EqualizerListItem.hpp"
#include "EqualizerListItemModel.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/Color>
#include <bb/cascades/ColorPaint>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/UIConfig>
#include <bb/cascades/UIPalette>
#include <bb/cascades/CustomListItem>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/ImageView>

using namespace bb::cascades;

EqualizerListItem::EqualizerListItem(Container *parent) :
        CustomListItem(parent)
{
    Container *rootContainer = new Container();
    StackLayout *layout = new StackLayout();
    layout->setOrientation(LayoutOrientation::LeftToRight);
    rootContainer->setLayout(layout);
    ui = rootContainer->ui();
    rootContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    rootContainer->setVerticalAlignment(VerticalAlignment::Center);
    rootContainer->setLeftPadding(ui->du(1));
    rootContainer->setTopPadding(ui->du(1));
    rootContainer->setRightPadding(ui->du(1));
    rootContainer->setBottomPadding(ui->du(1));

    Container *titleLabelContainer = Container::create().layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    title = Label::create().text(" ");
    title->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    titleLabelContainer->add(title);
    titleLabelContainer->setVerticalAlignment(VerticalAlignment::Center);
    rootContainer->add(titleLabelContainer);
    selectedIcon = ImageView::create(QString("asset:///images/ic_done.png")).preferredWidth(48).preferredHeight(48);
    rootContainer->add(selectedIcon);

    setDividerVisible(true);
    setContent(rootContainer);
}

void EqualizerListItem::update(const EqualizerListItemModel *item)
{
    FontWeight::Type weight = FontWeight::Normal;

    if (item->isSelected) {
        weight = FontWeight::Bold;
    }

    title->setText(item->title);
    title->textStyle()->setFontWeight(weight);
    selectedIcon->setVisible(item->isSelected);
}

void EqualizerListItem::select(bool select)
{
}

void EqualizerListItem::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);

    select(selected);
}

void EqualizerListItem::activate(bool activate)
{
    select(activate);
}
