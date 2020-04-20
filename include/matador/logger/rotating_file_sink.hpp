//
// Created by sascha on 17.04.20.
//

#ifndef MATADOR_ROTATING_FILE_SINK_HPP
#define MATADOR_ROTATING_FILE_SINK_HPP

#include "matador/logger/file_sink.hpp"
#include "matador/utils/file.hpp"

#include <vector>

namespace matador {

class rotating_file_sink : public basic_file_sink
{
public:
  rotating_file_sink(const std::string& path, size_t max_size, size_t file_count);

  void write(const char *message, size_t size) override;

private:
  std::string calculate_filename(size_t fileno);

  void rotate();
  void split(const std::string &path);

private:
  file logfile_;
  std::string base_path_;
  std::string extension_;
  size_t max_size_ = 0;
  size_t current_size_ = 0;
  size_t current_file_no_ = 0;
  size_t file_count_ = 0;
};

}
#endif //MATADOR_ROTATING_FILE_SINK_HPP
