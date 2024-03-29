#ifndef OOS_SESSION_HPP
#define OOS_SESSION_HPP

#include "matador/orm/export.hpp"

#include "matador/object/container.hpp"
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
   * Inserts an object of type T into the
   * object_store without writing it to the database.
   * It returns an object_ptr of the inserted object.
   *
   * @tparam T Type of the object to be inserted
   * @param obj Object to be inserted
   * @return object_ptr of the inserted object
   */
  template < class T >
  object_ptr<T> insert(T *obj)
  {
    return persistence_.store().insert(obj);
  }

  /**
   * Inserts an object of type Type into the object_store.
   * On successful insertion an object_ptr element with the
   * inserted object is returned.
   *
   * @tparam Type Type of the object to insert
   * @tparam Args Argument types of the constructor arguments
   * @param args Arguments to construct the object
   * @return Inserted object contained by an object_ptr on success.
   */
  template< class Type, typename... Args >
  object_ptr<Type> insert(Args&&... args) {
    return insert(new Type(std::forward<Args>(args)...));
  }

  /**
   * Remove object from object store
   * @tparam T Type of object to be removed
   * @param optr Object to be removed
   */
  template < class T >
  void remove(object_ptr<T> &optr)
  {
    persistence_.store().remove(optr);
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
   * Gets a certain object identified by
   * its primary key and its type.
   *
   * @tparam T Type of the object to get
   * @tparam V Type of the primary key
   * @param id Primary key value
   * @return The found object or an empty object
   */
  template < class T, class V >
  object_ptr<T> get(V id)
  {
    auto pk = identifier{id};

    return store().get<T>(pk);
  }

  /**
   * @brief Saves an object to the database
   *
   * @tparam T Type of object to save
   * @param obj Object to save
   * @return Saved object
   */
  template < class T >
  object_ptr<T> save(T *obj)
  {
    if (store().has_transaction()) {
      return store().insert(obj);
    } else {
      auto optr = insert(obj);
      flush();
      return optr;
    }
  }

  /**
   * @brief Save an object on database.
   *
   * @tparam T Type of object to save
   * @param obj Object to save
   * @return Saved object
   */
  template < class T >
  object_ptr<T> save(const object_ptr<T> &obj)
  {
    if (store().has_transaction()) {
      store().current_transaction().on_update(obj.proxy_);
    } else {
      persistence_.store().mark_modified(obj);
      flush();
    }
    return obj;
  }

  /**
   * Flushes all stored changes to the database
   */
  void flush();

  /**
   * @brief Loads all tables from database.
   *
   * Loads all tables from database. All object are inserted
   * into the underlying object_store.
   */
  void load();

  /**
   * @brief Load a specific table by given name
   *
   * @param name of the table to load
   */
  void load(const std::string &name);

  /**
   * @brief Load a specific table by given type
   *
   * @tparam T type of the table to load
   */
  template < class T >
  void load()
  {
    load(typeid(T).name());
  }

  /**
   * @brief Starts a transaction.
   *
   * @return The started transaction object
   */
  transaction begin();

  /**
   * @brief Return a reference to the underlying object_store
   *
   * @return A reference to the object_store.
   */
  object_store& store();

  /**
   * @brief Return a const reference to the underlying object_store
   *
   * @return A const reference to the object_store.
   */
  const object_store& store() const;

private:
  void load(const persistence::table_ptr &table);

  void load(const prototype_node &node);

private:
  class session_transaction_observer : public transaction::observer, public action_visitor
  {
  public:
    explicit session_transaction_observer(session &s);
    void on_begin() override;
    void on_commit(transaction::t_action_vector &actions) override;
    void on_rollback() override;

    void visit(insert_action *act) override;
    void visit(update_action *act) override;
    void visit(delete_action *act) override;
  private:
    session &session_;
  };

private:
  persistence &persistence_;

  std::shared_ptr<transaction::observer> observer_;

};

}

#endif //OOS_SESSION_HPP
