#ifndef MATADOR_SEQUENCE_SYNCHRONIZER_HPP
#define MATADOR_SEQUENCE_SYNCHRONIZER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/sequencer.hpp"

namespace matador {

class identifiable_holder;
class abstract_has_many;

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

  void serialize(const char *id, char &x) override;
  void serialize(const char *id, short &x) override;
  void serialize(const char *id, int &x) override;
  void serialize(const char *id, long &x) override;
  void serialize(const char *id, long long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
  void serialize(const char *id, unsigned long long &x) override;
  void serialize(const char *, float &) override {}
  void serialize(const char *, double &) override {}
  void serialize(const char *, bool &) override {}
  void serialize(const char *, char *, size_t) override {}
  void serialize(const char *, std::string &, size_t ) override {}
  void serialize(const char *, std::string &) override {}
  void serialize(const char *, date &) override {}
  void serialize(const char *, time &) override {}
  void serialize(const char *, identifiable_holder &, cascade_type) override {}
  void serialize(const char *, basic_identifier &) override {}
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

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

}

#endif //MATADOR_SEQUENCE_SYNCHRONIZER_HPP
