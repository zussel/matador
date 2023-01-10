#ifndef OOS_OBJECT_HOLDER_TYPE_HPP
#define OOS_OBJECT_HOLDER_TYPE_HPP

namespace matador {

/**
 * Represents the type of an object_holder. This is
 * used as a template parameter in the derived object_pointer
 * class.
 */
enum class object_holder_type {
  OBJECT_PTR, /**< Represents object_ptr type */
  HAS_ONE,    /**< Represents hs_one type */
  BELONGS_TO  /**< Represents belongs_to type */
};

}
#endif //OOS_OBJECT_HOLDER_TYPE_HPP
