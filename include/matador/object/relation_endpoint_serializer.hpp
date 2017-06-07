//
// Created by sascha on 6/2/17.
//

#ifndef MATADOR_RELATION_ENDPOINT_SERIALIZER_HPP
#define MATADOR_RELATION_ENDPOINT_SERIALIZER_HPP

namespace matador {
namespace detail {

template < class Value >
class relation_endpoint_serializer
{
public:
  template < class Owner >
  void set(const object_ptr<Owner> &/*owner*/, const std::string &/*field*/, const object_ptr<Value> &/*value*/) {}
};

}
}
#endif //MATADOR_RELATION_ENDPOINT_SERIALIZER_HPP
