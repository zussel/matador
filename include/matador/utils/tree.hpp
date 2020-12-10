#ifndef MATADOR_TREE_HPP // -*- c++ -*-
#define MATADOR_TREE_HPP

#include <cassert>
#include <iterator>
#include <algorithm>

#include <iostream>

namespace matador {

/// @cond MATADOR_DEV
template < class T >
struct t_tree_node {
  t_tree_node<T> *parent = nullptr;
  t_tree_node<T> *prev = nullptr;
  t_tree_node<T> *next = nullptr;
  t_tree_node<T> *first = nullptr;
  t_tree_node<T> *last = nullptr;
	T data{};
};

template < class T >
bool operator<(const t_tree_node<T> &a, const t_tree_node<T> &b) {
	return a.data < b.data;
}

template < class T > class tree;
template < class T > class tree_iterator;
template < class T >
class const_tree_iterator_base;

/// @endcond

/**
 * Base class for tree iterator types
 *
 * @tparam T Type of the tree containing data
 */
template < class T >
class tree_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
	typedef tree_iterator_base<T> self; /**< Shortcut to self */
	typedef t_tree_node<T> t_node;      /**< Shortcut to node class */

	typedef T  value_type;  /**< Shortcut to value type */
	typedef T* pointer;     /**< Shortcut to pointer of value type */
	typedef T& reference ;  /**< Shortcut to reference of value type */

	/**
	 * Default constructor
	 */
	tree_iterator_base() = default;

	/**
	 * Initialize the base iterator with a
	 * given node
	 *
	 * @param n Initial node for the iterator
	 */
	explicit tree_iterator_base(t_node *n) : node(n) {}

	/**
	 * Assign another iterator x to this iterator
	 *
	 * @param x Iterator to copy assign
	 * @return The copied iterator
	 */
	tree_iterator_base<T>& operator=(const tree_iterator_base<T> &x) = default;

	/**
	 * Move assign another iterator to the iterator
	 * @param x Iterator to move assign
	 * @return The moved iterator
	 */
	tree_iterator_base<T>& operator=(tree_iterator_base<T> &&x) noexcept = default;

	/**
	 * Default destructor
	 */
	virtual ~tree_iterator_base() = default;

	/**
	 * Equal compares another iterator with this
	 *
	 * @param i The iterator to compare
	 * @return True if the iterators are the same
	 */
	bool operator==(const self &i) const
	{
		return (node == i.node);
	}

	bool operator==(const const_tree_iterator_base<T> &i) const;

	/**
	 * Not equal compares another iterator with this
	 *
	 * @param i The iterator to compare
	 * @return True if the iterators are not the same
	 */
	bool operator!=(const self &i) const
	{
		return (node != i.node);
	}

  bool operator!=(const const_tree_iterator_base<T> &i) const;

  /**
	 * Returns a reference nodes data
	 *
	 * @return Reference to nodes data
	 */
	reference operator*() const { return static_cast<t_node*>(this->node)->data; }

  /**
   * Returns a reference nodes data
   *
   * @return Reference to nodes data
   */
	pointer operator->() const { return &(operator*()); }

  /**
   * @brief Pre increments self
   *
   * @return A reference to incremented self
   */
	self& operator++()
	{
		increment();
		return (*this);
	}

  /**
   * @brief Pre decrements self
   *
   * @return A reference to decremented self
   */
	self& operator--()
	{
		decrement();
		return (*this);
	}

  /**
   * @brief Post increments iterator
   *
   * Post increments iterator and returns a
   * new iterator object.
   *
   * @return Returns new incremented iterator
   */
	self operator++(int)
	{
		self tmp = *this;
		increment();
		return tmp;
	}

  /**
   * @brief Post decrements iterator
   *
   * Post decrements iterator and returns a
   * new iterator object.
   *
   * @return Returns new decremented iterator
   */
	self operator--(int) {
		self tmp = *this;
		decrement();
		return tmp;
	}

protected:
  friend class tree<T>;
  friend class tree_iterator<T>;
  friend class const_tree_iterator_base<T>;

/// @cond MATADOR_DEV
	virtual void decrement() {}
	virtual void increment() {}

	t_node *node = nullptr;
/// @endcond
};


/**
 * Base class for const tree iterator types
 *
 * @tparam T Type of the tree containing data
 */
template < class T >
class const_tree_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T&, const T*> {
public:
  typedef const_tree_iterator_base<T> self;   /**< Shortcut to self */
  typedef tree_iterator_base<T> iterator;     /**< Shortcut to non const tree iterator */
  typedef t_tree_node<T> t_node;              /**< Shortcut to node class */

  typedef T  value_type;  /**< Shortcut to value type */
  typedef T* pointer;     /**< Shortcut to pointer of value type */
  typedef T& reference ;  /**< Shortcut to reference of value type */

  /**
   * Default constructor
   */
	const_tree_iterator_base() = default;

