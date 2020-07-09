#include "SettingsSheet.hpp"
#include "src/applicationui.hpp"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/TextField>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ActionItem>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/UIConfig>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/Divider>
#include <bb/cascades/TextFieldInputMode>
#include <bb/cascades/Dropdown>
#include <bb/cascades/Option>

SettingsSheet::SettingsSheet() :
        BaseSheet()
{
    appSettings = ApplicationUI::appSettings;

    Page *content = new Page();
    Container *container = Container::create();
    UIConfig *ui = container->ui();
    container->setTopPadding(ui->du(2));
    container->setRightPadding(ui->du(1));
    container->setBottomPadding(ui->du(1));
    container->setLeftPadding(ui->du(1));

    tabDropdown = DropDown::create().title("Default Tab");
    container->add(tabDropdown);
    QStringList options;
    options << "Search" << "Channels" << "Playlists" << "Recommended" << "Trending";
    for (int i = 0; i < options.count(); i++) {
        tabDropdown->add(
                Option::create().text(options[i]).value(options[i]).selected(
                        appSettings->defaultTab() == options[i]));
    }

    container->add(Divider::create());

    qualityDropdown = DropDown::create().title("Default Quality");
    container->add(qualityDropdown);

    options.clear();
    options << "240p" << "360p" << "720p";
    for (int i = 0; i < options.count(); i++) {
        qualityDropdown->add(
                Option::create().text(options[i]).value(options[i]).selected(
                        appSettings->defaultQuality() == options[i]));
    }

    Container *autoplayContainer = Container::create();
    StackLayout *autoplayLayout = new StackLayout();
    autoplayLayout->setOrientation(LayoutOrientation::LeftToRight);
    autoplayContainer->setLayout(autoplayLayout);
    Container *autoplayLabelContainer = Container::create().layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    autoplayLabelContainer->setVerticalAlignment(VerticalAlignment::Center);
    Label *autoplayLabel = Label::create().text("Autoplay");
    autoplayLabel->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    autoplayLabelContainer->add(autoplayLabel);
    autoplayButton = ToggleButton::create().checked(appSettings->isAutoplay());
    autoplayContainer->add(autoplayLabelContainer);
    autoplayContainer->add(autoplayButton);
    container->add(autoplayContainer);

    Container *playbackTimeoutContainer = Container::create().topMargin(ui->du(2));
    StackLayout *playbackTimeoutLayout = new StackLayout();
    playbackTimeoutLayout->setOrientation(LayoutOrientation::LeftToRight);
    playbackTimeoutContainer->setLayout(playbackTimeoutLayout);
    Container *playbackTimeoutLabelContainer = Container::create().layoutProperties(
            StackLayoutProperties::create().spaceQuota(1));
    playbackTimeoutLabelContainer->setVerticalAlignment(VerticalAlignment::Center);
    Label *playbackTimeoutLabel = Label::create().text("Pause playback after").multiline(true);
    playbackTimeoutLabel->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    playbackTimeoutLabelContainer->add(playbackTimeoutLabel);
    playbackTimeoutTextField =
            TextField::create().hintText("minutes").preferredWidth(ui->du(20)).text(
                    QString::number(appSettings->getPlaybackTimeout())).inputMode(
                    TextFieldInputMode::NumbersAndPunctuation);
    playbackTimeoutContainer->add(playbackTimeoutLabelContainer);
    playbackTimeoutContainer->add(playbackTimeoutTextField);
    container->add(playbackTimeoutContainer);
    Container *playbackTimeoutCommentContainer = Container::create();
    playbackTimeoutCommentContainer->setTopMargin(ui->du(1));
    playbackTimeoutCommentContainer->setHorizontalAlignment(HorizontalAlignment::Fill);
    Label *playbackTimeoutCommentLabel = Label::create().multiline(true);
    playbackTimeoutCommentLabel->setText(
            "number of minutes playback will stop after while the app is in the background; from 0-1440 range, 0 means \"No Timeout\"");
    playbackTimeoutCommentLabel->textStyle()->setBase(SystemDefaults::TextStyles::subtitleText());
    playbackTimeoutCommentContainer->add(playbackTimeoutCommentLabel);
    container->add(playbackTimeoutCommentContainer);

    TitleBar *titleBar = new TitleBar(TitleBarKind::Default);
    ActionItem *closeAction = ActionItem::create().title("Cancel");
    ActionItem *saveAction = ActionItem::create().title("Done");
    QObject::connect(closeAction, SIGNAL(triggered()), this, SLOT(closeActionClick()));
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(saveActionClick()));
    titleBar->setTitle("Settings");
    titleBar->setDismissAction(closeAction);
    titleBar->setAcceptAction(saveAction);

    content->setTitleBar(titleBar);
    content->setContent(container);
    this->setContent(content);

    open();
}

void SettingsSheet::saveActionClick()
{
    appSettings->setAutoplay(autoplayButton->isChecked());
    bool parsedOk = false;
    int playbackTimeout = playbackTimeoutTextField->text().trimmed().toInt(&parsedOk);
    if (!parsedOk) {
        playbackTimeout = 0;
    } else {
        if (playbackTimeout < 0) {
            playbackTimeout = 0;
        } else if (playbackTimeout > 1440) {
            playbackTimeout = 1440;
        }
    }

    appSettings->setPlaybackTimeout(playbackTimeout);
    appSettings->setDefaultTab(tabDropdown->selectedValue().toString());
    appSettings->setDefaultQuality(qualityDropdown->selectedValue().toString());

    closeSheet();
}
