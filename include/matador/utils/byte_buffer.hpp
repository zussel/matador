#ifndef MATADOR_BYTE_BUFFER_HPP
#define MATADOR_BYTE_BUFFER_HPP

#include "matador_export.h"

#include <array>
#include <cstddef>
#include <list>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace matador::utils {
class buffer_view {
public:
  buffer_view() = default;
  explicit buffer_view(std::vector<char> bytes)
  : bytes_(std::make_shared<const std::vector<char>>(std::move(bytes))) {}

  [[nodiscard]] const char *data() const noexcept {
    return bytes_ ? bytes_->data() : nullptr;
  }

  [[nodiscard]] std::size_t size() const noexcept {
    return bytes_ ? bytes_->size() : 0;
  }

  [[nodiscard]] bool empty() const noexcept {
    return size() == 0;
  }

  [[nodiscard]] const char *begin() const noexcept {
    return data();
  }

  [[nodiscard]] const char *end() const noexcept {
    return data() ? data() + size() : nullptr;
  }

  [[nodiscard]] std::string_view as_string_view() const noexcept {
    return bytes_ ? std::string_view(bytes_->data(), bytes_->size()) : std::string_view{};
  }

private:
  std::shared_ptr<const std::vector<char>> bytes_;
};

/**
 * @class byte_buffer
 * @brief FIFO buffer for byte streams organized in fixed-size chunks.
 * 
 * This class provide a buffer for bytes. The
 * bytes are added in chunks. Once the current chunk
 * of bytes is full a new empty chunk is added.
 * The bytes are appended and released from the end
 * of the buffer.
 * It is used by the object_store to serialize objects.
 */
class MATADOR_EXPORT byte_buffer {
public:
  static constexpr std::size_t CHUNK_SIZE = 1 << 14; // 16 KiB

  /**
   * @brief Create an empty buffer.
   * 
   * Create an empty buffer. One empty chunk is
   * also created.
   */
  byte_buffer();
  ~byte_buffer() = default;

  byte_buffer(const byte_buffer &) = default;
  byte_buffer &operator=(const byte_buffer &) = default;
  byte_buffer(byte_buffer &&other) noexcept;
  byte_buffer &operator=(byte_buffer &&other) noexcept;

  /**
   * @brief Append an amount of bytes.
   * 
   * An amount of bytes is added to the buffer.
   * 
   * @param bytes The pointer to the bytes to add.
   * @param size The size of the bytes to add.
   */
  void append(const void *bytes, std::size_t size);

  void append(std::string_view str);

  template <typename Type, typename = std::enable_if_t<std::is_trivially_copyable_v<Type>>>
  void write(const Type &value) {
    append(&value, sizeof(Type));
  }

  /**
   * @brief Release a number of bytes from beginning of the buffer.
   * 
   * A number of bytes is released. The released bytes
   * are removed from the beginning of the buffer.
   * 
   * @param dest The address of the destination memory where the bytes should be written to.
   * @param size The number of bytes released from the buffer.
   * @return Released number of bytes
   */
  std::size_t release(void *dest, std::size_t size);

  template <typename Type, typename = std::enable_if_t<std::is_trivially_copyable_v<Type>>>
  bool read(Type &value) {
    return release(&value, sizeof(Type)) == sizeof(Type);
  }

  /**
   * Return the size of the buffer.
   *
   * @return The size of the buffer
   */
  [[nodiscard]] std::size_t size() const noexcept;

  /**
   * Return an immutable, independently owned view of the current bytes.
   */
  [[nodiscard]] buffer_view view() const;

  /**
   * Returns true if the buffer is empty.
   *
   * @return True if buffer is empty
   */
  [[nodiscard]] bool empty() const noexcept;

  /**
   * Clear the buffer.
   */
  void clear() noexcept;

private:
  struct buffer_chunk {
    buffer_chunk() : read_cursor(0), write_cursor(0) {}
    [[nodiscard]] std::size_t available() const { return CHUNK_SIZE - write_cursor; }
    [[nodiscard]] std::size_t used() const { return write_cursor - read_cursor; }
    [[nodiscard]] std::size_t released() const { return read_cursor; }
    std::array<char, CHUNK_SIZE> data{};
    std::size_t read_cursor{0};
    std::size_t write_cursor{0};
  };

  void ensure_non_empty();

  std::list<buffer_chunk> chunk_list_;
};
}

#endif /* MATADOR_BYTE_BUFFER_HPP */
