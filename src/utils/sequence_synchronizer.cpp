#include "matador/utils/sequence_synchronizer.hpp"

namespace matador {
void sequence_synchronizer::serialize(const char *, char &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, short &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, int &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, long &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, long long int &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, unsigned char &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, unsigned short &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, unsigned int &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, unsigned long &x)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(const char *, unsigned long long int &x)
{
  sync_integral(x);
}

}
