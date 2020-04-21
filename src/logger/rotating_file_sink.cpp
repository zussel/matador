#include "matador/logger/rotating_file_sink.hpp"

#include "matador/utils/string.hpp"
#include "matador/utils/os.hpp"

#include <utility>
#include <iostream>

namespace matador {

rotating_file_sink::rotating_file_sink(const std::string& path, size_t max_size, size_t file_count)
  : logfile_(path, "a")
  , max_size_(max_size)
  , file_count_(file_count)
{
  split(logfile_.path());

  //rotate();

  // generate filename {name}.{n}.[extension}
  std::string filename = calculate_filename(current_file_no_);

}

void rotating_file_sink::write(const char *message, size_t size)
{
  current_size_ += size;
  if (current_size_ > max_size_) {
    current_size_ = max_size_;
    rotate();
    // close current log file
    // rename current logfile
    // open current logfile truncated
  }
  basic_file_sink::write(message, size);
}

std::string rotating_file_sink::calculate_filename(size_t fileno)
{
  char buffer[1024];
  sprintf(buffer, "%s.%zu.%s", base_path_.c_str(), fileno, extension_.c_str());
  return buffer;
}

/*
 * Rotate log files:
 * log_path.log  -> log_path.01.log
 * log_path.01.log  -> log_path.02.log
 * log_path.02.log  -> log_path.03.log
 * ...
 * log_path.[n-1].log  -> log_path.[n].log
 * delete log_path.[n].log
 * create new log_path.log
 */
void rotating_file_sink::rotate()
{
  for (size_t i = file_count_; i != 0; --i) {
    if (i == file_count_) {
      matador::os::remove(calculate_filename(i));
      std::cout << "deleting " << calculate_filename(i) << "\n";
    } else {
      std::cout << "moving " << calculate_filename(i) << " to " << calculate_filename(i+1) + "\n";
    }
  }
  std::cout << "moving " << logfile_.path() << " to " << calculate_filename(1) << "\n";
}

void rotating_file_sink::split(const std::string &path)
{
  std::vector<std::string> result;
  if (matador::split(path, '.', result) != 2) {
    throw std::logic_error("splitted path must consists of two elements");
  }
  base_path_ = result.at(0);
  extension_ = result.at(1);
}
}
