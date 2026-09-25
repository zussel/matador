#ifndef TEST_RESULT_READER_HPP
#define TEST_RESULT_READER_HPP

#include "matador/query/interface/query_result_reader.hpp"

namespace matador::test::orm {

namespace detail {

class empty_binder final : public query::value_reader {
public:
    void read_value(const char *, size_t, int8_t &) override {}
    void read_value(const char *, size_t, int16_t &) override {}
    void read_value(const char *, size_t, int32_t &) override {}
    void read_value(const char *, size_t, int64_t &) override {}
    void read_value(const char *, size_t, uint8_t &) override {}
    void read_value(const char *, size_t, uint16_t &) override {}
    void read_value(const char *, size_t, uint32_t &) override {}
    void read_value(const char *, size_t, uint64_t &) override {}
    void read_value(const char *, size_t, bool &) override {}
    void read_value(const char *, size_t, float &) override {}
    void read_value(const char *, size_t, double &) override {}
    void read_value(const char *, size_t, utils::date_type_t &) override {}
    void read_value(const char *, size_t, utils::time_type_t &) override {}
    void read_value(const char *, size_t, utils::timestamp_type_t &) override {}
    void read_value(const char *, size_t, char *, size_t) override {}
    void read_value(const char *, size_t, std::string &) override {}
    void read_value(const char *, size_t, std::string &, size_t) override {}
    void read_value(const char *, size_t, utils::blob_type_t &) override {}
    void read_value(const char *, size_t, query::column_value &, size_t) override {}
};

}

class test_result_reader final : public query::query_result_reader {
public:
  [[nodiscard]] size_t column_count() const override;
  [[nodiscard]] const char *column(size_t index) const override;
  [[nodiscard]] utils::result<bool, utils::error> fetch() override;
  [[nodiscard]] size_t start_column_index() const override;
  void unshift() override;

  void read_value(const char *id, size_t index, int8_t &value) override;
  void read_value(const char *id, size_t index, int16_t &value) override;
  void read_value(const char *id, size_t index, int32_t &value) override;
  void read_value(const char *id, size_t index, int64_t &value) override;
  void read_value(const char *id, size_t index, uint8_t &value) override;
  void read_value(const char *id, size_t index, uint16_t &value) override;
  void read_value(const char *id, size_t index, uint32_t &value) override;
  void read_value(const char *id, size_t index, uint64_t &value) override;
  void read_value(const char *id, size_t index, bool &value) override;
  void read_value(const char *id, size_t index, float &value) override;
  void read_value(const char *id, size_t index, double &value) override;
  void read_value(const char *id, size_t index, utils::date_type_t &value) override;
  void read_value(const char *id, size_t index, utils::time_type_t &value) override;
  void read_value(const char *id, size_t index, utils::timestamp_type_t &value) override;
  void read_value(const char *id, size_t index, char *value, size_t size) override;
  void read_value(const char *id, size_t index, std::string &value) override;
  void read_value(const char *id, size_t index, std::string &value, size_t size) override;
  void read_value(const char *id, size_t index, utils::blob_type_t &value) override;
  void read_value(const char *id, size_t index, query::column_value &val, size_t size) override;

protected:
  value_reader &result_binder() override;

private:
  uint8_t rows_{5};
  detail::empty_binder empty_binder_;
};

}

#endif //TEST_RESULT_READER_HPP
