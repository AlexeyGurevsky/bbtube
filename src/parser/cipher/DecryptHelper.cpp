#include "DecryptHelper.hpp"

#include <QStringList>
#include <QUrl>

QString DecryptHelper::decryptUrl(QString encryptedUrl, const ScriptData *scriptData)
{
    QStringList split = encryptedUrl.split('&');
    QString s;
    QString sp;
    QString url;
    for (int j = 0; j < split.length(); j++) {
        if (split[j].startsWith("s=")) {
            s = QUrl::fromPercentEncoding(split[j].mid(2, split[j].length() - 2).toUtf8());
        } else if (split[j].startsWith("sp=")) {
            sp = QUrl::fromPercentEncoding(split[j].mid(3, split[j].length() - 3).toUtf8());
        } else if (split[j].startsWith("url=")) {
            url = QUrl::fromPercentEncoding(split[j].mid(4, split[j].length() - 4).toUtf8());
        }
    }

    QUrl tempUrl = QUrl(url);
    QList<QPair<QString, QString> > queryItems = tempUrl.queryItems();
    QByteArray sig = s.toUtf8();

    for (int i = 0; i < scriptData->cipherOperations.size(); i++) {
        sig = scriptData->cipherOperations.at(i)->process(sig);
    }

    queryItems.append(QPair<QString, QString>(sp, QString(sig)));
    tempUrl.setQueryItems(queryItems);

    return tempUrl.toString();
}

