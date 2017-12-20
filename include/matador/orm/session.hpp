//
// Created by sascha on 21.03.16.
//

#ifndef OOS_SESSION_HPP
#define OOS_SESSION_HPP

#ifdef _MSC_VER
#ifdef matador_orm_EXPORTS
#define OOS_ORM_API __declspec(dllexport)
#define EXPIMP_ORM_TEMPLATE
#else
#define OOS_ORM_API __declspec(dllimport)
#define EXPIMP_ORM_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_ORM_API
#endif

#include "matador/object/has_many.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/transaction.hpp"

#include "matador/orm/persistence.hpp"

namespace matador {

/**
 * @brief Represents a session to a database
 *
 * This class represents a session to a database
 * identified by a persistence object.
 *
 * @code
 *
 * matador::persistence p(<database connection string>);
 * p.attach<person>("person");
 *
 * matador::session s(p);
 *
 * @endcode
 *
 * Once a session object is instantiated one can
 * insert, update and delete objects.
 *
 * @code
 *
 * s.insert(new person("georg"));
 *
 * @endcode
 *
 * This session can also load the whole database into
 * the underlying object_store.
 */
class OOS_ORM_API session
{
public:
  /**
   * @brief Creates a new session object from persistence
   *
   * @param p The persistence object.
   */
  explicit session(persistence &p);

  /**
   * @brief Inserts an object.
   *
   * Inserts an object. This leads to a new database entry
   * and a new object in the underlying object_store.
   *
   * @tparam T The type of the object to be inserted
   * @param obj The object to be inserted
   * @return The new object wrapped by an object_ptr
   */
  template < class T >
  object_ptr<T> insert(T *obj)
  {
    if (store().has_transaction()) {
      return persistence_.store().insert(obj);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      object_ptr<T> optr(persistence_.store().insert(obj));
      tr.commit();
      return optr;
    }
  }

  /**
   * @brief Updates an object.
   *
   * Updates an object. This leads to an updated database entry
   * and an updated object in the underlying object_store.
   *
   * @tparam T The type of the object to be updated
   * @param optr The object to be updated
   * @return The updated object wrapped by an object_ptr
   */
  template < class T >
  object_ptr<T> update(const object_ptr<T> &optr)
  {
    if (store().has_transaction()) {
      store().current_transaction().on_update<T>(optr.proxy_);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      tr.on_update<T>(optr.proxy_);
      tr.commit();
    }
    return optr;
  }

  /**
   * @brief Deletes an object.
   *
   * Deletes an object. This leads to an deleted object
   * on the database as well as an deleted object in the
   * underlying object_store
   *
   * @tparam T The type of the object to deleted
   * @param optr The object to be deleted
   */
  template < class T >
  void remove(object_ptr<T> &optr)
  {
    if (store().has_transaction()) {
      store().current_transaction().on_delete<T>(optr.proxy_);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      store().remove(optr);
      tr.commit();
    }
  }

  /**
   * @brief Select all object of a specific type
   *
   * @tparam T Type of object to be selected
   * @return object_view containing the selection
   */
  template < class T >
  object_view<T> select()
  {
    return object_view<T>(store());
  }
  /**
   * @brief Inserts an object at the end of a container
   *
   * Inserts an object at the end of a container. This leads at
   * least to one new database entry. If the value is an entity/object
   * then a second object is also created.
   * The same changes are made to the underlying object_store
   * as well
   *
   * @tparam CONT The type of the container
   * @tparam T The type of the object
   * @param container The container the object should be inserted into
   * @param value The object to be inserted
   */
  template < template <class ...> class CONT, class T >
  void push_back(has_many<T, CONT> &container, const typename has_many<T, CONT>::value_type &value)
  {
    if (store().has_transaction()) {
      container.push_back(value);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      container.push_back(value);
      tr.commit();
    }
  }

