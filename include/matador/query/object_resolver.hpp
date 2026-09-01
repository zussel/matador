#ifndef MATADOR_OBJECT_RESOLVER_HPP
#define MATADOR_OBJECT_RESOLVER_HPP

#include "matador/query/abstract_type_resolver.hpp"

#include <memory>

namespace matador::query {
class identifier;
template<typename Type>
class object_resolver : public abstract_type_resolver {
public:
    object_resolver() : abstract_type_resolver(typeid(Type)) {}

    virtual std::shared_ptr<Type> resolve(const identifier& id) = 0;
};
}
#endif //MATADOR_OBJECT_RESOLVER_HPP
