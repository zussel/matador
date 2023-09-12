#include <algorithm>
#include <mutex>
#include <memory>
#include <unordered_map>

class connection {};

template < class ConnectionType >
class connection_pool;

template < class ConnectionType >
class connection_pointer
{
public:
  connection_pointer(connection_pool<ConnectionType> &pool, ConnectionType *connection)
  : pool_(pool)
  , connection_(connection)
  {}
  connection_pointer(connection_pointer &&x) noexcept
  : pool_(std::move(x.pool_))
  {}
  connection_pointer& operator=(connection_pointer &&x) noexcept {
    pool_ = std::move(x.pool_);
    return *this;
  }

  ~connection_pointer();

  ConnectionType* operator->() { return &connection_; }
  ConnectionType& operator*() { return connection_; }

  bool valid() const { return connection_ != nullptr; }

private:
  connection_pool<ConnectionType> &pool_;
  ConnectionType *connection_{nullptr};
};


template < class ConnectionType >
class connection_pool
{
public:
  using connection_ptr = connection_pointer<ConnectionType>;

  connection_ptr acquire_connection();
  void release_connection(connection_ptr c);
private:
  using connection_map = std::unordered_map<ConnectionType*, std::unique_ptr<ConnectionType>>;

  std::mutex mutex_;
  connection_map busy_connections_;
  connection_map idle_connections_;
};

template<class ConnectionType>
connection_pointer<ConnectionType>::~connection_pointer()
{
  pool_.release_connection(*this);
}

template<class ConnectionType>
typename connection_pool<ConnectionType>::connection_ptr connection_pool<ConnectionType>::acquire_connection() {
  return connection_pool::connection_ptr(connection_pool());
}

template<class ConnectionType>
void connection_pool<ConnectionType>::release_connection(connection_pool::connection_ptr c) {

}

int main()
{

}