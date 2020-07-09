#include "ImportChannelsSheet.hpp"
#include "src/utils/UIUtils.hpp"
#include "src/models/ChannelListItemModel.hpp"
#include "src/db/DbHelper.hpp"
#include "src/utils/ChannelListProxy.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/TextArea>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ActionItem>
#include <bb/cascades/UIConfig>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeTargetReply>
#include <bb/data/XmlDataAccess>

ImportChannelsSheet::ImportChannelsSheet() :
        BaseSheet()
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

    container->add(Label::create().text("Steps:").topMargin(0));
    container->add(
            Label::create().text("1. Make sure you're signed in to YouTube in the browser").multiline(
                    true).topMargin(0));
    container->add(
            Label::create().text(
                    "2. Download your subscriptions by tapping the action on the bar below. Save the file as a .txt file, for example, subscriptions.txt").multiline(
                    true).topMargin(0));
    container->add(Label::create().text("3. Open the file and copy its contents").topMargin(0));
    container->add(Label::create().text("4. Paste the contents into the area below").topMargin(0));
    container->add(Label::create().text("5. Tap the Import action").topMargin(0));

    textArea = TextArea::create().topMargin(ui->du(5)).hintText("Paste here").layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    container->add(textArea);
    root->add(container);

    overlay = UIUtils::createOverlay();
    root->add(overlay);

    TitleBar *titleBar = new TitleBar(TitleBarKind::Default);
    ActionItem *closeAction = ActionItem::create().title("Cancel");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    titleBar->setTitle("Import Channels");
    titleBar->setDismissAction(closeAction);

    content->setTitleBar(titleBar);
    content->setContent(root);
    this->setContent(content);

    importActionItem = bb::cascades::ActionItem::create();
    importActionItem->setImageSource(QString("asset:///images/ic_import.png"));
    importActionItem->setTitle("Import");
    bb::cascades::ActionItem *downloadActionItem = bb::cascades::ActionItem::create();
    downloadActionItem->setImageSource(QString("asset:///images/ic_download.png"));
    downloadActionItem->setTitle("Download");

    content->addAction(importActionItem, bb::cascades::ActionBarPlacement::Signature);
    content->addAction(downloadActionItem, bb::cascades::ActionBarPlacement::OnBar);

    QObject::connect(importActionItem, SIGNAL(triggered()), this, SLOT(onImportActionItemClick()));
    QObject::connect(downloadActionItem, SIGNAL(triggered()), this,
            SLOT(onDownloadActionItemClick()));

    open();
}

void ImportChannelsSheet::onImportActionItemClick()
{
    int addedChannelsCount = 0;

    if (textArea->text() != "") {
        overlay->setVisible(true);
        importActionItem->setEnabled(false);

        bb::data::XmlDataAccess xda;
        QVariantList list = xda.loadFromBuffer(textArea->text(), "/body/outline/outline").value<
                QVariantList>();

        if (list.count() > 0) {
            DbHelper::transaction();

            for (int i = 0; i < list.count(); i++) {
                QVariantMap feed = list[i].toMap();
                ChannelListItemModel channel;

                channel.channelId = QUrl(feed["xmlUrl"].toString()).queryItemValue("channel_id");
                channel.dateAdded = QDateTime::currentDateTimeUtc().toTime_t();
                channel.dateLastActivity = 0;
                channel.dateLastVisited = 0;
                channel.lastVideoId = "";
                channel.lastVideoTitle = "";
                channel.thumbnailUrl = "";
                channel.title = feed["title"].toString();

                if (!ChannelListProxy::getInstance()->contains(channel.channelId)) {
                    DbHelper::createChannel(&channel);
                    ChannelListProxy::getInstance()->add(&channel);
                    addedChannelsCount++;
                }
            }

            DbHelper::commit();
        }

        overlay->setVisible(false);
        importActionItem->setEnabled(true);
    }

    UIUtils::toastInfo("Added " + QString::number(addedChannelsCount) + " new channel(s)");
    closeSheet();
}

void ImportChannelsSheet::onDownloadActionItemClick()
{
    bb::system::InvokeRequest request;
    request.setAction("bb.action.OPEN");
    request.setTarget("sys.browser");
    request.setUri("https://www.youtube.com/subscription_manager?action_takeout=1");

    bb::system::InvokeTargetReply* reply = (new bb::system::InvokeManager)->invoke(request);
    reply->deleteLater();
}
