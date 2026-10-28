#ifndef SCHEMA_NODE_ITERATOR_HPP
#define SCHEMA_NODE_ITERATOR_HPP

#include <iterator>

namespace matador::query {

class schema_node;

class schema_node_iterator {
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = schema_node;
  using pointer = schema_node*;
  using reference = value_type&;

  /**
   * Creates an empty iterator
   */
  schema_node_iterator() = default;

  /**
   * @brief Creates an iterator for a concrete type.
   *
   * This constructor creates an iterator for a concrete
   * type and a concrete object.
   *
   * @param node The schema node of the object
   */
  explicit schema_node_iterator(pointer node);

  /**
   * Copy from a given const_object_view_iterator.
   *
   * @param x The prototype_iterator to copy from.
   */
  schema_node_iterator(const schema_node_iterator &x) = default;

  /**
   * Assign from a given prototype_iterator.
   *
   * @param x The prototype_iterator to assign from.
   * @return The assigned prototype_iterator.
   */
  schema_node_iterator& operator=(const schema_node_iterator &x) = default;

  ~schema_node_iterator() = default;

  /**
   * @brief Compares this with other iterators.
   *
   * Compares this with other iterators. Returns true
   * if the iterators node prototype_type are the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are the same.
   */
  bool operator==(const schema_node_iterator &i) const;

  /**
   * @brief Compares this with other iterators.
   *
   * Compares this with other iterators. Returns true
   * if the iterators node prototype_node are different.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are different.
   */
  bool operator!=(const schema_node_iterator &i) const;

  /**
   * Pre-increments the iterator
   *
   * @return Returns iterators successor.
   */
  schema_node_iterator& operator++();

  /**
   * Post increments the iterator
   *
   * @return Returns iterator before incrementing.
   */
  schema_node_iterator operator++( int );

  /**
   * Pre-increments the iterator
   *
   * @return Returns iterators predecessor.
   */
  schema_node_iterator& operator--();

  /**
   * Post decrements the iterator
   *
   * @return Returns iterator before decrementing.
   */
  schema_node_iterator operator--(int);

  /**
   * Returns the pointer to the node.
   *
   * @return The pointer to the node.
   */
  pointer operator->() const;

  /**
   * Returns the node.
   *
   * @return The iterators underlying node.
   */
  reference operator*() const;

  /**
   * Returns the pointer to the node.
   *
   * @return The pointer to the node.
   */
  [[nodiscard]] pointer get() const;

private:
  void increment();
  void decrement();

private:
  pointer node_{nullptr};
};

class const_schema_node_iterator {
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = schema_node;
  using pointer = const schema_node*;
  using reference = const value_type&;

  /**
   * Creates an empty iterator
   */
  const_schema_node_iterator() = default;

  /**
   * @brief Creates an iterator for a concrete type.
   *
   * This constructor creates an iterator for a concrete
   * type and a concrete object.
   *
   * @param node The schema node of the object
   */
  explicit const_schema_node_iterator(pointer node);

  /**
   * Copy from a given const_object_view_iterator.
   *
   * @param x The prototype_iterator to copy from.
   */
  const_schema_node_iterator(const const_schema_node_iterator &x) = default;

  /**
   * Assign from a given prototype_iterator.
   *
   * @param x The prototype_iterator to assign from.
   * @return The assigned prototype_iterator.
   */
  const_schema_node_iterator& operator=(const const_schema_node_iterator &x) = default;

  ~const_schema_node_iterator() = default;

  /**
   * @brief Compares this with other iterators.
   *
   * Compares this with other iterators. Returns true
   * if the iterators node prototype_type are the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are the same.
   */
  bool operator==(const const_schema_node_iterator &i) const;

  /**
   * @brief Compares this with other iterators.
   *
   * Compares this with other iterators. Returns true
   * if the iterators node prototype_node are different.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are different.
   */
  bool operator!=(const const_schema_node_iterator &i) const;

  /**
   * Pre-increments the iterator
   *
   * @return Returns iterators successor.
   */
  const_schema_node_iterator& operator++();

  /**
   * Post increments the iterator
   *
   * @return Returns iterator before incrementing.
   */
  const_schema_node_iterator operator++( int );

  /**
   * Pre-increments the iterator
   *
   * @return Returns iterators predecessor.
   */
  const_schema_node_iterator& operator--();

  /**
   * Post decrements the iterator
   *
   * @return Returns iterator before decrementing.
   */
  const_schema_node_iterator operator--(int);

  /**
   * Returns the pointer to the node.
   *
   * @return The pointer to the node.
   */
  pointer operator->() const;

  /**
   * Returns the node.
   *
   * @return The iterators underlying node.
   */
  reference operator*() const;

  /**
   * Returns the pointer to the node.
   *
   * @return The pointer to the node.
   */
  [[nodiscard]] pointer get() const;

private:
  void increment();
  void decrement();

private:
  pointer node_{nullptr};
};
}
#endif //SCHEMA_NODE_ITERATOR_HPP
