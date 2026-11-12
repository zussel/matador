#include <catch2/catch_test_macros.hpp>

#include "matador/utils/byte_buffer.hpp"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

using namespace matador::utils;

namespace {

struct TestData {
  int32_t id{0};
  double value{0.0};
  char tag{'\0'};

  bool operator==(const TestData &other) const {
    return id == other.id && value == other.value && tag == other.tag;
  }
};

std::vector<uint8_t> generate_pattern(const size_t size, const uint8_t seed = 0) {
  std::vector<uint8_t> result(size);
  for (size_t i = 0; i < size; ++i) {
    result[i] = static_cast<uint8_t>((seed + i) & 0xFF);
  }
  return result;
}

} // namespace

TEST_CASE("Byte buffer construction and initial state", "[utils][byte_buffer]") {
  byte_buffer buffer;

  REQUIRE(buffer.empty());
  REQUIRE(buffer.size() == 0);

  char dest[10] = {0};
  REQUIRE(buffer.release(dest, sizeof(dest)) == 0);
  REQUIRE(buffer.empty());
}

TEST_CASE("Byte buffer basic append and release", "[utils][byte_buffer]") {
  byte_buffer buffer;

  SECTION("Nullptr or zero size is a no-op") {
    const char data[] = "Hello";
    buffer.append(nullptr, 5);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);

    buffer.append(data, 0);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);

    char dest[10] = {0};
    REQUIRE(buffer.release(nullptr, 5) == 0);
    REQUIRE(buffer.release(dest, 0) == 0);
  }

  SECTION("Append and release raw byte data") {
    const std::string input = "Matador Database Library";
    buffer.append(input.data(), input.size());

    REQUIRE_FALSE(buffer.empty());
    REQUIRE(buffer.size() == input.size());

    std::string output(input.size(), '\0');
    const auto read_bytes = buffer.release(output.data(), output.size());

    REQUIRE(read_bytes == input.size());
    REQUIRE(output == input);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
  }

  SECTION("Append std::string_view") {
    const std::string_view sv = "Testing string_view overload";
    buffer.append(sv);

    REQUIRE(buffer.size() == sv.size());

    std::string output(sv.size(), '\0');
    REQUIRE(buffer.release(output.data(), output.size()) == sv.size());
    REQUIRE(output == sv);
    REQUIRE(buffer.empty());
  }

  SECTION("Partial releases consume data incrementally") {
    const std::string input = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    buffer.append(input.data(), input.size());

    char part1[10] = {0};
    REQUIRE(buffer.release(part1, 10) == 10);
    REQUIRE(std::string(part1, 10) == "ABCDEFGHIJ");
    REQUIRE(buffer.size() == input.size() - 10);

    char part2[10] = {0};
    REQUIRE(buffer.release(part2, 10) == 10);
    REQUIRE(std::string(part2, 10) == "KLMNOPQRST");
    REQUIRE(buffer.size() == input.size() - 20);

    char part3[6] = {0};
    REQUIRE(buffer.release(part3, 6) == 6);
    REQUIRE(std::string(part3, 6) == "UVWXYZ");
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
  }
}

TEST_CASE("Byte buffer templated write and read", "[utils][byte_buffer]") {
  byte_buffer buffer;

  SECTION("Trivially copyable primitive types") {
    const int32_t int_val = 123456789;
    const double double_val = 3.1415926535;
    const uint8_t byte_val = 0xAF;

    buffer.write(int_val);
    buffer.write(double_val);
    buffer.write(byte_val);

    REQUIRE(buffer.size() == sizeof(int_val) + sizeof(double_val) + sizeof(byte_val));

    int32_t read_int = 0;
    double read_double = 0.0;
    uint8_t read_byte = 0;

    REQUIRE(buffer.read(read_int));
    REQUIRE(read_int == int_val);

    REQUIRE(buffer.read(read_double));
    REQUIRE(read_double == double_val);

    REQUIRE(buffer.read(read_byte));
    REQUIRE(read_byte == byte_val);

    REQUIRE(buffer.empty());
  }

  SECTION("Custom trivially copyable struct") {
    const TestData original{42, 99.99, 'M'};
    buffer.write(original);

    REQUIRE(buffer.size() == sizeof(TestData));

    TestData retrieved{};
    REQUIRE(buffer.read(retrieved));
    REQUIRE(retrieved == original);
    REQUIRE(buffer.empty());
  }

  SECTION("Read failure when insufficient data is available") {
    const uint16_t short_val = 0x1234;
    buffer.write(short_val);

    uint32_t int_val = 0;
    REQUIRE_FALSE(buffer.read(int_val));
    // The 2 available bytes were read, but could not satisfy 4 bytes
    REQUIRE(buffer.empty());
  }
}

