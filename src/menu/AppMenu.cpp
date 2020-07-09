#include "AppMenu.hpp"
#include "src/Equalizer/EqualizerSheet.hpp"
#include "src/settings/SettingsSheet.hpp"
#include "src/info/InfoSheet.hpp"

#include <bb/cascades/Menu>
#include <bb/cascades/HelpActionItem>
#include <bb/cascades/SettingsActionItem>
#include <bb/cascades/ActionItem>

AppMenu::AppMenu()
{
    HelpActionItem *help = new HelpActionItem;
    help->setImageSource(QString("asset:///images/ic_info.png"));
    help->setTitle("Info");
    SettingsActionItem *settings = new SettingsActionItem;
    ActionItem *equalizer = ActionItem::create().title("Equalizer").imageSource(
            QString("asset:///images/ic_equalizer.png"));

    this->setHelpAction(help);
    this->setSettingsAction(settings);
    this->addAction(equalizer);

    QObject::connect(help, SIGNAL(triggered()), this, SLOT(onHelpTriggered()));
    QObject::connect(settings, SIGNAL(triggered()), this, SLOT(onSettingsTriggered()));
    QObject::connect(equalizer, SIGNAL(triggered()), this, SLOT(onEqualizerTriggered()));
}

void AppMenu::onHelpTriggered()
{
    InfoSheet *sheet = new InfoSheet();
}

void AppMenu::onEqualizerTriggered()
{
    EqualizerSheet *sheet = new EqualizerSheet();
}

void AppMenu::onSettingsTriggered()
{
    SettingsSheet *sheet = new SettingsSheet();
}

