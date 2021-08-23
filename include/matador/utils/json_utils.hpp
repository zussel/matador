#ifndef MATADOR_JSON_UTILS_HPP
#define MATADOR_JSON_UTILS_HPP

#include <ostream>
#include <map>

namespace matador {

/**
 * Joins a range of elements as string within a list
 * with a given delimiter and writes it to the
 * given stream
 *
 * @tparam R Type of the range (e.g. map, list, vector, etc)
 * @param range The range with the elements to join
 * @param out The stream to write on
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class R >
std::ostream& join(R &range, std::ostream &out, const std::string &delim)
{
  if (range.size() < 2) {
    for (auto &i : range) {
      out << i;
    }
  } else {
    auto it = range.begin();
    out << *it++;
    for (;it != range.end(); ++it) {
      out << delim << *it;
    }
  }
  return out;
}

/**
 * Joins a range of elements as string within a list
 * with a given delimiter and writes it to the
 * given stream
 *
 * @tparam R Type og the range (e.g. map, list, vector, etc)
 * @param range The range with the elements to join
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class R >
std::string join(R &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (const auto &i : range) {
      result += to_string(i);
    }
  } else {
    auto it = range.begin();
    result += to_string(*it++);
    for (;it != range.end(); ++it) {
      result += delim + to_string(*it);
    }
  }
  return result;
}

/**
 * Joins a map of key value pairs as string within
 * a list with a given delimiter and writes it to
 * the given stream.
 *
 * The key value pairs will be written in a
 * json style: ("key": value)
 *
 * @tparam K Type of key
 * @tparam V Type of value
 * @param range The range with the elemets to join
 * @param out The stream to write on
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class K, class V >
std::ostream& join(std::map<K, V> &range, std::ostream &out, const std::string &delim)
{
  if (range.size() < 2) {
    for (auto &i : range) {
      out << "\"" << i.first << "\": " << i.second;
    }
  } else {
    auto it = range.begin();
    out << "\"" << it->first << "\": " << it->second;
    for (++it ;it != range.end(); ++it) {
      out << delim << "\"" << it->first << "\": " << it->second;
    }
  }
  return out;
}

/**
 * Joins a map of key value pairs as string within
 * a list with a given delimiter and writes it to
 * the given stream.
 *
 * The key value pairs will be written in a
 * json style: ("key": value)
 *
 * @tparam K Type of key
 * @tparam V Type of value
 * @param range The range with the elemets to join
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class K, class V >
std::string join(std::map<K, V> &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (auto &i : range) {
      result += "\"" + i.first + "\": " + to_string(i.second);
    }
  } else {
    auto it = range.begin();
    result += "\"" + it->first + "\": " + to_string(it->second);
    for (++it ;it != range.end(); ++it) {
      result += delim + "\"" + it->first + "\": " + to_string(it->second);
    }
  }
  return result;
}

}

#endif //MATADOR_JSON_UTILS_HPP
