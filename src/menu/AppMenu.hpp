#ifndef APPMENU_HPP_
#define APPMENU_HPP_

#include <bb/cascades/Menu>
using namespace bb::cascades;
class AppMenu : public Menu
{
    Q_OBJECT
public:
    AppMenu();
    ~AppMenu()
    {
    }
private slots:
    void onHelpTriggered();
    void onEqualizerTriggered();
    void onSettingsTriggered();
};


#endif /* APPMENU_HPP_ */
