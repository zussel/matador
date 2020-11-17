//
// Created by sascha on 11/30/15.
//

#ifndef OOS_PROTOTYPE_ITERATOR_HPP
#define OOS_PROTOTYPE_ITERATOR_HPP

#include "matador/object/prototype_node.hpp"

#include <iterator>

namespace matador {

class object_proxy;
class const_prototype_iterator;

/**
* @cond MATADOR_DEV
* @class prototype_iterator
* @brief An iterator for traversing the prototypes
*
* Defines an iterator for prototype_node. The nodes
* are stored in the object_store and must not be altered.
* therefor the iterator is declared as const.
*/
class MATADOR_OBJECT_API prototype_iterator : public std::iterator<std::bidirectional_iterator_tag, prototype_node, std::ptrdiff_t, prototype_node*, prototype_node&>
{
public:
typedef std::iterator<std::bidirectional_iterator_tag, prototype_node, std::ptrdiff_t, prototype_node*, prototype_node&> base_iterator; /**< Shortcut to the base iterator class. */
typedef prototype_iterator self;	            /**< Shortcut for this class. */
typedef base_iterator::value_type value_type; /**< Shortcut for the value type. */
typedef base_iterator::pointer pointer;       /**< Shortcut for the pointer type. */
typedef base_iterator::reference reference ;  /**< Shortcut for the reference type */

/**
* Creates an empty iterator
*/
prototype_iterator() = default;

/**
* @brief Creates a iterator for a concrete type.
*
* This constructor creates an iterator for a concrete
* type and a concrete serializable.
*
* @param node The prototype_node of the object_proxy
*/
explicit prototype_iterator(pointer node);

/**
* Copy from a given const_object_view_iterator.
*
* @param x The prototype_iterator to copy from.
*/
prototype_iterator(const prototype_iterator &x);

/**
* Assign from a given prototype_iterator.
*
* @param x The prototype_iterator to assign from.
* @return The assigned prototype_iterator.
*/
prototype_iterator& operator=(const prototype_iterator &x);

~prototype_iterator() = default;

/**
* @brief Compares this with another iterators.
*
* Compares this with another iterators. Returns true
* if the iterators node prototype_type are the same.
*
* @param i The iterator to compare with.
* @return True if the iterators are the same.
*/
bool operator==(const prototype_iterator &i) const;
bool operator==(const const_prototype_iterator &i) const;

/**
* @brief Compares this with another iterators.
*
* Compares this with another iterators. Returns true
* if the iterators node prototype_node are not the same.
*
* @param i The iterator to compare with.
* @return True if the iterators are not the same.
*/
bool operator!=(const prototype_iterator &i) const;
bool operator!=(const const_prototype_iterator &i) const;

/**
* Pre increments the iterator
*
* @return Returns iterators successor.
*/
self& operator++();

/**
* Post increments the iterator
*
* @return Returns iterator before incrementing.
*/
self operator++(int);

/**
* Pre increments the iterator
*
* @return Returns iterators predeccessor.
*/
self& operator--();

/**
* Post decrements the iterator
*
* @return Returns iterator before decrementing.
*/
self operator--(int);

/**
* Returns the pointer to the node.
*
* @return The pointer to the node.
*/
pointer operator->() const;

/**
* Returns the node.
*
* @return The iterators underlaying node.
*/
reference operator*() const;

/**
* Returns the pointer to the node.
*
* @return The pointer to the node.
*/
pointer get() const;

private:
void increment();
void decrement();

private:
friend class const_prototype_iterator;

pointer node_ = nullptr;
};

class MATADOR_OBJECT_API const_prototype_iterator
  : public std::iterator<std::bidirectional_iterator_tag, prototype_node, std::ptrdiff_t, const prototype_node *, const prototype_node &>
{
public:
typedef std::iterator<std::bidirectional_iterator_tag, prototype_node, std::ptrdiff_t, const prototype_node*, const prototype_node&> base_iterator; /**< Shortcut to the base iterator class. */
typedef const_prototype_iterator self;	            /**< Shortcut for this class. */
typedef base_iterator::value_type value_type; /**< Shortcut for the value type. */
typedef base_iterator::pointer pointer;       /**< Shortcut for the pointer type. */
typedef base_iterator::reference reference ;  /**< Shortcut for the reference type */

/**
* Creates an empty iterator
*/
const_prototype_iterator();

/**
* @brief Creates a iterator for a concrete type.
*
* This constructor creates an iterator for a concrete
* type and a concrete serializable.
*
* @param node The prototype_node of the object_proxy
*/
explicit const_prototype_iterator(pointer node);

/**
* Copy from a given const_object_view_iterator.
*
* @param x The prototype_iterator to copy from.
*/
const_prototype_iterator(const const_prototype_iterator &x);

/**
* Copy from a given const_object_view_iterator.
*
* @param x The prototype_iterator to copy from.
*/
const_prototype_iterator(const prototype_iterator &x);

/**
* Assign from a given prototype_iterator.
*
* @param x The prototype_iterator to assign from.
* @return The assigned prototype_iterator.
*/
const_prototype_iterator& operator=(const prototype_iterator &x);

/**
* Assign from a given prototype_iterator.
*
* @param x The prototype_iterator to assign from.
* @return The assigned prototype_iterator.
*/
const_prototype_iterator& operator=(const const_prototype_iterator &x);

~const_prototype_iterator() = default;

/**
* @brief Compares this with another iterators.
*
* Compares this with another iterators. Returns true
* if the iterators node prototype_type are the same.
*
* @param i The iterator to compare with.
* @return True if the iterators are the same.
*/
bool operator==(const const_prototype_iterator &i) const;

/**
* @brief Compares this with another iterators.
*
* Compares this with another iterators. Returns true
* if the iterators node prototype_node are not the same.
*
* @param i The iterator to compare with.
* @return True if the iterators are not the same.
*/
bool operator!=(const const_prototype_iterator &i) const;

/**
* Pre increments the iterator
*
* @return Returns iterators successor.
*/
self& operator++();

/**
* Post increments the iterator
*
* @return Returns iterator before incrementing.
*/
self operator++(int);

/**
* Pre increments the iterator
*
* @return Returns iterators predeccessor.
*/
self& operator--();

/**
* Post decrements the iterator
*
* @return Returns iterator before decrementing.
*/
self operator--(int);

/**
* Returns the pointer to the node.
*
* @return The pointer to the node.
*/
pointer operator->() const;

/**
* Returns the node.
*
* @return The iterators underlaying node.
*/
reference operator*() const;

/**
* Returns the pointer to the node.
*
* @return The pointer to the node.
*/
pointer get() const;

private:
void increment();
void decrement();

private:
pointer node_;
};

/// @endcond

}
#endif //OOS_PROTOTYPE_ITERATOR_HPP