  /**
   * Copy from iterator class
   *
   * @param x Iterator to copy from
   */
	explicit const_tree_iterator_base(const iterator &x) : node(x.node) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   */
	explicit const_tree_iterator_base(t_node *n) : node(n) {}

	/**
	 * Default destructor
	 */
	virtual ~const_tree_iterator_base() = default;

  /**
   * Equal compares another iterator with this
   *
   * @param i The iterator to compare
   * @return True if the iterators are the same
   */
  bool operator==(const self &i) const
  {
    return (node == i.node);
  }

  bool operator==(const tree_iterator_base<T> &i) const
  {
    return (node == i.node);
  }

  /**
   * Not equal compares another iterator with this
   *
   * @param i The iterator to compare
   * @return True if the iterators are not the same
   */
  bool operator!=(const self &i) const
  {
    return (node != i.node);
  }

  bool operator!=(const tree_iterator_base<T> &i) const
  {
    return (node != i.node);
  }

  /**
   * Returns a reference nodes data
   *
   * @return Reference to nodes data
   */
  reference operator*() const { return static_cast<t_node*>(this->node)->data; }

  /**
   * Returns a reference nodes data
   *
   * @return Reference to nodes data
   */
  pointer operator->() const { return &(operator*()); }

  /**
   * @brief Pre increments self
   *
   * @return A reference to incremented self
   */
  self& operator++()
  {
    increment();
    return (*this);
  }

  /**
   * @brief Pre decrements self
   *
   * @return A reference to decremented self
   */
  self& operator--()
  {
    decrement();
    return (*this);
  }

  /**
   * @brief Post increments iterator
   *
   * Post increments iterator and returns a
   * new iterator object.
   *
   * @return Returns new incremented iterator
   */
  self operator++(int)
  {
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * @brief Post decrements iterator
   *
   * Post decrements iterator and returns a
   * new iterator object.
   *
   * @return Returns new decremented iterator
   */
  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

protected:
  friend class tree<T>;
  friend class tree_iterator<T>;
  friend class tree_iterator_base<T>;

/// @cond MATADOR_DEV
	virtual void decrement() {}
	virtual void increment() {}

	t_node *node = nullptr;
/// @endcond
};

template < class T >
bool tree_iterator_base<T>::operator==(const const_tree_iterator_base<T> &i) const
{
  return (node == i.node);
}

template < class T >
bool tree_iterator_base<T>::operator!=(const const_tree_iterator_base<T> &i) const
{
  return (node != i.node);
}

/**
 * Default tree iterator iterates
 * tree level by level from left to right
 * starting with the root node
 *
 * @tparam T Type of the tree
 */
template < typename T >
class tree_iterator : public tree_iterator_base<T> {
public:
	typedef typename tree_iterator_base<T>::t_node node_type; /**< Shortcut to node class */

	/**
	 * Default constructor
	 */
	tree_iterator() = default;

	/**
	 * Copy constructor
	 *
	 * @param x Iterator to copy from
	 */
	tree_iterator(const tree_iterator<T> &x) : tree_iterator_base<T>(x.node) {}

	/**
	 * Copy from base iterator class
	 *
	 * @param x Iterator to copy from
	 */
	tree_iterator(const tree_iterator_base<T> &x) : tree_iterator_base<T>(x.node) {}

	/**
	 * Initializes the iterator with the given node
	 *
	 * @param n Initial node of the iterator
	 */
	explicit tree_iterator(node_type *n) : tree_iterator_base<T>(n) {}

	/**
	 * Copy assign constructor
	 *
	 * @param x Iterator to copy from
	 * @return The assign iterator
	 */
	tree_iterator<T>& operator=(const tree_iterator<T> &x)
  {
	  tree_iterator_base<T>::operator=(x);
	  return *this;
  }

  /**
   * Move assign constructor
   *
   * @param x Iterator to move assign from
   * @return The moved iterator
   */
	tree_iterator<T>& operator=(tree_iterator<T> &&x) noexcept
  {
	  tree_iterator_base<T>::operator=(x);
	  return *this;
  }

protected:
/// @cond MATADOR_DEV
	virtual void decrement();
	virtual void increment();
/// @endcond
};

/**
 * Default const tree iterator iterates
 * tree level by level from left to right
 * starting with the root node
 *
 * @tparam T Type of the tree
 */
template < typename T >
class const_tree_iterator : public const_tree_iterator_base<T> {
public:
	typedef typename const_tree_iterator_base<T>::t_node t_node; /**< Shortcut to node type */

	/**
	 * Default constructor
	 */
	const_tree_iterator() = default;

