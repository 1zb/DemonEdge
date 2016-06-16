#ifndef _READER_HPP_
#define _READER_HPP_

#include <string>
#include <algorithm>
#include <vector>
#include <cstring>

namespace dota {

class Reader {
 public:
  using word_t = uint64_t;

  // default constructor
  Reader() : data_(), pos_(0), size_(0) {}

  // copy constructor
  Reader(const Reader& r) : data_(r.data_), pos_(r.pos_), size_(r.size_) {}

  // copy assignment
  Reader& operator=(const Reader&) = delete;

  // move constructor
  Reader(Reader&&) = delete;

  // move assignment
  Reader& operator=(Reader&&) = delete;

  // init from std::string
  Reader(const std::string &str) : data_(), pos_(0), size_(str.size()*8) {
    // 64
    data_.resize((str.size() + 3) / 4 + 1);
    std::memcpy(&data_[0], str.c_str(), str.size());
    // std::copy(str.begin(), str.end(), &data_[0]);
    // std::for_each(data_.cbegin(), data_.cend(), [](const word_t &n){std::cout << n << std::endl;});
  }

  //
  ~Reader() = default;

  //
  inline bool good() const {return pos_ < size_;}

  //
  inline std::size_t end() const {return size_;}

  //
  inline std::size_t position() const {return pos_;}

  //
  uint32_t ReadBits(const std::size_t n);

  //
  void ReadBits(std::string& dest, std::size_t n);
  // bytes
  void ReadBytes(char* buffer, const std::size_t n);

  // string
  std::string ReadString();
  
  // bit varint
  uint32_t ReadUBitVar();

  // varint
  uint32_t ReadVarInt();

  // boolean
  bool ReadBool();

 private:
  std::vector<word_t> data_;
  std::size_t pos_;
  std::size_t size_;
};

}

#endif  // SRC_DEMONEDGE_READER_HPP_