#include "matador/utils/strptime.hpp"

#include <iostream>
#include <chrono>

template < typename ValueType >
struct implicit_value
{
  implicit_value(ValueType val) : value(val) {};
  operator ValueType() { return value; }
  ValueType value;
};

struct year
{
  unsigned short value;
};

struct month
{
  unsigned short value;
};

static const month january{1};
static const month jan = january;
static const month february{2};
static const month feb = february;
static const month march{3};
static const month mar = march;
static const month april{4};
static const month apr = april;
static const month may{5};
static const month june{6};
static const month jun = june;
static const month july{7};
static const month jul = july;
static const month august{8};
static const month aug = august;
static const month september{9};
static const month sep = september;
static const month october{10};
static const month oct = october;
static const month november{11};
static const month nov = november;
static const month december{12};
static const month dec = december;

struct month_day : public implicit_value<unsigned short> {
  using implicit_value<unsigned short>::implicit_value;
};

struct hour
{
  unsigned short value;
};

struct minute
{
  unsigned short value;
};

struct second
{
  unsigned short value;
};

struct milli_second
{
  unsigned int value;
};

year operator "" _year(unsigned long long y)
{
  return { static_cast<unsigned short>(y) };
}

month operator "" _month(unsigned long long m)
{
  return { static_cast<unsigned short>(m) };
}

month_day operator "" _day(unsigned long long d)
{
  return { static_cast<unsigned short>(d) };
}

hour operator "" _h(unsigned long long h)
{
  return { static_cast<unsigned short>(h) };
}

minute operator "" _m(unsigned long long m)
{
  return { static_cast<unsigned short>(m) };
}

second operator "" _s(unsigned long long s)
{
  return { static_cast<unsigned short>(s) };
}

milli_second operator "" _ms(unsigned long long ms)
{
  return { static_cast<unsigned int>(ms) };
}

class timestamp
{
private:
  enum class t_tz {
    UTC, LOCAL
  };

public:
  timestamp();
  timestamp(year y, month, month_day, hour hours, minute minutes, second seconds, milli_second milli_seconds);
  ~timestamp() = default;

  bool is_local() const { return tz_ == t_tz::LOCAL; }
  bool is_utc() const { return tz_ == t_tz::UTC; }

  timestamp to_utc() const;

  std::string str(const char* format = "%Y-%m-%dT%H:%M:%S") const;
  static timestamp parse(const std::string &time_string, const char* format = "%Y-%m-%dT%H:%M:%S");

  std::tm tm() const { return tm_; }

  friend std::ostream &operator<<(std::ostream &out, const timestamp &x) {
    out << x.str();
    return out;
  }

private:
  timestamp(time_t secs, unsigned int millis, t_tz tz);
  timestamp(const std::tm &tm, unsigned int millis, t_tz tz);

private:
  t_tz tz_{t_tz::LOCAL};
  std::tm tm_{};
  unsigned int milliseconds_{};
};

namespace os {

std::tm localtime(time_t *t)
{
  std::tm res{};
  auto err = localtime_s(&res, t);
  if (err) {
    throw std::logic_error("invalid argument to localtime");
  }
  return res;
}

std::tm gmtime(time_t *t)
{
  std::tm res{};
  auto err = gmtime_s(&res, t);
  if (err) {
    throw std::logic_error("invalid argument to localtime");
  }
  return res;
}

}

timestamp::timestamp()
: tz_(t_tz::LOCAL)
{
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  auto seconds_since_epoch = millis / 1000;
  tm_ = os::localtime(&seconds_since_epoch);
  milliseconds_ = (millis%1000)*1000;
}

timestamp::timestamp(year y, month m, month_day md, hour hours, minute minutes, second seconds, milli_second milli_seconds)
: tz_(t_tz::LOCAL)
, milliseconds_(milli_seconds.value)
{
  tm_.tm_year = y.value - 1900;
  tm_.tm_mon = m.value - 1;
  tm_.tm_mday = md;
  tm_.tm_hour = hours.value;
  tm_.tm_min = minutes.value;
  tm_.tm_sec = seconds.value;
  tm_.tm_isdst = -1;

  auto t = std::mktime(&tm_);
  tm_ = os::localtime(&t);
}

timestamp::timestamp(time_t secs, unsigned int millis, timestamp::t_tz tz)
  : tz_(tz)
  , milliseconds_(millis)
{
  if (tz_ == t_tz::LOCAL) {
    tm_ = os::localtime(&secs);
  } else if (tz_ == t_tz::UTC) {
    tm_ = os::gmtime(&secs);
  }
}

timestamp::timestamp(const std::tm &tm, unsigned int millis, timestamp::t_tz tz)
: tz_(tz)
, tm_(tm)
, milliseconds_(millis)
{}

timestamp timestamp::to_utc() const
{
  if (tz_ == t_tz::UTC) {
    return *this;
  }
  return {mktime(const_cast<std::tm*>(&tm_)), milliseconds_, t_tz::UTC};
}

std::string timestamp::str(const char *format) const {
  char buffer[255];
  if (strftime(buffer, 255, format, &tm_) == 0) {
    throw std::logic_error("couldn't format time string");
  }
  return buffer;
}

timestamp timestamp::parse(const std::string &time_string, const char *format)
{
  struct tm tm{};
  memset(&tm, 0, sizeof(struct tm));
  const char *endptr = matador::detail::strptime(time_string.c_str(), format, &tm);

  return {tm, 0, t_tz::LOCAL};;
}

int main() {

  timestamp ts(2023_year, august, 15_day, 12_h, 48_m, 17_s, 123_ms);

  std::cout << "current: " << ts << "\n";
  std::cout << "local? : " << std::boolalpha << ts.is_local() << "\n";
  std::cout << "utc?   : " << std::boolalpha << ts.is_utc() << "\n";
  std::cout << "utc    : " << ts.to_utc() << "\n";

  std::cout << "\n";
  timestamp ts2(2023_year, february, 15_day, 12_h, 48_m, 17_s, 123_ms);

  std::cout << "current: " << ts2 << "\n";
  std::cout << "local? : " << std::boolalpha << ts2.is_local() << "\n";
  std::cout << "utc?   : " << std::boolalpha << ts2.is_utc() << "\n";
  std::cout << "utc    : " << ts2.to_utc() << "\n";

  std::cout << "\n";
  auto tsutc = ts2.to_utc();
  std::cout << "current: " << tsutc << "\n";
  std::cout << "local? : " << std::boolalpha << tsutc.is_local() << "\n";
  std::cout << "utc?   : " << std::boolalpha << tsutc.is_utc() << "\n";
  std::cout << "utc    : " << tsutc.to_utc() << "\n";

  std::cout << "\n";
  auto pt = timestamp::parse("2023-08-15T12:48:17");
  std::cout << "current: " << pt << "\n";
  std::cout << "local? : " << std::boolalpha << pt.is_local() << "\n";
  std::cout << "utc?   : " << std::boolalpha << pt.is_utc() << "\n";
  std::cout << "utc    : " << pt.to_utc() << "\n";
};