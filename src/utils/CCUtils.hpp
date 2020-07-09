#ifndef CCUTILS_HPP_
#define CCUTILS_HPP_

class CCUtils
{
public:
    static QString fix(QString input)
    {
        int pos = 0;
        QString styleKey = "<style xml:id=\"";
        QStringList styles;

        while ((pos = input.indexOf(styleKey, pos)) >= 0) {
            int tagStart = pos;
            int tagEnd = input.indexOf(">", pos) + 1;
            int idStart = pos + styleKey.length();
            QString styleId = input.mid(idStart, input.indexOf("\"", idStart) - idStart);

            if (styleId != "s1" && styleId != "s2" && !styles.contains(styleId)) {
                styles.append(styleId);
                input.remove(tagStart, tagEnd - tagStart);
            } else {
                pos = tagEnd;
            }
        }

        pos = 0;
        QString regionKey = "<region xml:id=\"";
        QStringList regions;

        while ((pos = input.indexOf(regionKey, pos)) >= 0) {
            int tagStart = pos;
            int tagEnd = input.indexOf(">", pos) + 1;
            int idStart = pos + regionKey.length();
            QString regionId = input.mid(idStart, input.indexOf("\"", idStart) - idStart);

            if (regionId != "r1" && !regions.contains(regionId)) {
                regions.append(regionId);
                input.remove(tagStart, tagEnd - tagStart);
            } else {
                pos = tagEnd;
            }
        }

        for (int i = 0; i < styles.count(); i++) {
            input.replace(QString("style=\"%1\"").arg(styles[i]), "style=\"s2\"");
        }
        for (int i = 0; i < regions.count(); i++) {
            input.replace(QString("region=\"%1\"").arg(regions[i]), "");
        }

        return input;
    }
};

#endif /* CCUTILS_HPP_ */
