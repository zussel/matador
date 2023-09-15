#include "matador/utils/sequence_synchronizer.hpp"

namespace matador {

void sequence_synchronizer::serialize(short &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(int &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(long &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(long long int &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned short &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned int &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned long &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

void sequence_synchronizer::serialize(unsigned long long int &x, const field_attributes &/*attr*/)
{
  sync_integral(x);
}

}
