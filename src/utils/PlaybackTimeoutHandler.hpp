#ifndef PLAYBACKTIMEOUTHANDLER_HPP_
#define PLAYBACKTIMEOUTHANDLER_HPP_

#include "GlobalPlayerContext.hpp"
#include "src/applicationui.hpp"
#include "src/settings/AppSettings.hpp"

#include <bb/cascades/Application>
#include <bb/multimedia/MediaState>
using namespace bb::cascades;

class PlaybackTimeoutHandler: public QObject
{
Q_OBJECT
public:
    PlaybackTimeoutHandler(QObject *parent) :
            QObject(parent)
    {
        settings = ApplicationUI::appSettings;
        playerContext = ApplicationUI::playerContext;

        timer = new QTimer(parent);
        timer->setSingleShot(true);

        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

        QObject::connect(Application::instance(), SIGNAL(thumbnail()), this, SLOT(onThumbnailed()));
        QObject::connect(Application::instance(), SIGNAL(fullscreen()), this, SLOT(onFullsreen()));
        QObject::connect(Application::instance(), SIGNAL(invisible()), this, SLOT(onInvisible()));
    }
    virtual ~PlaybackTimeoutHandler()
    {
    }
private slots:
    void onThumbnailed()
    {
        if (!timer->isActive() && settings->getPlaybackTimeout() > 0) {
            timer->start(settings->getPlaybackTimeout() * 1000 * 60);
        }
    }
    void onFullsreen()
    {
        if (timer->isActive()) {
            timer->stop();
        }
    }
    void onInvisible()
    {
        if (!timer->isActive() && settings->getPlaybackTimeout() > 0) {
            timer->start(settings->getPlaybackTimeout() * 1000 * 60);
        }
    }
    void onTimeout()
    {
        if (playerContext->getMediaState() == bb::multimedia::MediaState::Started) {
            playerContext->pause();
        }
    }
private:
    QTimer *timer;
    AppSettings *settings;
    GlobalPlayerContext *playerContext;
};

#endif /* PLAYBACKTIMEOUTHANDLER_HPP_ */
