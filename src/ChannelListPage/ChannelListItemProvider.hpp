#ifndef CHANNELLISTITEMPROVIDER_HPP_
#define CHANNELLISTITEMPROVIDER_HPP_

#include "src/models/ChannelListItemModel.hpp"

#include <bb/cascades/Container>
#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/CustomListItem>
#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>
#include <bb/cascades/ActionSet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/InvokeActionItem>

using namespace bb::cascades;

class ChannelListItemProvider;
class ChannelListItem;

class ChannelListItem: public CustomListItem, public ListItemListener
{
    Q_OBJECT

public:
    ChannelListItem(ListView *listView, const ChannelListItemProvider *provider,
            Container *parent = 0);
    virtual ~ChannelListItem()
    {
    }

    void updateItem(const ChannelListItemModel *item, QVariantList indexPath);
    void select(bool select);
    void reset(bool selected, bool activated);
    void activate(bool activate);

signals:
    void removeFromChannelsActionItemClick(QVariantList indexPath);
private slots:
    void onRemoveFromChannelsActionItemClick();
    void onCopyChannelLinkActionItemClick();
    void onCopyRssLinkActionItemClick();
    void onOpenChannelInBrowserActionItemClick();
private:
    const ChannelListItemProvider *provider;
    ImageView *thumbnail;
    Label *title;
    Label *subtitle;
    ListView *listView;
    Container *thumbnailContainer;
    Container *itemContainer;
    Container *unseenContainer;
    QVariantList indexPath;
    ActionSet *actionSet;

    bb::cascades::ActionItem *removeFromChannelsActionItem;
    bb::cascades::ActionItem *copyChannelLinkActionItem;
    bb::cascades::ActionItem *copyRssLinkActionItem;
    bb::cascades::ActionItem *openChannelInBrowserActionItem;
    bb::cascades::InvokeActionItem *shareChannelActionItem;

    ChannelListItemModel* getModelByPath();
};

class ChannelListItemProvider: public ListItemProvider
{
    Q_OBJECT
public:
    QMap<QString, QByteArray> imagesCache;

    ChannelListItemProvider()
    {
    }
    virtual ~ChannelListItemProvider()
    {
    }

    virtual VisualNode * createItem(ListView* list, const QString &type)
    {
        ChannelListItem *item = new ChannelListItem(list, this);

        QObject::connect(item, SIGNAL(removeFromChannelsActionItemClick(QVariantList)), this,
                SLOT(onRemoveFromChannelsActionItemClick(QVariantList)));

        return item;
    }

    virtual void updateItem(ListView* list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
    {
        ChannelListItemModel *item = data.value<ChannelListItemModel*>();
        static_cast<ChannelListItem*>(listItem)->updateItem(item, indexPath);
    }
signals:
    void removeFromChannelsActionItemClick(QVariantList);
private slots:
    void onRemoveFromChannelsActionItemClick(QVariantList indexPath)
    {
        emit removeFromChannelsActionItemClick(indexPath);
    }
};

#endif /* CHANNELLISTITEMPROVIDER_HPP_ */
