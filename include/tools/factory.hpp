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

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

#include <map>

namespace oos {

template < class K, class V >
class factory
{
public:
  class producer_base;

  typedef K key_type;
  typedef V value_type;
  typedef std::tr1::shared_ptr<producer_base> producer_ptr;
  typedef std::map<key_type, producer_ptr> container_t;
  typedef typename container_t::size_type size_type;
  typedef typename container_t::iterator iterator;
  typedef typename container_t::const_iterator const_iterator;

public:
  class producer_base
  {
  public:
    virtual ~producer_base() {}
    virtual value_type* create() const = 0;
  };
  
  template < class T >
  class default_producer : public producer_base
  {
    virtual ~default_producer() {}
    virtual value_type* create() const
    {
      return new T;
    }
  };
  
public:
  factory() {}
  ~factory() {}
  
  std::pair<iterator, bool> insert(const key_type &key, producer_base *p)
  {
    return container_.insert(std::make_pair(key, producer_ptr(p)));
  };
  
  size_type erase(const key_type &key)
  {
    return container_.erase(key);
  }
  
  value_type* create(const key_type &key) const
  {
    const_iterator i = container_.find(key);
    if (i == end()) {
      return 0;
    }
    return i->second->create();
  }
  
  iterator begin()
  {
    return container_.begin();
  }
  
  const_iterator begin() const
  {
    return container_.begin();
  }
  
  iterator end()
  {
    return container_.end();
  }
  
  const_iterator end() const
  {
    return container_.end();
  }
  
  bool empty() const
  {
    return container_.empty();
  }
  
  size_type size() const
  {
    return container_.size();
  }

  size_type count(const key_type &key) const
  {
    return container_.count();
  }

  iterator find(const key_type &key)
  {
    return container_.find(key);
  }

  const_iterator find(const key_type &key) const
  {
    return container_.find(key);
  }

private:
  container_t container_;
};

}

#endif /* FACTORY_HPP */