TEST_CASE("Byte buffer chunk boundary and multi-chunk operations", "[utils][byte_buffer]") {
  constexpr size_t chunk_size = byte_buffer::CHUNK_SIZE;
  byte_buffer buffer;

  SECTION("Exact chunk size boundary") {
    const auto pattern = generate_pattern(chunk_size, 42);
    buffer.append(pattern.data(), pattern.size());

    REQUIRE(buffer.size() == chunk_size);

    std::vector<uint8_t> output(chunk_size);
    REQUIRE(buffer.release(output.data(), output.size()) == chunk_size);
    REQUIRE(output == pattern);
    REQUIRE(buffer.empty());
  }

  SECTION("Append crossing chunk boundary by one byte") {
    const size_t total_size = chunk_size + 1;
    const auto pattern = generate_pattern(total_size, 10);
    buffer.append(pattern.data(), pattern.size());

    REQUIRE(buffer.size() == total_size);

    std::vector<uint8_t> output(total_size);
    REQUIRE(buffer.release(output.data(), output.size()) == total_size);
    REQUIRE(output == pattern);
    REQUIRE(buffer.empty());
  }

  SECTION("Large payload spanning multiple chunks") {
    const size_t total_size = chunk_size * 4 + 1024; // > 65 KiB
    const auto pattern = generate_pattern(total_size, 7);
    buffer.append(pattern.data(), pattern.size());

    REQUIRE(buffer.size() == total_size);

    std::vector<uint8_t> output(total_size);
    REQUIRE(buffer.release(output.data(), output.size()) == total_size);
    REQUIRE(output == pattern);
    REQUIRE(buffer.empty());
  }

  SECTION("Releasing in irregular chunk sizes across boundaries") {
    const size_t total_size = chunk_size * 3 + 500;
    const auto pattern = generate_pattern(total_size, 1);
    buffer.append(pattern.data(), pattern.size());

    std::vector<uint8_t> output(total_size);
    size_t offset = 0;
    const size_t step = 4099; // Prime-like step crossing chunk boundaries

    while (offset < total_size) {
      const size_t to_read = std::min(step, total_size - offset);
      const size_t bytes_read = buffer.release(output.data() + offset, to_read);
      REQUIRE(bytes_read == to_read);
      offset += bytes_read;
    }

    REQUIRE(offset == total_size);
    REQUIRE(output == pattern);
    REQUIRE(buffer.empty());
  }
}

TEST_CASE("Byte buffer underflow protection", "[utils][byte_buffer]") {
  byte_buffer buffer;

  SECTION("Release more bytes than available in single chunk") {
    const std::string data = "Short";
    buffer.append(data.data(), data.size());

    char dest[20] = {0};
    const auto read_bytes = buffer.release(dest, 20);

    REQUIRE(read_bytes == data.size());
    REQUIRE(std::string(dest, read_bytes) == data);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);

    // Further releases return 0
    REQUIRE(buffer.release(dest, 10) == 0);
  }

  SECTION("Release more bytes than available across multiple chunks") {
    constexpr size_t total_size = byte_buffer::CHUNK_SIZE + 500;
    const auto pattern = generate_pattern(total_size, 99);
    buffer.append(pattern.data(), pattern.size());

    std::vector<uint8_t> dest(total_size + 1000, 0);
    const auto read_bytes = buffer.release(dest.data(), dest.size());

    REQUIRE(read_bytes == total_size);
    dest.resize(read_bytes);
    REQUIRE(dest == pattern);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
  }
}

