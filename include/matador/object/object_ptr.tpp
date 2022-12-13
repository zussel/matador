#include "matador/object/object_ptr.hpp"
#include "matador/object/object_store.hpp"

namespace matador {

template < class T, object_holder_type OPT >
T* object_pointer<T, OPT>::modify() {
  if (proxy_ && proxy_->obj()) {
    if (proxy_->object_type_entry_ && proxy_->object_type_entry_->store()) {
      proxy_->object_type_entry_->store()->mark_modified(proxy_);
    }
    return (T*)proxy_->obj();
  } else {
    return nullptr;
  }
}

}