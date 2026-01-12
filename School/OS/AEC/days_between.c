#include <stdio.h>
#define ABS(x) ((x) < 0 ? -(x) : (x))
struct date
{
    int year;
    int month;
    int day;
};
int days_serial(const struct date *d)
{
    int y = d->year;
    int m = d->month;
    int day = d->day;
    if (m < 3)
    {
        y--;
        m += 12;
    }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + day - 1;
}
int days_between(const struct date *day1, const struct date *day2)
{
    return ABS(days_serial(day2) - days_serial(day1));
}