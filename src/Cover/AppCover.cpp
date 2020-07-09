#include "AppCover.hpp"
#include "src/applicationui.hpp"
#include "src/parser/models/VideoMetadata.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/LabelAutoSizeProperties>
#include <bb/cascades/UIConfig>
#include <bb/cascades/Color>

using namespace bb::cascades;

AppCover::AppCover()
{
    playerContext = ApplicationUI::playerContext;

    Container *root = new Container();
    UIConfig *uic = root->ui();

    root->setLayout(new DockLayout());
    root->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    root->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    root->setBackground(Color::Black);
    thumbnail = ImageView::create();
    thumbnail->setScalingMethod(ScalingMethod::AspectFit);
    thumbnail->setHorizontalAlignment(HorizontalAlignment::Fill);
    thumbnail->setVerticalAlignment(VerticalAlignment::Fill);
    root->add(thumbnail);

    titleContainer = new Container();
    title = new Label();
    title->setMultiline(true);
    title->autoSize()->setMaxLineCount(2);
    titleContainer->add(title);
    titleContainer->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    titleContainer->setLeftPadding(uic->du(1));
    titleContainer->setTopPadding(uic->du(1));
    titleContainer->setRightPadding(uic->du(1));
    titleContainer->setBottomPadding(uic->du(1));
    titleContainer->setBackground(Color::fromARGB(0xff323232));
    titleContainer->setOpacity(0.8);
    root->add(titleContainer);

    onMiniPlayerStopped();

    this->setContent(root);

    QObject::connect(playerContext, SIGNAL(metadataChanged()), this, SLOT(onMetadataChanged()));
    QObject::connect(playerContext, SIGNAL(miniPlayerStopped()), this, SLOT(onMiniPlayerStopped()));
}

void AppCover::onMetadataChanged()
{
    SingleVideoMetadata metadata = playerContext->getVideoMetadata().video;

    if (videoId == metadata.videoId) {
        return;
    }

    titleContainer->setVisible(true);
    title->setText(metadata.title);
    videoId = metadata.videoId;

    QString url = "https://i.ytimg.com/vi/" + videoId + "/default.jpg";

    QEventLoop loop;
    QNetworkRequest request(url);
    QNetworkReply *reply = ApplicationUI::networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (!reply->error()) {
        QByteArray image = reply->readAll();
        thumbnail->setHorizontalAlignment(HorizontalAlignment::Fill);
        thumbnail->setVerticalAlignment(VerticalAlignment::Fill);
        thumbnail->setImage(Image(image));
    } else {
        showDefaultImage();
    }
    reply->deleteLater();
}

void AppCover::onMiniPlayerStopped()
{
    titleContainer->setVisible(false);
    showDefaultImage();
}

void AppCover::showDefaultImage()
{
    thumbnail->setImageSource(QString("asset:///images/player_background.png"));
    thumbnail->setHorizontalAlignment(HorizontalAlignment::Center);
    thumbnail->setVerticalAlignment(VerticalAlignment::Center);
}
