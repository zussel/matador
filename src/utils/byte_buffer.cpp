#include "matador/utils/byte_buffer.hpp"

#include <cstring>
#include <cstddef>
#include <utility>
#include <vector>

namespace matador::utils {
byte_buffer::byte_buffer() {
  chunk_list_.emplace_back();
}

byte_buffer::byte_buffer(byte_buffer &&other) noexcept
    : chunk_list_(std::move(other.chunk_list_)) {
  other.ensure_non_empty();
  ensure_non_empty();
}

byte_buffer &byte_buffer::operator=(byte_buffer &&other) noexcept {
  if (this != &other) {
    chunk_list_ = std::move(other.chunk_list_);
    other.ensure_non_empty();
    ensure_non_empty();
  }
  return *this;
}

void byte_buffer::append(const void *bytes, std::size_t size) {
  if (bytes == nullptr || size == 0) {
    return;
  }

  ensure_non_empty();
  const auto *ptr = static_cast<const char *>(bytes);
  std::size_t bytes_written = 0;

  while (size > 0) {
    buffer_chunk &chunk = chunk_list_.back();

    if (const std::size_t to_write = std::min(size, chunk.available()); to_write > 0) {
      std::memcpy(&chunk.data[chunk.write_cursor], ptr + bytes_written, to_write);
      chunk.write_cursor += to_write;
      bytes_written += to_write;
      size -= to_write;
    }

    if (chunk.available() == 0 && size > 0) {
      chunk_list_.emplace_back();
    }
  }
}

void byte_buffer::append(const std::string_view str) {
  append(str.data(), str.size());
}

std::size_t byte_buffer::release(void *dest, const std::size_t size) {
  if (dest == nullptr || size == 0 || chunk_list_.empty()) {
    return 0;
  }

  auto *ptr = static_cast<char *>(dest);
  std::size_t total_read = 0;

  while (!chunk_list_.empty() && total_read < size) {
    buffer_chunk &chunk = chunk_list_.front();
    const std::size_t to_read = std::min(size - total_read, chunk.used());

    if (to_read > 0) {
      std::memcpy(ptr + total_read, &chunk.data[chunk.read_cursor], to_read);
      chunk.read_cursor += to_read;
      total_read += to_read;
    }

    // Wenn der aktuelle Chunk vollständig gelesen ist
    if (chunk.used() == 0) {
      // Wenn es der einzige Chunk ist, setzen wir lediglich die Cursor zurück (spart Allokationen)
      if (chunk_list_.size() == 1) {
        chunk.read_cursor = 0;
        chunk.write_cursor = 0;
        break;
      }
      chunk_list_.pop_front();
    }
  }

  ensure_non_empty();
  return total_read;
}

std::size_t byte_buffer::size() const noexcept {
  if (chunk_list_.empty()) {
    return 0;
  }
  return (chunk_list_.size() * CHUNK_SIZE) - chunk_list_.back().available() - chunk_list_.front().released();
}

buffer_view byte_buffer::view() const {
  std::vector<char> bytes(size());
  if (!bytes.empty()) {
    auto copy = *this;
    copy.release(bytes.data(), bytes.size());
  }
  return buffer_view(std::move(bytes));
}

bool byte_buffer::empty() const noexcept {
  return size() == 0;
}

void byte_buffer::clear() noexcept {
  chunk_list_.clear();
  chunk_list_.emplace_back();
}

void byte_buffer::ensure_non_empty() {
  if (chunk_list_.empty()) {
    chunk_list_.emplace_back();
  }
}
}
