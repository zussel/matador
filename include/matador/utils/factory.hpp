/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <memory>
#include <map>

namespace matador {

/**
 * @tparam K The type for the key value.
 * @tparam V The value type.
 * @class factory
 * @brief A generic factory class.
 *
 * This is a generic factory class where values
 * are stored in producer objects as pointers
 * wrapped by std::shared_ptr of type V.
 * The key type is used to identify the producer.
 * Once create is called a new serializable of value
 * type V is created and returned.
 * The class uses a producer interface of type
 * producer_base and a default producer class
 * default_producer is provided.
 */
template < class K, class V >
class factory
{
public:
  class producer_base;

  typedef K key_type;                                          /**< Shortcut for key type. */
  typedef V value_type;                                        /**< Shortcut for value type. */
  typedef std::shared_ptr<producer_base> producer_ptr;    /**< Shortcut for producer pointer class. */
  typedef std::map<key_type, producer_ptr> container_t;        /**< Shortcut for map type. */
  typedef typename container_t::size_type size_type;           /**< Shortcut for size type. */
  typedef typename container_t::iterator iterator;             /**< Shortcut for iterator. */
  typedef typename container_t::const_iterator const_iterator; /**< Shortcut for const_iterator. */

public:
  /**
   * @class producer_base
   * @brief Base producer class for factory
   *
   * The base class for the
   * producer classes
   */
  class producer_base
  {
  public:
    virtual ~producer_base() = default;
    /**
     * Creates a new serializable of value_type
     *
     * @return A new serializable of value_type.
     */
    virtual value_type* create() const = 0;
  };
  
  /**
   * @tparam T Type of the serializable to produce.
   * @class default_producer
   * @brief Default producer class for factory
   * 
   * This class represents the default
   * producer class which simple create
   * a new serializable of value_type by calling
   * new T.
   */
  template < class T >
  class default_producer : public producer_base
  {
    virtual ~default_producer() = default;
    /**
     * Returns a new serializable of value_type
     * by calling new T.
     *
     * @return A new serializable of value_type.
     */
    virtual value_type* create() const
    {
      return new T;
    }
  };
  
public:
  /**
   * Creates an empty factory.
   */
  factory() = default;
  ~factory() = default;
  
  /**
   * Inserts a new producer for a type
   * identified by the given key. If the key
   * already exists the value of the second
   * parameter is false and the returned iterator
   * is the existing producer.
   *
   * @param key The key value for the producer.
   * @param p   The producer.
   * @return    A pair containing the new or existing iterator
   *            and a boolean value indicating if the insertion
   *            succeeded.
   */
  std::pair<iterator, bool> insert(const key_type &key, producer_base *p)
  {
    return container_.insert(std::make_pair(key, producer_ptr(p)));
  };
  
  /**
   * Erase the value with given key and
   * returns the number of erased values.
   *
   * @param key The key type to erase.
   * @return The number of erased values.
   */
  size_type erase(const key_type &key)
  {
    return container_.erase(key);
  }
  
  /**
   * Creates a new value for the given
   * key. If the key is valid the underlying
   * producer calls create and returns a new
   * serializable. If the key is invalid null is returned.
   *
   * @param key The key type to erase.
   * @return A new serializable of value_type or null.
   */
  value_type* create(const key_type &key) const
  {
    const_iterator i = container_.find(key);
    if (i == end()) {
      return nullptr;
    }
    return i->second->create();
  }
  
  /**
   * Return the begin iterator
   *
   * @return The begin iterator.
   */
  iterator begin()
  {
    return container_.begin();
  }
  
  /**
   * Return the const begin iterator
   *
   * @return The const begin iterator.
   */
  const_iterator begin() const
  {
    return container_.begin();
  }
  
  /**
   * Return the end iterator
   *
   * @return The end iterator.
   */
  iterator end()
  {
    return container_.end();
  }
  
  /**
   * Return the const end iterator
   *
   * @return The const end iterator.
   */
  const_iterator end() const
  {
    return container_.end();
  }
  
  /**
   * Return true if the factory is empty.
   *
   * @return Returns true on empty factory.
   */
  bool empty() const
  {
    return container_.empty();
  }
  
  /**
   * Returns the current number
   * of producer in the factory.
   *
   * @return Number of producers.
   */
  size_type size() const
  {
    return container_.size();
  }

  /**
   * Returns the number of producers
   * with the given key.
   *
   * @param key The key type to search.
   * @return The number of producers with key.
   */
  size_type count(const key_type &key) const
  {
    return container_.count(key);
  }

  /**
   * Returns the iterator with the producer
   * of given type or end if key isn't found.
   *
   * @param key The key type to search.
   * @return The iterator corresponding to the key.
   */
  iterator find(const key_type &key)
  {
    return container_.find(key);
  }

  /**
   * Returns the iterator with the producer
   * of given type or end if key isn't found.
   *
   * @param key The key type to search.
   * @return The iterator corresponding to the key.
   */
  const_iterator find(const key_type &key) const
  {
    return container_.find(key);
  }

private:
  container_t container_;
};

}

#endif /* FACTORY_HPP */
