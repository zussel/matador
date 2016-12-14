#ifndef CALENDAR_H
#define CALENDAR_H

int gc2jd(int day, int month, int year);
void jd2gc(int jday, int &day, int &month, int &year);

int time2seconds(unsigned int hour, unsigned int minute, unsigned int second);
void seconds2time(unsigned int seconds, unsigned int &hour, unsigned int &minute, unsigned int &second);

#endif /* CALENDAR_H */