	/**
	 * Copy constructs from given const tree base iterator
	 *
	 * @param x Iterator to copy from
	 */
  explicit const_tree_iterator(const const_tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_iterator(const tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   */
	explicit const_tree_iterator(t_node *n) : const_tree_iterator_base<T>(n) {}

protected:
/// @cond MATADOR_DEV
	virtual void decrement();
	virtual void increment();
/// @endcond
};

/**
 * Sibling iterator iterates only the sibling
 * of another iterator
 *
 * @tparam T Type of the tree
 */
template < typename T >
class tree_sibling_iterator : public tree_iterator_base<T> {
public:
	typedef typename tree_iterator_base<T>::t_node t_node; /**< Shortcut to the node type */

  /**
   * Default constructor
   */
	tree_sibling_iterator() = default;

  /**
   * Copy constructs from given iterator
   *
   * @param x Iterator to copy from
   */
  tree_sibling_iterator(const tree_sibling_iterator<T> &x) : tree_iterator_base<T>(x.node) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit tree_sibling_iterator(const tree_iterator_base<T> &x) : tree_iterator_base<T>(x.node) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   */
	explicit tree_sibling_iterator(t_node *n) : tree_iterator_base<T>(n) {}

protected:
/// @cond MATADOR_DEV
	virtual void decrement() {
		assert(this->node);
		this->node = this->node->prev;
	}

	virtual void increment() {
		assert(this->node);
		this->node = this->node->next;
	}
/// @endcond
};

/**
 * Sibling iterator iterates only the sibling
 * of another iterator
 *
 * @tparam T Type of the tree
 */
template < typename T >
class const_tree_sibling_iterator : public const_tree_iterator_base<T> {
public:
	typedef typename const_tree_iterator_base<T>::t_node t_node; /**< Shortcut to the node type */

	/**
	 * Default constructor
	 */
	const_tree_sibling_iterator() = default;

  /**
   * Copy constructs from given const tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_sibling_iterator(const const_tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_sibling_iterator(const tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   */
	explicit const_tree_sibling_iterator(t_node *n) : const_tree_iterator_base<T>(n) {}

protected:
/// @cond MATADOR_DEV
	virtual void decrement() {
		assert(this->node);
		this->node = this->node->prev;
	}

	virtual void increment() {
		assert(this->node);
		this->node = this->node->next;
	}
/// @endcond
};

/**
 * A subtree iterator iterates over all
 * elements below a given node.
 *
 * @tparam T Type of the iterator
 */
template < typename T >
class tree_subtree_iterator : public tree_iterator_base<T> {
public:
	typedef typename tree_iterator_base<T>::t_node t_node; /**< Shortcut to the node type */

  /**
   * Default constructor
   */
	tree_subtree_iterator() = default;

  /**
   * Copy constructs from given iterator
   *
   * @param x Iterator to copy from
   */
  tree_subtree_iterator(const tree_subtree_iterator<T> &x) : tree_iterator_base<T>(x.node), root(nullptr) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit tree_subtree_iterator(const tree_iterator_base<T> &x) : tree_iterator_base<T>(x.node), root(nullptr) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   */
	explicit tree_subtree_iterator(t_node *n) : tree_iterator_base<T>(n), root(n->parent) {}

protected:
/// @cond MATADOR_DEV
	t_node *root = nullptr;

	virtual void decrement();        
	virtual void increment();
/// @endcond
};

/**
 * A subtree iterator iterates over all
 * elements below a given node.
 *
 * @tparam T Type of the iterator
 */
template < typename T >
class const_tree_subtree_iterator : public const_tree_iterator_base<T> {
public:
	typedef typename const_tree_iterator_base<T>::t_node t_node; /**< Shortcut to the node type */

  /**
   * Default constructor
   */
	const_tree_subtree_iterator() = default;

  /**
   * Copy constructs from given const tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_subtree_iterator(const const_tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node), root(nullptr) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_subtree_iterator(const tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node), root(nullptr) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   */
	explicit const_tree_subtree_iterator(t_node *n) : const_tree_iterator_base<T>(n), root(n->parent) {}

protected:
/// @cond MATADOR_DEV
	t_node *root = nullptr;

	virtual void decrement();        
	virtual void increment();
/// @endcond
};

/**
 * Leaf iterator only iterates over leafs
 * of the tree.
 *
 * @tparam T Type of the tree
 */
template < typename T >
class tree_leaf_iterator : public tree_iterator_base<T> {
public:
	typedef typename tree_iterator_base<T>::t_node t_node; /**< Shortcut to the node type */

  /**
   * Default constructor
   */
	tree_leaf_iterator() = default;

  /**
   * Copy constructs from given iterator
   *
   * @param x Iterator to copy from
   */
  tree_leaf_iterator(const tree_leaf_iterator<T> &x) : tree_iterator_base<T>(x.node) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit tree_leaf_iterator(const tree_iterator_base<T> &x) : tree_iterator_base<T>(x.node) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   * @param as_begin Indicates that the given node is the beginning node
   */
	tree_leaf_iterator(t_node *n, bool as_begin) : tree_iterator_base<T>(n)
	{
	  if (as_begin) {
      while (this->node->first && this->node->first->next != this->node->last) {
        this->node = this->node->first->next;
      }
	  }
	}

protected:
/// @cond MATADOR_DEV
	virtual void decrement();
	virtual void increment();
/// @endcond
};

/**
 * Leaf const iterator only iterates over leafs
 * of the tree.
 *
 * @tparam T Type of the tree
 */
template < typename T >
class const_tree_leaf_iterator : public const_tree_iterator_base<T> {
public:
	typedef typename const_tree_iterator_base<T>::t_node t_node; /**< Shortcut to the node type */

  /**
   * Default constructor
   */
	const_tree_leaf_iterator() = default;

  /**
   * Copy constructs from given const tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_leaf_iterator(const const_tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node) {}

  /**
   * Copy constructs from given tree base iterator
   *
   * @param x Iterator to copy from
   */
  explicit const_tree_leaf_iterator(const tree_iterator_base<T> &x) : const_tree_iterator_base<T>(x.node) {}

  /**
   * Initializes the iterator with the given node
   *
   * @param n Initial node of the iterator
   * @param as_begin Indicates that the given node is the beginning node
   */
	const_tree_leaf_iterator(t_node *n, bool as_begin) : const_tree_iterator_base<T>(n)
	{
    if (as_begin) {
      while (this->node->first && this->node->first->next != this->node->last) {
        this->node = this->node->first->next;
      }
    }
	}

protected:
/// @cond MATADOR_DEV
	virtual void decrement();
	virtual void increment();
/// @endcond
};

/**
 * @class tree
 * @brief A STL like tree class
 * 
 * The tree class provides a container with an STL like interface. One
 * can store data in it a like tree. There are several iterator types,
 * find methods and modify (insert, push_back/front) methods.
 * Access is provide via iterators.
 */
template < class T >
class tree {
public:
	typedef t_tree_node<T> t_node;  /**< Shortcut to node type */
	typedef T value_type;           /**< Shortcut to value type */

	/**
	 * Creates an empty tree of type T.
	 */
	tree();
	/**
	 * Cleans up the tree but not the stored data.
	 */
	~tree();

public:
	typedef tree_iterator<T> iterator;                            /**< Shortcut to default iterator type */
	typedef const_tree_iterator<T> const_iterator;                /**< Shortcut to default const iterator type */
	typedef tree_sibling_iterator<T> sibling_iterator;            /**< Shortcut to sibling iterator type */
	typedef const_tree_sibling_iterator<T> const_sibling_iterator;/**< Shortcut to const sibling iterator type */
	typedef tree_subtree_iterator<T> subtree_iterator;            /**< Shortcut to subtree iterator type */
	typedef const_tree_subtree_iterator<T> const_subtree_iterator;/**< Shortcut to const subtree iterator type */
	typedef tree_leaf_iterator<T> leaf_iterator;                  /**< Shortcut to leaf iterator type */
	typedef const_tree_leaf_iterator<T> const_leaf_iterator;      /**< Shortcut to const leaf iterator type */
	typedef std::reverse_iterator<iterator> reverse_iterator;     /**< Shortcut to reverse iterator type */
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator; /**< Shortcut to const reverse iterator type */

	typedef T& reference; /**< Shortcut to value reference type */
	typedef T* pointer;   /**< Shortcut to value pointer type */
	typedef const T& const_reference; /**< Shortcut to value const reference type */
	typedef const T* const_pointer;   /**< Shortcut to value const pointer type */

	/**
	 * Returns the begin iterator of the tree
	 * @return begin iterator of the tree
	 */
	iterator begin() {
		return iterator(root_->next);
	}
	/**
	 * Returns the end iterator of the tree
	 * @return end iterator of the tree
	 */
	iterator end() {
		return iterator(root_);
	}
	/**
	 * Returns the constant begin iterator of the tree
	 * @return constant begin iterator of the tree
	 */
	const_iterator begin() const {
		return const_iterator(root_->next);
	}
	/**
	 * Returns the constant end iterator of the tree
	 * @return constant end iterator of the tree
	 */
	const_iterator end() const {
		return const_iterator(root_);
	}
	/**
	 * Returns the reverse begin iterator of the tree
	 * @return reverse begin iterator of the tree
	 */
	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	/**
	 * Returns the reverse end iterator of the tree
	 * @return reverse end iterator of the tree
	 */
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	/**
	 * Returns the constant reverse begin iterator of the tree
	 * @return constant reverse begin iterator of the tree
	 */
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(end());
	}
	/**
	 * Returns the constant reverse end iterator of the tree
	 * @return constant reverse end iterator of the tree
	 */
	const_reverse_iterator rend() const {
		return const_reverse_iterator(begin());
	}
	/**
	 * Returns the sibling begin iterator for a node (iterator)
	 * @param i
	 * @return sibling begin iterator
	 */
	sibling_iterator begin(const tree_iterator_base<T> &i) {
		return sibling_iterator(i.node->first->next);
	}
	/**
	 * Returns the sibling end iterator for a node (iterator)
	 * @param i
	 * @return sibling end iterator
	 */
	sibling_iterator end(const tree_iterator_base<T> &i) {
		return sibling_iterator(i.node->last);
	}
	/**
	 * Returns the constant sibling begin iterator for a node (iterator)
	 * @param i
	 * @return constant sibling begin iterator
	 */
	const_sibling_iterator begin(const tree_iterator_base<T> &i) const {
		return const_sibling_iterator(i.node->first->next);
	}
	/**
	 * Returns the constant sibling end iterator for a node (iterator)
	 * @param i
	 * @return constant sibling end iterator
	 */
	const_sibling_iterator end(const tree_iterator_base<T> &i) const {
		return const_sibling_iterator(i.node->last);
	}
	/**
	 * Returns the subtree begin iterator for a node (iterator)
	 * @param i
	 * @return subtree begin iterator
	 */
	subtree_iterator begin_subtree(const tree_iterator_base<T> &i) {
		return subtree_iterator(i.node->first->next);
	}
	/**
	 * Returns the subtree end iterator for a node (iterator)
	 * @param i
	 * @return subtree end iterator
	 */
	subtree_iterator end_subtree(const tree_iterator_base<T> &i) {
		return subtree_iterator(i.node->last);
	}
	/**
	 * Returns the constant subtree begin iterator for a node (iterator)
	 * @param i
	 * @return constant subtree begin iterator
	 */
	const_subtree_iterator begin_subtree(const const_tree_iterator_base<T> &i) const {
		return const_subtree_iterator(i.node->first->next);
	}
	/**
	 * Returns the constant subtree begin iterator for a node (iterator)
	 * @param i
	 * @return constant subtree begin iterator
	 */
	const_subtree_iterator begin_subtree(const tree_iterator_base<T> &i) const {
		return const_subtree_iterator(i.node->first->next);
	}
	/**
	 * Returns the constant subtree end iterator for a node (iterator)
	 * @param i
	 * @return constant subtree end iterator
	 */
	const_subtree_iterator end_subtree(const const_tree_iterator_base<T> &i) const {
		return const_subtree_iterator(i.node->last);
	}
	/**
	 * Returns the constant subtree end iterator for a node (iterator)
	 * @param i
	 * @return constant subtree end iterator
	 */
	const_subtree_iterator end_subtree(const tree_iterator_base<T> &i) const {
		return const_subtree_iterator(i.node->last);
	}
	/**
	 * Return the first leaf node as iterator
	 * @return leaf begin iterator
	 */
	leaf_iterator begin_leaf() {
		return leaf_iterator(root_->next, true);
	}
	/**
	 * Return the first leaf node as constant iterator
	 * @return constant leaf begin iterator
	 */
	const_leaf_iterator begin_leaf() const {
		return const_leaf_iterator(root_->next);
	}
	/**
	 * Return the end leaf node as iterator
	 * @return leaf end iterator
	 */
	leaf_iterator end_leaf() {
//		return leaf_iterator(end_, false);
		return leaf_iterator(root_, false);
	}
	/**
	 * Return the first leaf node as constant iterator
	 * @return constant leaf begin iterator
	 */
	const_leaf_iterator end_leaf() const {
		return const_leaf_iterator(root_);
	}
	/**
	 * Returns the current size of the tree (takes linear amount of time)
	 * @return size of tree
	 */
	size_t size() const {
		return std::distance(begin(), end());
	}
	/**
	 * Returns the current amount of children of the given node
	 * (takes linear amount of time)
	 * @return size of children under node
	 */
	size_t size(const tree_iterator_base<T> &i) const {
		return std::distance(begin_subtree(i), end_subtree(i));
	}
	/**
	 * Returns wether the tree is empty or not
	 * @return wether the tree is empty
	 */
	bool empty() const {
		return root_->next == root_;
	}
	/**
	 * Returns if iterator has children
	 * @param x iterator used for empty check
	 * @return wether the node has children
	 */
	bool empty(const tree_iterator_base<T> &x) const {
		return x.node->first->next == x.node->last;
	}
	/**
	 * Empties the tree
	 */
	void clear();
	/**
	 * Erase iterator i from tree include the complete subtree
	 * @param i the iterator to erase
	 * @return the next valid node as iterator
	 */
	template <typename iter> iter erase(iter i);
	/**
	 * Moves subtree represented by iterator a and insert before iterator b
	 * @param a iterator to move
	 * @param b iterator where insertion takes place (before)
	 */
	template <typename iter> void move(iter a, iter b);
	
	// merge
	//void merge(tree<T> &t);
	// remove
	// remove_if
	// unique

	/**
	 * Insert as previous sibling of i and returns new node iterator
	 * @param i next sibling of new node
	 * @param x new node of type T
	 * @return new iterator of node x
	 */
	template <typename iter> iter insert(iter i, const T &x);
	/**
	 * Insert as last child of i and returns new node iterator
	 * @param i parent children of new node of type T
	 * @param v new node of type T
	 * @return new iterator of node x
	 */
	template <typename iter> iter push_back_child(iter i, const T &v);
	/**
	 * Insert as first child of i and returns new node iterator
	 * @param i parent children of new node of type T
	 * @param v new node of type T
	 * @return new iterator of node x
	 */
	template <typename iter> iter push_front_child(iter i, const T &v);
	/**
	 * Sort children of iterator node i with standard compare operator
	 * @param i parent node of children to sort
	 */
	void sort_children(tree_iterator_base<T> &i);
	/**
	 * Sort children of iterator node i with compare operator of type CMP
	 * @param i parent node of children to sort
	 * @param cmp compare operator
	 */
	template < typename CMP >
	void sort_children(tree_iterator_base<T> &i, CMP cmp);
	/**
	 * Sorts the complete tree with the standard compare method. But
	 * the sorting is done for all siblings (children of a node) discretely
	 */
	void sort();
	/**
	 * Sorts the complete tree with the given compare method. But
	 * the sorting is done for all siblings (children of a node) discretely
	 * @param cmp compare operator
	 */
	template < typename CMP >
	void sort(CMP cmp);

	typedef std::pair<sibling_iterator, sibling_iterator> range_pair; /**< Shortcut to a range pair type */

	/**
	 * Returns two iterators representing a range of children nodes
	 * which equals a given predicate (e.g. same name)
	 * Note: children must be sorted, otherwise a range of all nodes
	 * equaling the predicate can't be guaranted!
	 * @param i parent iterator of future range children
	 * @param predicate which all range children must equal
	 * @return range of children (as range_pair, first and last iterator)
	 */
	template < typename Predicate >
	range_pair equal_range(const tree_iterator_base<T> &i, Predicate predicate) const;
	/**
	 * Returns the parent node of iterator. if iterator hasn't a
	 * parent node end() is returned
	 * @param i iterator to get the parent from
	 * @return the parent iterator
	 */
	iterator parent(const tree_iterator_base<T> &i) const;
	/**
	 * Search a node within a given path. Path comes as two
	 * iterators pfirst and plast over which can be iterated.
	 * @param pfirst the first iterator of path
	 * @param plast the last iterator of path
	 * @param predicate the search rule for the node to find
	 * @return the first iterator which equals the predicate
	 */
	template <typename iter, typename Predicate>
	iterator find_in_path(iter pfirst, iter plast, Predicate predicate);
	/**
	 * Returns the depth of the given node iterator i
	 * @param i iterator for which the depth is returned
	 * @return depth of iterator (root is 0 (zero))
	 */
	size_t depth(const tree_iterator_base<T> &i) const;
	/**
	 * Returns the depth of the given node constant iterator i
	 * @param i constant iterator for which the depth is returned
	 * @return depth of iterator (root is 0 (zero))
	 */
	size_t depth(const const_tree_iterator_base<T> &i) const;
	/**
	 * Returns the depth of the given node reverse iterator i
	 * @param i reverse iterator for which the depth is returned
	 * @return depth of iterator (root is 0 (zero))
	 */
	size_t depth(const reverse_iterator &i) const { return this->depth(--i.base()); }
	/**
	 * Returns the depth of the given node constant reverse iterator i
	 * @param i constant reverse iterator for which the depth is returned
	 * @return depth of iterator (root is 0 (zero))
	 */
	size_t depth(const const_reverse_iterator &i) const { return this->depth(--i.base()); }
private:
	void init();
	void init_children(t_node *node);

	void clear_children(t_node *node);

private:
	t_node *root_;
//	t_node *end_;
};

/// @cond MATADOR_DEV

template < class T >
tree<T>::tree() {
  init();
}

template < class T >
tree<T>::~tree() {
  clear();
  delete root_;
//  delete end_;
}

template < class T >
void
tree<T>::init() {
  root_ = new t_node;
  //end_ = new t_node;
  root_->next = root_;
  root_->prev = root_;
  //end_->prev = root_;
}

template < class T >
void tree<T>::init_children(t_node *node) {
  auto f = new t_node;
  auto l = new t_node;
  f->parent = node;
  l->parent = node;
  f->next = l;
  l->prev = f;
  node->first = f;
  node->last = l;
}

template < class T >
void tree<T>::clear_children(t_node *node) {
  t_node *tmp;
  auto first = node->first->next;
  auto last = node->last;
  // only clear nodes between first and last
  while (first != last) {
    tmp = first;
    if (tmp->first) {
      // clear children
      clear_children(tmp);
      // delete first and last of current child
      delete tmp->first;
      delete tmp->last;
    }
    // choose next child
    first = first->next;
    // delete old child
    delete tmp;
  }
  // finally link first to last and vice versa
  node->first->next = node->last;
  node->last->prev = node->first;
}

template < class T >
void tree<T>::clear() {
  auto first = root_->next;
  t_node *tmp;
  while (first != root_) {
    tmp = first;
    clear_children(tmp);
    delete tmp->first;
    delete tmp->last;
    first = first->next;
    delete tmp;
  }
  root_->next = root_;
  root_->prev = root_;
//  end_->prev = root_;
}

template < class T >
template <typename iter>
iter tree<T>::erase(iter i) {
  clear_children(i.node);

  // relink
  i.node->prev->next = i.node->next;
  i.node->next->prev = i.node->prev;

  // finally delete it
  delete i.node->first;
  delete i.node->last;

	iter ret(i.node->next);
  delete i.node;
  
  return ret;
}

template <class T>
template <typename iter>
void tree<T>::move(iter a, iter b) {
  // relink
  a.node->prev->next = a.node->next;
  a.node->next->prev = a.node->prev;

  // insert node before b
  // same parent as b
  a.node->parent = b.node->parent;
  // a is now predeccessor of b
  a.node->prev = b.node->prev;
  a.node->prev->next = a.node;
  // a is now successor of bs former predecessor
  a.node->next = b.node;
  b.node->prev = a.node;
}

template <class T>
template <typename iter>
iter tree<T>::insert(iter i, const T& x) {
  auto node = new t_node;
  node->data = x;
  // set proper children "list"
  init_children(node);
  node->parent = i.node->parent;
  node->next = i.node;
  node->prev = i.node->prev;
  // link previous sibling of i to us
  i.node->prev = node;
  node->prev->next = node;
  return iter(node);
}

template <class T>
template <typename iter>
iter tree<T>::push_back_child(iter i, const T& v) {
  assert(i.node->parent != root_);
  return insert(iter(i.node->last), v);
}

template <class T>
template <typename iter>
iter tree<T>::push_front_child(iter i, const T& v) {
  assert(i.node->parent != root_);
  return insert(iter(i.node->first->next), v);
}

template < class T >
void tree<T>::sort_children(tree_iterator_base<T> &i) {
  // copy children to array
  size_t size = std::distance(begin(i), end(i));
  auto *a = new t_node[size];
  auto *first = i.node->first->next;
  int j = 0;
  while (first != i.node->last) {
    a[j++] = *first;
    first = first->next;
  }
  // sort array with compare function
  std::sort(a, a+size);
  // relink children
  first = i.node->first->next;
  j = 0;
  while (first != i.node->last) {
    first->data = a[j++].data;
    first = first->next;
  }
  delete [] a;
}

template < class T >
template < typename Compare >
void tree<T>::sort_children(tree_iterator_base<T> &i, Compare cmp) {
  // copy children to array
  size_t size = std::distance(begin(i), end(i));
  auto *a = new t_node[size];
  t_node *first = i.node->first->next;
  int j = 0;
  while (first != i.node->last) {
    a[j++] = *first;
    first = first->next;
  }
  // sort array with compare function
  std::sort(a, a+size, cmp);
  // relink children
  first = i.node->first->next;
  j = 0;
  while (first != i.node->last) {
    first->data = a[j++].data;
    first = first->next;
  }
  delete [] a;
}

template < class T >
void tree<T>::sort() {
	iterator first = this->begin();
	iterator last = this->end();
	while (first != last) {
		this->sort_children(first);
		++first;
	}
}

template < class T >
template < typename Compare >
void tree<T>::sort(Compare cmp) {
	iterator first = this->begin();
	iterator last = this->end();
	while (first != last) {
		this->sort_children(first, cmp);
		++first;
	}
}

template < typename T >
template < typename Predicate >
typename tree<T>::range_pair tree<T>::equal_range(const tree_iterator_base<T> &i, Predicate predicate) const {
  auto *j = i.node->first->next;
  t_node *last, *first;
  first = last = j;

  bool found = false;
  while (j != i.node->last) {
    if (found) {
      // find last node fitting the predicate
      if (predicate(j->data)) {
        j = j->next;
      } else {
        last = j;
        break;
      }
    } else {
      // find first node which fits the predicate
      if (!predicate(j->data)) {
        j = j->next;
      } else {
        first = j;
        found = true;
      }
    }
  }
  // return this sibling_iterator pair
  return range_pair(sibling_iterator(first), sibling_iterator(last));
}

template< class T >
typename tree<T>::iterator tree<T>::parent(const tree_iterator_base<T> &i) const {
  if (i.node->parent) {
    return iterator(i.node->parent);
  }
  return iterator(root_->next);
}

template<class T>
template<typename iter, typename Predicate>
typename tree<T>::iterator tree<T>::find_in_path(iter pfirst, iter plast, Predicate predicate) {
  if (pfirst == plast)
    return end();

  iterator current = std::find_if(begin(), end(), [&pfirst, &predicate](const T &item) {
    return predicate(*pfirst, item);
  });

  if (current == end())
    return end();

  iterator last;
  ++pfirst;

  while (pfirst != plast) {
    last = end(current);
    current = std::find_if(begin(current), end(current), [&pfirst, &predicate](const T &item) {
      return predicate(*pfirst, item);
    });

    if (current == last)
      return end();

    ++pfirst;
  }
  return current;
}

template < class T >
size_t tree<T>::depth(const tree_iterator_base<T> &i) const {
  t_tree_node<T> *node = i.node;
  assert(node);
  size_t d = 0;
  while(node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

template < class T >
size_t tree<T>::depth(const const_tree_iterator_base<T> &i) const {
  t_tree_node<T> *node = i.node;
  assert(node);
  size_t d = 0;
  while(node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

template < class T >
void tree_iterator<T>::decrement() {
  assert(this->node);
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (this->node->prev && this->node->prev->prev) {
    this->node = this->node->prev;
    while (this->node->last && this->node->first->next != this->node->last)
      this->node = this->node->last->prev;
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    this->node = this->node->parent;
  }
}

template < class T >
void tree_iterator<T>::increment() {
  assert(this->node);
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (this->node->first->next != this->node->last)
    this->node = this->node->first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    while (this->node->parent && this->node->next == this->node->parent->last) {
      this->node = this->node->parent;
    }
    this->node = this->node->next;
  }
}

template < class T >
void const_tree_iterator<T>::decrement() {
  assert(this->node);
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (this->node->prev && this->node->prev->prev) {
    this->node = this->node->prev;
    while (this->node->last && this->node->first->next != this->node->last)
      this->node = this->node->last->prev;
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    this->node = this->node->parent;
  }
}

template < class T >
void const_tree_iterator<T>::increment() {
  assert(this->node);
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (this->node->first->next != this->node->last)
    this->node = this->node->first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    while (this->node->parent && this->node->next == this->node->parent->last) {
      this->node = this->node->parent;
    }
    this->node = this->node->next;
  }
}

template < class T >
void tree_subtree_iterator<T>::increment() {
  assert(this->node);
  assert(root);
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (this->node->first->next != this->node->last)
    this->node = this->node->first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the root
    while (this->node->parent && this->node->next == this->node->parent->last && this->node->parent != root) {
      this->node = this->node->parent;
    }
    this->node = this->node->next;
  }
}

template < class T >
void tree_subtree_iterator<T>::decrement() {
  assert(this->node);
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (this->node->prev) {
    this->node = this->node->prev;
    while (this->node->last)
      this->node = this->node->last;
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    this->node = this->node->parent;
  }
}

template < class T >
void const_tree_subtree_iterator<T>::increment() {
  assert(this->node);
  assert(root);
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  //  std::cerr << "++ node <" << this->node << "> root <" << this->root << ">\n";
  if (this->node->first->next != this->node->last) {
    this->node = this->node->first->next;
    //    std::cerr << "   node->first->next <" << this->node << ">\n";
  } else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the root
    while (this->node->parent && this->node->next == this->node->parent->last && this->node->parent != root) {
      this->node = this->node->parent;
    }
    this->node = this->node->next;
    //    std::cerr << "   node->parent(...)->next <" << this->node << ">\n";
  }
}

template < class T >
void const_tree_subtree_iterator<T>::decrement() {
  assert(this->node);
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (this->node->prev) {
    this->node = this->node->prev;
    while (this->node->last)
      this->node = this->node->last;
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    this->node = this->node->parent;
  }
}

template < class T >
void tree_leaf_iterator<T>::increment() {
  assert(this->node);
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (this->node->first && this->node->first->next != this->node->last)
    this->node = this->node->first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    while (this->node->parent && this->node->next == this->node->parent->last) {
      this->node = this->node->parent;
    }
    this->node = this->node->next;
  }
	while (this->node->first && this->node->first->next != this->node->last) {
		this->node = this->node->first->next;
	}
}

template < class T >
void tree_leaf_iterator<T>::decrement() {
  assert(this->node);
  // if node hasn't a previous sibling we set the
  // node to the previous sibling of the first parent which
  // has a previous sibling
//  if (!this->node->prev || (this->node->prev && !this->node->prev->prev)) {
//		do {
//  		this->node = this->node->parent;
//		} while (this->node->parent && !this->node->parent->prev);
//	}
//  this->node = this->node->prev;
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
//	while (this->node->last && this->node->first->next != this->node->last)
//		this->node = this->node->last->prev;

  if (this->node->last && this->node->last->prev != this->node->first)
    this->node = this->node->last->prev;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    while (this->node->parent && this->node->prev == this->node->parent->first) {
      this->node = this->node->parent;
    }
    this->node = this->node->prev;
  }
  while (this->node->last && this->node->last->prev != this->node->first) {
    this->node = this->node->last->prev;
  }

}

template < class T >
void const_tree_leaf_iterator<T>::increment() {
  assert(this->node);
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (this->node->first->next != this->node->last)
    this->node = this->node->first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    while (this->node->parent && this->node->next == this->node->parent->last) {
      this->node = this->node->parent;
    }
    this->node = this->node->next;
  }
	while (this->node->first) {
		this->node = this->node->first;
	}
}

template < class T >
void const_tree_leaf_iterator<T>::decrement() {
  assert(this->node);
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (this->node->prev && this->node->prev->prev) {
    this->node = this->node->prev;
    while (this->node->last && this->node->first->next != this->node->last)
      this->node = this->node->last->prev;
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    this->node = this->node->parent;
  }
  while (this->node->last->prev) {
  	this->node = this->node->last->prev;
	}
}

/// @endcond
}

#endif /* MATADOR_TREE_HPP */
