#ifndef SETTINGSSHEET_HPP_
#define SETTINGSSHEET_HPP_

#include "AppSettings.hpp"
#include "src/utils/BaseSheet.hpp"

#include <QObject>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/TextField>
#include <bb/cascades/Dropdown>

using namespace bb::cascades;

class SettingsSheet: public BaseSheet
{
    Q_OBJECT
public:
    SettingsSheet();
    virtual ~SettingsSheet()
    {
    }
private slots:
    void saveActionClick();
private:
    AppSettings *appSettings;
    ToggleButton *autoplayButton;
    TextField *playbackTimeoutTextField;
    DropDown *tabDropdown;
    DropDown *qualityDropdown;
};

#endif /* SETTINGSSHEET_HPP_ */
