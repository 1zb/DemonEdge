#include "reader.hpp"

namespace dota{
  uint32_t Reader::ReadBits(const std::size_t n) {
    //
    if (n > size_ - pos_) {
      // TODO
    }

    //
    if (n > 32) {
      // TODO
    }

    const uint32_t bit_size = sizeof(word_t) * 8;
    const uint32_t start    = pos_ / bit_size;
    const uint32_t end      = (pos_ + n - 1) / bit_size;
    const uint32_t s        = (pos_ % bit_size);
    
    uint32_t ret;
    if (start == end) {
      ret = (data_[start] >> s) & ((1 << (uint32_t)n) - 1);
    } else {
      ret = ((data_[start] >> s) | (data_[end] << (bit_size - s)))
          & ((1 << (uint32_t)n) - 1);
    }

    pos_ += n;
    return ret;
  }

  //
  void Reader::ReadBytes(char* buffer, const std::size_t n) {
    for (std::size_t i = 0; i < n; i++) {
      buffer[i] = static_cast<char>(ReadBits(8));
      // if (buffer[i] == '\0') return;
    }
  }
}