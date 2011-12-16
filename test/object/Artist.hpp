#ifndef ARTIST_HPP
#define ARTIST_HPP

#include "object/object.hpp"

#include <string>

namespace oos {
  class object_atomizer;
}

class Artist : public oos::object
{
private:
  std::string name_;

public:
  Artist();
  Artist(const std::string &n);
  virtual ~Artist();

  void read_from(oos::object_atomizer *reader);
  void write_to(oos::object_atomizer *writer) const;

  std::string name() const;
  void name(const std::string &n);

  friend std::ostream& operator <<(std::ostream &os, const Artist &a);
};

#endif /* ARTIST_HPP */