TEST_CASE("Byte buffer FIFO ordering and stream simulation", "[utils][byte_buffer]") {
  byte_buffer buffer;

  SECTION("Interleaved writes and reads") {
    const auto block_a = generate_pattern(500, 1);
    const auto block_b = generate_pattern(800, 2);
    const auto block_c = generate_pattern(1200, 3);

    buffer.append(block_a.data(), block_a.size());
    REQUIRE(buffer.size() == 500);

    std::vector<uint8_t> read_a(300);
    REQUIRE(buffer.release(read_a.data(), 300) == 300);
    REQUIRE(std::equal(read_a.begin(), read_a.end(), block_a.begin()));
    REQUIRE(buffer.size() == 200);

    buffer.append(block_b.data(), block_b.size());
    REQUIRE(buffer.size() == 1000);

    std::vector<uint8_t> remaining_a(200);
    REQUIRE(buffer.release(remaining_a.data(), 200) == 200);
    REQUIRE(std::equal(remaining_a.begin(), remaining_a.end(), block_a.begin() + 300));
    REQUIRE(buffer.size() == 800);

    buffer.append(block_c.data(), block_c.size());
    REQUIRE(buffer.size() == 2000);

    std::vector<uint8_t> read_bc(2000);
    REQUIRE(buffer.release(read_bc.data(), 2000) == 2000);

    std::vector<uint8_t> expected_bc;
    expected_bc.insert(expected_bc.end(), block_b.begin(), block_b.end());
    expected_bc.insert(expected_bc.end(), block_c.begin(), block_c.end());

    REQUIRE(read_bc == expected_bc);
    REQUIRE(buffer.empty());
  }

  SECTION("Cursor recycling in single chunk") {
    // When a single chunk is fully read, cursors are reset to 0, avoiding chunk reallocation
    for (int cycle = 0; cycle < 10; ++cycle) {
      const auto data = generate_pattern(1000, static_cast<uint8_t>(cycle));
      buffer.append(data.data(), data.size());
      REQUIRE(buffer.size() == 1000);

      std::vector<uint8_t> out(1000);
      REQUIRE(buffer.release(out.data(), 1000) == 1000);
      REQUIRE(out == data);
      REQUIRE(buffer.empty());
    }
  }

  SECTION("Stream producer-consumer simulation across multiple chunks") {
    constexpr size_t total_stream_bytes = byte_buffer::CHUNK_SIZE * 5;
    const auto full_stream = generate_pattern(total_stream_bytes, 123);

    std::vector<uint8_t> received_stream;
    received_stream.reserve(total_stream_bytes);

    size_t produced = 0;
    const size_t write_chunk = 5000;
    const size_t read_chunk = 3000;

    std::vector<uint8_t> temp_read(read_chunk);

    while (produced < total_stream_bytes || !buffer.empty()) {
      if (produced < total_stream_bytes) {
        const size_t to_write = std::min(write_chunk, total_stream_bytes - produced);
        buffer.append(full_stream.data() + produced, to_write);
        produced += to_write;
      }

      if (!buffer.empty()) {
        const size_t read_bytes = buffer.release(temp_read.data(), read_chunk);
        received_stream.insert(received_stream.end(), temp_read.begin(), temp_read.begin() + read_bytes);
      }
    }

    REQUIRE(received_stream.size() == total_stream_bytes);
    REQUIRE(received_stream == full_stream);
    REQUIRE(buffer.empty());
  }
}

