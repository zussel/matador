#include "matador/utils/sequence_synchronizer.hpp"

namespace matador {

void sequence_synchronizer::serialize(short &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(int &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(long &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(long long int &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned short &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned int &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned long &x, long /*size*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned long long int &x, long /*size*/)
{
  sync_integral(x);
}

}
