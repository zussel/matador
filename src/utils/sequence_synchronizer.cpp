#include "matador/utils/sequence_synchronizer.hpp"

namespace matador {
void sequence_synchronizer::on_attribute(const char *, char &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, short &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, int &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, long &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, long long int &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, unsigned char &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, unsigned short &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, unsigned int &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, unsigned long &x)
{
  sync_integral(x);
}

void sequence_synchronizer::on_attribute(const char *, unsigned long long int &x)
{
  sync_integral(x);
}

}
