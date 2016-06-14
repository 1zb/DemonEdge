#include "dem.hpp"

#include <iostream>
#include <array>
#include <algorithm>

#include "demo.pb.h"

#include "snappy.h"


namespace dota{
  
  // void test1(std::function<void()> func) {}

  // void test2(CDemoFileHeader m) {}

  Parser::Parser() {
    callbacks_.Connect<CDemoFileHeader>(Event::OnCDemoFileHeader, &Parser::OnCDemoFileHeader, this);
    callbacks_.Connect<CDemoFileInfo>(Event::OnCDemoFileInfo, &Parser::OnCDemoFileInfo, this);
  }

  void Parser::Open(std::string path) {
    stream_.open(path.c_str(), std::ifstream::in | std::ifstream::binary);
    if (!stream_.is_open()) {
      std::cout << "cannot open file " << path << std::endl;
      return;
    }

    const std::streampos fstart = stream_.tellg();
    stream_.seekg(0, std::ios::end);
    std::streampos fsize = stream_.tellg() - fstart;
    stream_.seekg(fstart);
    std::cout << "filesize: " << fsize << std::endl;

    // PBDEMS2\n
    std::size_t header_length = 8;
    std::array<char, 8> header;
    stream_.read(header.begin(), header_length);
    std::cout << "header: " ;
    std::for_each(header.cbegin(), header.cend(), [](const char &n){std::cout << n;});
    std::cout << std::endl;

    std::cout << "pos: " << stream_.tellg() << std::endl;

    stream_.seekg(8, std::ios::cur);
    std::cout << "pos: " << stream_.tellg() << std::endl;
  }

  //
  void Parser::Start() {
    while (stream_.good()) {
      TopMessage message = Read();
      tick_ = message.tick;
      // std::cout << message.kind << ' ' << message.tick << ' ' << stream_.tellg() << std::endl;

      CallByDemoType(message);
      if (message.kind == DEM_FileInfo) {
        // std::cout << tick_ << std::endl;
        break;
      }
    }
    std::cout << "end" << std::endl;
    // dt_classes_.output();
  }

  //
  TopMessage Parser::Read() {
    uint32_t kind = ReadVarInt();
    const bool compressed = kind & DEM_IsCompressed;
    kind = (kind & ~DEM_IsCompressed);

    uint32_t tick = ReadVarInt();
    if (tick == 4294967295) tick = 0;

    uint32_t size = ReadVarInt();

    // std::string buffer(size, '\0');
    // buffer.reserve(size);
    char* buffer = new char[100000];
    stream_.read(&buffer[0], size);
    // delete buffer;
    // std::copy_n(std::istreambuf_iterator<char>(stream_), size + 1, std::back_inserter<std::string>(buffer) );

    // stream_.seekg(size, std::ios::cur);

    TopMessage message {compressed, tick, kind, buffer, size};

    if (compressed && snappy::IsValidCompressedBuffer(buffer, size)) {
      std::size_t size_uncompressed;
      if (!snappy::GetUncompressedLength(buffer, size, &size_uncompressed)) {
        std::cout << "cannot get uncompressed length" << std::endl;
      }
      message.data.clear();
      message.data.resize(size_uncompressed);
      if (!snappy::RawUncompress(buffer, size, &message.data[0])) {
        std::cout << "uncompress unsuccesful" << std::endl;
      }
    } 
    // buffer.clear();
    delete buffer;
    return message;
  }

  uint32_t Parser::ReadVarInt() {
    char buffer;
    uint32_t count = 0;
    uint32_t result = 0;
    do {
      if (count == 5) {
        return -1;
      } else {
        buffer = stream_.get();
        result |= (uint32_t)(buffer & 0x7F) << (7 * count);
        count++;
      }
    } while (buffer & 0x80);
    return result;
  }

  //
  void Parser::CallByDemoType(const TopMessage &tmessage) {
    switch (tmessage.kind) {
      case DEM_FileHeader: {
        CDemoFileHeader event;
        // std::cout << "header " <<  tmessage.tick << std::endl;
        callbacks_.Call(Event::OnCDemoFileHeader, event, tmessage.data);
        break;
      }
      case DEM_FileInfo: {
        CDemoFileInfo event;
        // std::cout << "fileinfo " << tmessage.tick << std::endl;
        callbacks_.Call(Event::OnCDemoFileHeader, event, tmessage.data);
        break;
      }
      case DEM_Stop: {
        CDemoStop event;
        // std::cout << "stop " <<  tmessage.tick << std::endl;
        callbacks_.Call(Event::OnCDemoStop, event, tmessage.data);
        break;
      }
      default: 
        // std::cout << tick_ << std::endl;
        break;
    }
  }

  //
  void Parser::OnCDemoFileHeader(const CDemoFileHeader& message) {
    // std::cout << message.DebugString() << std::endl;
  }

  //
  void Parser::OnCDemoFileInfo(const CDemoFileInfo& message) {
    // std::cout << message.DebugString() << std::endl;
  }
}