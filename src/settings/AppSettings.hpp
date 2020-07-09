#ifndef APPSETTINGS_HPP_
#define APPSETTINGS_HPP_

#include "src/db/DbHelper.hpp"

class AppSettings: public QObject
{
Q_OBJECT
public:
    AppSettings(QObject *parent) :
            QObject(parent)
    {
        autoplay = DbHelper::isAutoplay();
        playbackTimeout = DbHelper::getPlaybackTimeout();
        tab = DbHelper::defaultTab();
        quality = DbHelper::defaultQuality();
    }
    ~AppSettings()
    {
    }
    bool isAutoplay()
    {
        return autoplay;
    }
    void setAutoplay(bool value)
    {
        if (autoplay != value) {
            autoplay = value;
            DbHelper::setAutoplay(value);
            emit autoplayChanged(value);
        }
    }
    int getPlaybackTimeout()
    {
        return playbackTimeout;
    }
    void setPlaybackTimeout(int value)
    {
        if (playbackTimeout != value) {
            playbackTimeout = value;
            DbHelper::setPlaybackTimeout(value);
            emit playbackTimeoutChanged(value);
        }
    }
    QString defaultTab()
    {
        return tab;
    }
    void setDefaultTab(QString value)
    {
        if (tab != value) {
            tab = value;
            DbHelper::setDefautTab(value);
        }
    }
    QString defaultQuality()
    {
        return quality;
    }
    void setDefaultQuality(QString value)
    {
        if (quality != value) {
            quality = value;
            DbHelper::setDefautQuality(value);
        }
    }
signals:
    void autoplayChanged(bool value);
    void playbackTimeoutChanged(int value);
private:
    bool autoplay;
    int playbackTimeout;
    QString tab;
    QString quality;
};

#endif /* APPSETTINGS_HPP_ */
