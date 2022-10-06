#ifndef MATADOR_THREAD_HELPER_HPP
#define MATADOR_THREAD_HELPER_HPP

#include "matador/utils/export.hpp"

#include <thread>

namespace matador {

/**
 * Returns the current unique number for the given
 * thread id.
 *
 * @param id Thread id for which a unique is acquired.
 * @return The thread number.
 */
OOS_UTILS_API std::size_t acquire_thread_index(std::thread::id id);

}

#endif //MATADOR_THREAD_HELPER_HPP
