#include "InfoSheet.hpp"
#include "src/applicationui.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ActionItem>
#include <bb/cascades/UIConfig>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/ActionItem>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeManager>
#include <bb/ApplicationInfo>

InfoSheet::InfoSheet() :
        BaseSheet()
{
    Page *content = new Page();
    Container *container = Container::create();
    UIConfig *ui = container->ui();
    container->setTopPadding(ui->du(2));
    container->setRightPadding(ui->du(1));
    container->setBottomPadding(ui->du(1));
    container->setLeftPadding(ui->du(1));
    container->add(
            Label::create().multiline(true).text(
                    "Author: Alexey Gurevski\nVersion:" + bb::ApplicationInfo().version()
                            + "\nFor more info visit the CrackBerry Forum."));
    container->add(
            Label::create().multiline(true).text(
                    "If you want to donate please leave [BBTube] tag in the comments field.\nThank you."));
    TitleBar *titleBar = new TitleBar(TitleBarKind::Default);
    ActionItem *closeAction = ActionItem::create().title("Back");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    titleBar->setTitle("About");
    titleBar->setDismissAction(closeAction);

    content->setTitleBar(titleBar);
    content->setContent(container);
    this->setContent(content);

    bb::cascades::ActionItem *crackberryActionItem = new bb::cascades::ActionItem();
    crackberryActionItem->setTitle("CrackBerry");
    crackberryActionItem->setImageSource(QString("asset:///images/ic_browser.png"));
    content->addAction(crackberryActionItem, bb::cascades::ActionBarPlacement::Signature);
    QObject::connect(crackberryActionItem, SIGNAL(triggered()), this,
            SLOT(onCrackberryActionItemClick()));
    bb::cascades::ActionItem *donateActionItem = new bb::cascades::ActionItem();
    donateActionItem->setTitle("Donate");
    donateActionItem->setImageSource(QString("asset:///images/ic_paypal.png"));
    content->addAction(donateActionItem, bb::cascades::ActionBarPlacement::OnBar);
    QObject::connect(donateActionItem, SIGNAL(triggered()), this, SLOT(onDonateActionItemClick()));
    open();
}

void InfoSheet::onCrackberryActionItemClick()
{
    bb::system::InvokeRequest request;
    request.setAction("bb.action.OPEN");
    request.setTarget("sys.browser");
    request.setUri("https://forums.crackberry.com/forum-f274/a-1184096/");

    bb::system::InvokeTargetReply* reply = (new bb::system::InvokeManager)->invoke(request);
    reply->deleteLater();
}

void InfoSheet::onDonateActionItemClick()
{
    bb::system::InvokeRequest request;
    request.setAction("bb.action.OPEN");
    request.setTarget("sys.browser");
    request.setUri("https://paypal.me/nagmet");

    bb::system::InvokeTargetReply* reply = (new bb::system::InvokeManager)->invoke(request);
    reply->deleteLater();
}
