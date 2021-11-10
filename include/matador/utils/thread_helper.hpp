#ifndef MATADOR_THREAD_HELPER_HPP
#define MATADOR_THREAD_HELPER_HPP

#include <thread>

namespace matador {

std::size_t acquire_thread_index(std::thread::id id);

}

#endif //MATADOR_THREAD_HELPER_HPP
