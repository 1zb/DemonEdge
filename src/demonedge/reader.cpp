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
  void Reader::ReadBits(std::string& dest, std::size_t n) {
    dest.resize(0);
    while (n > 7) {
      dest += static_cast<char>(ReadBits(8));
      n -= 8;
    }
    if (n != 0) {
      dest += static_cast<char>(ReadBits(n));
    }
  }

  //
  void Reader::ReadBytes(char* buffer, const std::size_t n) {
    for (std::size_t i = 0; i < n; i++) {
      buffer[i] = static_cast<char>(ReadBits(8));
      // if (buffer[i] == '\0') return;
    }
  }

  //
  std::string Reader::ReadString() {
    std::string str;
    std:;size_t n = 0x400;
    for (std::size_t i = 0; i < n; i++) {
      char c = static_cast<char>(ReadBits(8));
      if (c == '\0') {
        break;
      }
      str += c;
    }
    return str;
  }
  //
  uint32_t Reader::ReadUBitVar() {
    uint32_t ret = ReadBits(6);

    switch (ret & 48) {
      case 16: {
        ret = (ret & 15) | (ReadBits(4) << 4);
        break;
      }
      case 32: {
        ret = (ret & 15) | (ReadBits(8) << 4);
        break;
      }
      case 48: {
        ret = (ret & 15) | (ReadBits(28) << 4);
        break;
      }
    }
    return ret;
  }

  //
  uint32_t Reader::ReadVarInt() {
    uint32_t buffer;
    uint32_t count = 0;
    uint32_t result = 0;
    do {
      if (count == 5) {
        return -1;
      } else {
        buffer = ReadBits(8);
        result |= (uint32_t)(buffer & 0x7F) << (7 * count);
        count++;
      }
    } while (buffer & 0x80);
    return result;
  }

  bool Reader::ReadBool() {
    return (ReadBits(1) != 0);
  }

}