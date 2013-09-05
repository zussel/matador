#ifndef ELEMENT_TYPES_HPP
#define ELEMENT_TYPES_HPP

namespace oos {

typedef enum {
  unknown_type    = 0,
  file_type       = 1,
  link_type       = 2,
  directory_type  = 4,
  all_types = file_type | link_type | directory_type
} element_type;

}

#endif /* ELEMENT_TYPES_HPP */
