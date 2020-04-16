#include "matador/logger/basic_file_sink.hpp"

namespace matador {

basic_file_sink::basic_file_sink(FILE *f)
: stream(f)
{}

void basic_file_sink::write(const char *message, size_t size)
{
  fwrite(message, sizeof(char), size, stream);
  fflush(stream);
}

void basic_file_sink::close()
{
  if (stream) {
    fclose(stream);
    stream = nullptr;
  }
}

}