TEST_CASE("Byte buffer clear functionality", "[utils][byte_buffer]") {
  byte_buffer buffer;

  SECTION("Clear on empty buffer") {
    buffer.clear();
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
  }

  SECTION("Clear on single-chunk buffer") {
    buffer.append("Temporary data", 14);
    REQUIRE(buffer.size() == 14);

    buffer.clear();
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);

    buffer.append("New data", 8);
    REQUIRE(buffer.size() == 8);

    char dest[8];
    REQUIRE(buffer.release(dest, 8) == 8);
    REQUIRE(std::string(dest, 8) == "New data");
  }

  SECTION("Clear on multi-chunk buffer") {
    const auto large_data = generate_pattern(byte_buffer::CHUNK_SIZE * 3, 5);
    buffer.append(large_data.data(), large_data.size());
    REQUIRE(buffer.size() == large_data.size());

    buffer.clear();
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);

    const std::string fresh = "Fresh Start";
    buffer.append(fresh.data(), fresh.size());
    REQUIRE(buffer.size() == fresh.size());

    char dest[11];
    REQUIRE(buffer.release(dest, 11) == 11);
    REQUIRE(std::string(dest, 11) == fresh);
  }
}

TEST_CASE("Byte buffer copy semantics", "[utils][byte_buffer]") {
  const auto pattern = generate_pattern(byte_buffer::CHUNK_SIZE + 500, 33);

  byte_buffer original;
  original.append(pattern.data(), pattern.size());

  SECTION("Copy constructor") {
    byte_buffer copy(original);

    REQUIRE(copy.size() == original.size());
    REQUIRE_FALSE(copy.empty());

    // Modify original
    char dummy[100];
    original.release(dummy, sizeof(dummy));
    REQUIRE(original.size() == pattern.size() - 100);
    REQUIRE(copy.size() == pattern.size());

    std::vector<uint8_t> output(pattern.size());
    REQUIRE(copy.release(output.data(), output.size()) == pattern.size());
    REQUIRE(output == pattern);
    REQUIRE(copy.empty());
  }

  SECTION("Copy assignment") {
    byte_buffer copy;
    copy.append("Old content", 11);

    copy = original;
    REQUIRE(copy.size() == pattern.size());

    // Self assignment check
    copy = *&copy;
    REQUIRE(copy.size() == pattern.size());

    std::vector<uint8_t> output(pattern.size());
    REQUIRE(copy.release(output.data(), output.size()) == pattern.size());
    REQUIRE(output == pattern);
    REQUIRE(copy.empty());
  }
}

TEST_CASE("Byte buffer move semantics", "[utils][byte_buffer]") {
  const auto pattern = generate_pattern(byte_buffer::CHUNK_SIZE + 200, 77);

  SECTION("Move constructor") {
    byte_buffer source;
    source.append(pattern.data(), pattern.size());

    byte_buffer target(std::move(source));

    REQUIRE(target.size() == pattern.size());
    REQUIRE_FALSE(target.empty());

    // Moved-from buffer must remain valid and empty
    REQUIRE(source.empty());
    REQUIRE(source.size() == 0);

    // Source can still be used safely
    source.append("Reused", 6);
    REQUIRE(source.size() == 6);
    char src_dest[6];
    REQUIRE(source.release(src_dest, 6) == 6);
    REQUIRE(std::string(src_dest, 6) == "Reused");

    // Target has correct data
    std::vector<uint8_t> output(pattern.size());
    REQUIRE(target.release(output.data(), output.size()) == pattern.size());
    REQUIRE(output == pattern);
    REQUIRE(target.empty());
  }

  SECTION("Move assignment") {
    byte_buffer source;
    source.append(pattern.data(), pattern.size());

    byte_buffer target;
    target.append("Existing", 8);

    target = std::move(source);

    REQUIRE(target.size() == pattern.size());

    // Moved-from buffer must remain valid and empty
    REQUIRE(source.empty());
    REQUIRE(source.size() == 0);

    // Self-move assignment safety
    target = std::move(*&target);
    REQUIRE(target.size() == pattern.size());

    std::vector<uint8_t> output(pattern.size());
    REQUIRE(target.release(output.data(), output.size()) == pattern.size());
    REQUIRE(output == pattern);
    REQUIRE(target.empty());
  }
}
