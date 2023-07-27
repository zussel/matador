#ifndef MATADOR_SEQUENCE_SYNCHRONIZER_HPP
#define MATADOR_SEQUENCE_SYNCHRONIZER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/utils/sequencer.hpp"

namespace matador {

/// @cond MATADOR_DEV

class OOS_UTILS_API sequence_synchronizer : public identifier_serializer
{
public:
  explicit sequence_synchronizer(sequencer &seq)
    : seq_(seq)
  {}

  void sync(identifier &bi)
  {
    bi.serialize(*this);
  }

  void serialize(short &x, long size) override;
  void serialize(int &x, long size) override;
  void serialize(long &x, long size) override;
  void serialize(long long &x, long size) override;
  void serialize(unsigned short &x, long size) override;
  void serialize(unsigned int &x, long size) override;
  void serialize(unsigned long &x, long size) override;
  void serialize(unsigned long long &x, long size) override;
  void serialize(std::string &, long /*size*/) override {}
  void serialize(null_type_t &, long /*size*/) override {}

private:
  template<class T>
  void sync_integral(const T &val)
  {
    if (val > static_cast<T>(seq_.current())) {
      seq_.update(val);
    }
  }

private:
  sequencer &seq_;

};

/// @endcond

}

#endif //MATADOR_SEQUENCE_SYNCHRONIZER_HPP
