#ifndef ARTIST_HPP
#define ARTIST_HPP

#include "object/object.hpp"

#include <string>

class Artist : public object
{
public:
	Artist();
  Artist(const std::string &n);
	virtual ~Artist();
	
	void read_from(object_atomizer *reader);
	void write_to(object_atomizer *writer);

  std::string name() const;
  void name(const std::string &n);

private:
	std::string name_;
};

#endif /* ARTIST_HPP */
