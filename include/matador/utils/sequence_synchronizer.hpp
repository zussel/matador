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

  void serialize(const char *, short &x) override;
  void serialize(const char *, int &x) override;
  void serialize(const char *, long &x) override;
  void serialize(const char *, long long &x) override;
  void serialize(const char *, unsigned short &x) override;
  void serialize(const char *, unsigned int &x) override;
  void serialize(const char *, unsigned long &x) override;
  void serialize(const char *, unsigned long long &x) override;
  void serialize(const char *, std::string &) override {}
  void serialize(const char *, null_type_t &) override {}

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
