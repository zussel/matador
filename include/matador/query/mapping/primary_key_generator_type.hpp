#ifndef PRIMARY_KEY_GENERATOR_TYPE_HPP
#define PRIMARY_KEY_GENERATOR_TYPE_HPP

namespace matador::query {
enum class generator_type {
  None = 0,   /**< No generator type set. */
  Manual,     /**< User sets the primary key value manually. */
  Auto,       /**< Matador chooses the best generator type depending on the underlying dbms. */
  Identity,   /**< DBMS automatically generates the primary key value. */
  Sequence,   /**< DBMS automatically generates the primary key value by using a sequence table. */
  Table       /**< DBMS automatically generates the primary key value by using a table. */
};
}
#endif //PRIMARY_KEY_GENERATOR_TYPE_HPP
