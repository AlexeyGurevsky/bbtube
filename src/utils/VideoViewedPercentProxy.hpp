#ifndef VideoViewedPercentProxy_HPP_
#define VideoViewedPercentProxy_HPP_

#include <QMap>

class VideoViewedPercentProxy : public QObject
{
    Q_OBJECT
private:
    QMap<QString, int> map;
    static VideoViewedPercentProxy* instance;
    VideoViewedPercentProxy()
    {
    }
public:
    static VideoViewedPercentProxy* getInstance();
    void setViewedPercent(QString videoId, int value);
    void deleteById(QString videoId);
    int getPercent(QString videoId);
    void deleteAll();
signals:
    void updated(QString videoId, int percent);
    void deletedAll();
};

#endif /* VideoViewedPercentProxy_HPP_ */
