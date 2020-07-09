#ifndef DATETIMEUTILS_HPP_
#define DATETIMEUTILS_HPP_

class DateTimeUtils
{
public:
    static QString timeAgo(int delta)
    {
        const int SECOND = 1;
        const int MINUTE = 60 * SECOND;
        const int HOUR = 60 * MINUTE;
        const int DAY = 24 * HOUR;
        const int WEEK = 7 * DAY;
        const int MONTH = 30 * DAY;
        const int YEAR = 365 * DAY;

        int minutes = delta / MINUTE;
        int hours = delta / HOUR;
        int days = delta / DAY;
        int weeks = delta / WEEK;
        int months = delta / MONTH;
        int years = delta / YEAR;

        if (delta < 1 * MINUTE)
            return delta == 1 ? "1 second ago" : QString::number(delta) + " seconds ago";

        if (delta < 2 * MINUTE)
            return "a minute ago";

        if (delta < 45 * MINUTE)
            return QString::number(minutes) + " minutes ago";

        if (delta < 180 * MINUTE)
            return "1 hour ago";

        if (delta < 24 * HOUR)
            return QString::number(hours) + " hours ago";

        if (delta < 7 * DAY)
            return days <= 1 ? "1 day ago" : QString::number(days) + " days ago";

        if (delta < 30 * DAY)
            return weeks <= 1 ? "1 week ago" : QString::number(weeks) + " weeks ago";

        if (delta < 12 * MONTH) {
            return months <= 1 ? "1 month ago" : QString::number(months) + " months ago";
        }

        return years <= 1 ? "1 year ago" : QString::number(years) + " years ago";
    }
};

#endif /* DATETIMEUTILS_HPP_ */
