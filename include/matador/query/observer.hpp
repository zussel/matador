#ifndef MATADOR_OBSERVER_HPP
#define MATADOR_OBSERVER_HPP

#include <typeindex>

namespace matador::query {
class schema_node;

class abstract_observer {
public:
  virtual ~abstract_observer() = default;

protected:
  /**
   * Constructs a basic_object_store_observer for the
   * give std::type_index.
   *
   * @param ti type index of the observer
   */
  explicit abstract_observer(const std::type_index &ti);

public:
  /**
 * Return the type index of the observer.
 *
 * @return The type index of the observer.
 */
  [[nodiscard]] const std::type_index& index() const;

private:
  std::type_index type_index_;
};

/**
 * @class observer
 * @tparam Type Type of the observer
 * @brief Base class for typed observer classes
 *
 * When interested in observe
 * - attach (prototype_node)
 * - detach (prototype_node)
 * - insert (an object)
 * - update (an object)
 * - delete (an object)
 * actions an observer class instance must be
 * registered within the repository.
 * Use this class as a base class for all observer classes.
 */
template < class Type >
class observer : public abstract_observer {
public:
  /**
   * Default constructor
   */
  observer() : abstract_observer(std::type_index(typeid(Type))) {}

  /**
   * @brief Called on schema_node attachment
   *
   * When a prototype node is attached to the repository,
   * this is called after the attaching succeeded.
   *
   * @param node The attached schema_node
   * @param prototype The prototype object of the attached node
   */
  virtual void on_attach(const schema_node &node, const Type &prototype) const = 0;

  /**
   * @brief Called on schema_node detached
   *
   * When a prototype node is detached from the repository,
   * this is called before the detaching succeeded.
   *
   * @param node The to be detached schema_node
   * @param prototype The prototype object of the detached node
   */
  virtual void on_detach(const schema_node &node, const Type &prototype) const = 0;

  /**
   * @brief Called on object insertion.
   *
   * Called when an object is inserted
   * into the repository.
   *
   * @param obj The proxy of the inserted object.
   */
  virtual void on_insert(const Type &obj) = 0;

  /**
   * @brief Called on object update.
   *
   * Called when an object is updated
   * in the repository.
   *
   * @param obj The proxy of the updated object.
   */
  virtual void on_update(const Type &obj) = 0;

  /**
   * @brief Called on object deletion.
   *
   * Called when an object is deleted
   * from the repository.
   *
   * @param obj The proxy of the deleted object.
   */
  virtual void on_delete(const Type &obj) = 0;
};

namespace internal {
template < class Type >
class null_observer : public observer<Type> {
public:
  template < class OtherType >
  explicit null_observer(const null_observer<OtherType> *) {}
  void on_attach(const schema_node &, const Type &) const override {}
  void on_detach(const schema_node &, const Type &) const override {}
  void on_insert(Type &) override {}
  void on_update(Type &) override {}
  void on_delete(Type &) override {}
};
}
}
#endif //MATADOR_OBSERVER_HPP