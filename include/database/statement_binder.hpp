#ifndef STATEMENT_BINDER_HPP
#define STATEMENT_BINDER_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include "object/object_atomizer.hpp"

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace oos {

class statement_impl;
class object;

/// @cond OOS_DEV

class statement_binder : public object_writer
{
public:
  typedef std::tr1::shared_ptr<statement_impl> statement_impl_ptr;

public:
  statement_binder();
  virtual ~statement_binder();

  void bind(const statement_impl_ptr &stmt, object *o, bool bind_id);
  
  /**
   * @brief Write a single character to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, char x);

  /**
   * @brief Write a float to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, float x);

  /**
   * @brief Write a double to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, double x);

  /**
   * @brief Write a short to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, short x);

  /**
   * @brief Write a int to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, int x);

  /**
   * @brief Write a long to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, long x);

  /**
   * @brief Write an unsigned short to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned char x);

  /**
   * @brief Write an unsigned short to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned short x);

  /**
   * @brief Write an unsigned int to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned int x);

  /**
   * @brief Write an unsigned long to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned long x);

  /**
   * @brief Write a bool to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, bool x);

  /**
   * @brief Write a const character string to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const char *x, int s);

  /**
   * @brief Write a std::string to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const std::string &x);

  /**
   * @brief Write a varchar_base to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void write(const char *id, const varchar_base &x);

  /**
   * @brief Write an object_base_ptr to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement. In this case the id of the
   * foreign object is written to the column.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const object_base_ptr &x);

  /**
   * @fn virtual void write(const char *id, const object_container &x)
   * @brief Write a object_container to the atomizer.
   * 
   * Write a object_container to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, const object_container&);

private:
  statement_impl_ptr stmt_;
  int column_;
};

///@endcond

}

#endif /* STATEMENT_BINDER_HPP */