  /**
   * @brief Inserts an object at the begin of a container
   *
   * Inserts an object at the begin of a container. This leads at
   * least to one new database entry. If the value is an entity/object
   * then a second object is also created.
   * The same changes are made to the underlying object_store
   * as well
   *
   * @tparam CONT The type of the container
   * @tparam T The type of the object
   * @param container The container the object should be inserted into
   * @param value The object to be inserted
   */
  template < template <class ...> class CONT, class T >
  void push_front(has_many<T, CONT> &container, const typename has_many<T, CONT>::value_type &value)
  {
    if (store().has_transaction()) {
      container.push_front(value);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      container.push_front(value);
      tr.commit();
    }
  }

  /**
   * @brief Deletes an object from a container
   *
   * Deletes an object identified by the given iterator
   * from a container. This leads at least to one removed
   * database entry.
   * The same changes are made to the underlying object_store
   * as well.
   *
   * @tparam CONT The type of the container
   * @tparam T The type of the object
   * @param container The container the object shoulb be removed from
   * @param it The iterator representing the object
   */
  template < template <class ...> class CONT, class T >
  void erase(has_many<T, CONT> &container, const typename has_many<T, CONT>::iterator &it)
  {
    if (store().has_transaction()) {
      container.erase(it);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      container.erase(it);
      tr.commit();
    }
  }

  /**
   * @brief Deletes a range of objects from a container
   *
   * Deletes a range objects identified by the given first and
   * last iterator from a container. This leads at least to one
   * removed database entry.
   * The same changes are made to the underlying object_store
   * as well.
   *
   * @tparam CONT The type of the container
   * @tparam T The type of the object
   * @param container The container the objects should be removed from
   * @param first The iterator representing the first object
   * @param last The iterator representing the last object
   */
  template < template <class ...> class CONT, class T >
  void erase(has_many<T, CONT> &container, const typename has_many<T, CONT>::iterator &first, const typename has_many<T, CONT>::iterator &last)
  {
    if (store().has_transaction()) {
      container.erase(first, last);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      container.erase(first, last);
      tr.commit();
    }
  }

  template < template <class ...> class CONT, class T >
  void remove(has_many<T, CONT> &container, const typename has_many<T, CONT>::value_type &value)
  {
    if (store().has_transaction()) {
      container.remove(value);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      container.remove(value);
      tr.commit();
    }
  }

  /**
   * @brief Clears the complete container
   *
   * Deletes all elements from the given container.
   * The same changes are made to the underlying object_store
   * as well.
   *
   * @tparam CONT The type of the container
   * @tparam T The type of the object
   * @param container The container to be cleared
   */
  template < template <class ...> class CONT, class T >
  void clear(has_many<T, CONT> &container)
  {
    if (store().has_transaction()) {
      container.clear();
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      container.clear();
      tr.commit();
    }
  }

  /**
   * @brief Loads all tables from database.
   *
   * Loads all tables from database. All object are inserted
   * into the underlying object_store.
   */
  void load();

  /**
   * @brief Starts a transaction.
   *
   * @return The started transaction object
   */
  transaction begin();

  /**
   * @brief Return a reference to the underlaying object_store
   *
   * @return A reference to the object_store.
   */
  object_store& store();

  /**
   * @brief Return a const reference to the underlaying object_store
   *
   * @return A const reference to the object_store.
   */
  const object_store& store() const;

private:
  void load(const persistence::table_ptr &table);

private:
  class session_observer : public transaction::observer, public action_visitor
  {
  public:
    explicit session_observer(session &s);
    virtual void on_begin();
    virtual void on_commit(transaction::t_action_vector &actions);
    virtual void on_rollback();

    virtual void visit(insert_action *act);
    virtual void visit(update_action *act);
    virtual void visit(delete_action *act);
  private:
    session &session_;
  };

private:
  persistence &persistence_;

  std::shared_ptr<transaction::observer> observer_;

};

}

#endif //OOS_SESSION_HPP
