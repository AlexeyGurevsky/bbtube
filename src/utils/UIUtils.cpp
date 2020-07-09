#include "UIUtils.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/Color>
#include <bb/cascades/UIConfig>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

bb::cascades::Container* UIUtils::createOverlay()
{
    bb::cascades::Container *overlay = new bb::cascades::Container();
    bb::cascades::UIConfig *ui = overlay->ui();
    overlay->setVerticalAlignment(bb::cascades::VerticalAlignment::Fill);
    overlay->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Fill);
    overlay->setOpacity(0.5);
    overlay->setBackground(bb::cascades::Color::Gray);
    overlay->setVisible(false);
    overlay->setLayout(new bb::cascades::DockLayout());
    bb::cascades::ActivityIndicator *spinner = bb::cascades::ActivityIndicator::create();
    spinner->setVerticalAlignment(bb::cascades::VerticalAlignment::Center);
    spinner->setHorizontalAlignment(bb::cascades::HorizontalAlignment::Center);
    spinner->setPreferredWidth(ui->du(40));
    spinner->setPreferredHeight(ui->du(40));
    spinner->start();
    overlay->add(spinner);

    return overlay;
}

void UIUtils::toastError(QString message)
{
    bb::system::SystemToast *toast = new bb::system::SystemToast();

    toast->setBody(message);
    toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast->show();
}

void UIUtils::toastInfo(QString message)
{
    bb::system::SystemToast *toast = new bb::system::SystemToast();

    toast->setBody(message);
    toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast->show();
}
