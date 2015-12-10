//
// Created by sascha on 12/10/15.
//

#ifndef OOS_BASIC_OBJECT_HOLDER_HPP
#define OOS_BASIC_OBJECT_HOLDER_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif


namespace oos {

/**
 * @class basic_object_holder
 * @brief Base class for the serializable pointer and reference class
 *
 * This is the base class for the serializable pointer
 * and reference class. The class holds the proxy
 * of the serializable and the id of the serializable.
 */
class OOS_API basic_object_holder
{
protected:
  /**
   * @brief Creates and empty base pointer.
   * 
   * Creates and empty base pointer. The boolean
   * tells the class if the serializable is handled
   * as a reference or an pointer. The difference
   * is that the reference couldn't be deleted
   * from the object_store and the pointer can.
   * 
   * @param is_ref If true the serializable is handled as a reference.
   */
  explicit basic_object_holder(bool is_ref);

  /**
   * Copies from another basic_object_holder
   * 
   * @param x the basic_object_holder to copy from.
   */
  basic_object_holder(const basic_object_holder &x);

  /**
   * Assign operator.
   * 
   * @param x The basic_object_holder to assign from.
   */
  basic_object_holder& operator=(const basic_object_holder &x);

  /**
   * @brief Creates an basic_object_holder with a given object_proxy
   * 
   * Creates an basic_object_holder with a given object_proxy. The
   * boolean tells the basic_object_holder if it should be
   * handled as a reference or a pointer.
   * 
   * @param op The object_proxy of the basic_object_holder
   * @param is_ref If true the serializable is handled as a reference.
   */
  basic_object_holder(object_proxy *op, bool is_ref);

  /**
   * Destroys the basic_object_holder
   * and decides wether the underlaying
   * object_proxy is destroyed as well.
   *
   * It is destroyed if it is not inserted
   * into any object_store.
   */
  ~basic_object_holder();

public:

  /**
   * Equal to operator for the basic_object_holder
   * 
   * @param x The basic_object_holder to check equality with.
   */
  bool operator==(const basic_object_holder &x) const;

  /**
   * Not equal to operator for the basic_object_holder
   * 
   * @param x The basic_object_holder to check unequality with.
   */
  bool operator!=(const basic_object_holder &x) const;

  /**
   * Resets the basic_object_holder with the given object_proxy.
   * 
   * @param proxy The new object_proxy for the basic_object_holder.
   * @param is_ref Indicates if the given object_proxy is a reference.
   */
  void reset(object_proxy *proxy = 0, bool is_ref = false);

  /**
   * Resets the basic_object_holder with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  void reset(const std::shared_ptr<basic_identifier> &id);

  /**
   * Returns if the serializable is loaded.
   * 
   * @return True if the serializable is loaded.
   */
  bool is_loaded() const;

  /**
   * Returns the serializable id.
   * 
   * @return The id of the serializable.
   */
  unsigned long id() const;

  /**
   * Sets the serializable id. If a proxy
   * is set an exception is thrown.
   * 
   * @param i The new serializable id
   */
  void id(unsigned long i);

  /**
   * Returns the corresponding
   * object_store or nullptr
   */
  object_store* store() const;

  /**
   * Returns the serializable
   * 
   * @return The serializable.
   */
  void* ptr();

  /**
   * Returns the serializable
   *
   * @return The serializable.
   */
  const void* ptr() const;

  /**
   * Returns the serializable
   * 
   * @return The serializable.
   */
  void* lookup_object();

  /**
   * Returns the serializable
   *
   * @return The serializable.
   */
  void* lookup_object() const;

  /**
   * Returns if the serializable is treated as a reference.
   * 
   * @return True if the serializable is treated like a reference.
   */
  virtual bool is_reference() const;

  /**
   * Returns if this basic_object_holder is inside
   * of the object_store. This is important
   * to calculate the reference and pointer
   * counter.
   * 
   * @return True if the basic_object_holder internal
   */
  bool is_internal() const;

  /**
   * Returns the reference count.
   * 
   * @return The reference count.
   */
  unsigned long ref_count() const;

  /**
   * Returns the pointer count.
   * 
   * @return The pointer count.
   */
  unsigned long ptr_count() const;

  /**
   * Returns true if serializable has a primary key
   *
   * @return true if serializable has a primary key
   */
  bool has_primary_key() const;

  /**
   * Gets the primary key of the foreign serializable
   *
   * @return The primary key of the foreign serializable
   */
  std::shared_ptr<basic_identifier> primary_key() const;

  /**
   * Prints the underlaying serializable
   *
   * @param out The output stream to write on.
   * @param x The serializable pointer to print.
   * @return The output stream.
   */
  friend OOS_API std::ostream& operator<<(std::ostream &out, const basic_object_holder &x);

private:
  friend class object_serializer;
  friend class object_proxy;
  friend class object_deleter;
  friend class object_inserter;
  friend class object_store;
  friend class object_container;

  // Todo: change interface to remove friend
  friend class session;
  // Todo: replace private access of proxy with call to reset
  friend class table_reader;

  template < class T, bool TYPE > friend class object_holder;
  template < class T > friend class has_one;

  object_proxy *proxy_ = nullptr;
  bool is_reference_ = false;
  bool is_internal_ = false;
  unsigned long oid_ = 0;
};

}

#endif //OOS_BASIC_OBJECT_HOLDER_HPP
