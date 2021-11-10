#include "matador/utils/thread_helper.hpp"

#include <mutex>
#include <map>

namespace matador {
std::size_t acquire_thread_index(std::thread::id id)
{
  static std::size_t nextindex = 0;
  static std::mutex my_mutex;
  static std::map<std::thread::id, std::size_t> ids;
  std::lock_guard<std::mutex> lock(my_mutex);
  if(ids.find(id) == ids.end())
    ids[id] = nextindex++;
  return ids[id];}
}
