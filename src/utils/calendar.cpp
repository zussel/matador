#include <cstdlib>

/*!
  \internal
  Converts a Gregorian date to a Julian day.
  This algorithm is taken from Communications of the ACM, Vol 6, No 8.
  \sa jul2greg()
*/

int
gc2jd(int d, int m, int y) {
  unsigned int c, ya;
  if ( y <= 99 ) {
    y += 1900;
  }
  if ( m > 2 ) {
    m -= 3;
  } else {
    m += 9;
    y--;
  }
  c = y; // NOTE: Sym C++ 6.0 bug
  c /= 100;
  ya = y - 100*c;
  return 1721119 + d + (146097*c)/4 + (1461*ya)/4 + (153*m+2)/5;
}

/*!
  \internal
  Converts a Julian day to a Gregorian date.
  This algorithm is taken from Communications of the ACM, Vol 6, No 8.
  \sa greg2jul()
*/

void
jd2gc(int jd, int &d, int &m, int &y) {
  unsigned int x;
  unsigned int j = jd - 1721119;
  y = (j*4 - 1)/146097;
  j = j*4 - 146097*y - 1;
  x = j/4;
  j = (x*4 + 3) / 1461;
  y = 100*y + j;
  x = (x*4) + 3 - 1461*j;
  x = (x + 4)/4;
  m = (5*x - 3)/153;
  x = 5*x - 3 - 153*m;
  d = (x + 5)/5;
  if ( m < 10 ) {
    m += 3;
  } else {
    m -= 9;
    y++;
  }
}

int
time2seconds(unsigned int hour, unsigned int minute, unsigned int second) {
  return hour * 3600 + 60 * minute + second;
}

void
seconds2time(unsigned int seconds, unsigned int &hour, unsigned int &minute, unsigned int &second) {
  hour = seconds / 3600;
  minute = (seconds - 3600 * hour) / 60;
  second = seconds - 3600 * hour - 60 * minute;
}
