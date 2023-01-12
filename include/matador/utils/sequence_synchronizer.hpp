#ifndef MATADOR_SEQUENCE_SYNCHRONIZER_HPP
#define MATADOR_SEQUENCE_SYNCHRONIZER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/sequencer.hpp"

namespace matador {

class identifiable_holder;
class abstract_has_many;

/// @cond MATADOR_DEV

class OOS_UTILS_API sequence_synchronizer : public serializer
{
public:
  explicit sequence_synchronizer(sequencer &seq)
    : seq_(seq)
  {}

  void sync(basic_identifier &bi)
  {
    bi.serialize(nullptr, *this);
  }

  void on_attribute(const char *id, char &x) override;
  void on_attribute(const char *id, short &x) override;
  void on_attribute(const char *id, int &x) override;
  void on_attribute(const char *id, long &x) override;
  void on_attribute(const char *id, long long &x) override;
  void on_attribute(const char *id, unsigned char &x) override;
  void on_attribute(const char *id, unsigned short &x) override;
  void on_attribute(const char *id, unsigned int &x) override;
  void on_attribute(const char *id, unsigned long &x) override;
  void on_attribute(const char *id, unsigned long long &x) override;
  void on_attribute(const char *, float &) override {}
  void on_attribute(const char *, double &) override {}
  void on_attribute(const char *, bool &) override {}
  void on_attribute(const char *, char *, size_t) override {}
  void on_attribute(const char *, std::string &, size_t ) override {}
  void on_attribute(const char *, std::string &) override {}
  void on_attribute(const char *, date &) override {}
  void on_attribute(const char *, time &) override {}
  void on_belongs_to(const char *, identifiable_holder &, cascade_type) override {}
  void on_has_one(const char *, identifiable_holder &, cascade_type) override {}
  void on_primary_key(const char *, basic_identifier &) override {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) override {}

private:
  template<class T>
  void sync_integral(T val)
  {
    if (val > (T)seq_.current()) {
      seq_.update(val);
    }
  }

private:
  sequencer &seq_;

};

/// @endcond

}

#endif //MATADOR_SEQUENCE_SYNCHRONIZER_HPP
