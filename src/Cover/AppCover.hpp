#ifndef APPCOVER_HPP_
#define APPCOVER_HPP_

#include "src/utils/GlobalPlayerContext.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/SceneCover>

using namespace bb::cascades;

class AppCover: public SceneCover
{
    Q_OBJECT
private:
    ImageView *thumbnail;
    Container *titleContainer;
    Label *title;

    QString videoId;
    GlobalPlayerContext *playerContext;
    QByteArray thumbnailData;

    void showDefaultImage();
private slots:
    void onMetadataChanged();
    void onMiniPlayerStopped();
public:
    AppCover();
    virtual ~AppCover()
    {
    }
};

#endif /* APPCOVER_HPP_ */
