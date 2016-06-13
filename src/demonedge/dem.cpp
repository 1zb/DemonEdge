#include "dem.hpp"

#include <iostream>
#include <array>
#include <algorithm>

#include "demo.pb.h"

#include "snappy.h"


namespace dota{
  
  Parser::Parser() {
    // std::function<void(const Parser&, CDemoFileHeader)> f = &Parser::OnCDemoFileHeader;
    // new Callback<const CDemoFileHeader&, Parser>(&Parser::OnCDemoFileHeader);
    // Connect<Parser, CDemoFileHeader>(Event::OnCDemoFileHeader, &Parser::OnCDemoFileHeader);
    // Connect<Parser, CDemoFileInfo>(Event::OnCDemoFileInfo, &Parser::OnCDemoFileInfo);
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
      CallByDemoType(message);
      // std::cout << message.kind << ' ' << message.tick << std::endl;
    }
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

    std::string buffer;
    buffer.reserve(size);

    // stream_.read(buffer.front(), size);
    // std::cout << "before: " << stream_.tellg()<< std::endl;
    // stream_.read(&buffer[0], size);
    std::copy_n(std::istreambuf_iterator<char>(stream_), size + 1, std::back_inserter<std::string>(buffer) );
    // std::cout << "after: " << stream_.tellg()<< std::endl;
    // std::cout << buffer << std::endl;

    // TopMessage message;
    TopMessage message {compressed, tick, kind, buffer, size};;

    if (compressed && snappy::IsValidCompressedBuffer(buffer.data(), size)) {
      std::size_t size_uncompressed;
      if (!snappy::GetUncompressedLength(buffer.data(), size, &size_uncompressed)) {
        std::cout << "cannot get uncompressed length" << std::endl;
      }
      message.data.clear();
      message.data.resize(size_uncompressed);
      if (!snappy::RawUncompress(buffer.data(), size, &message.data[0])) {
        std::cout << "uncompress unsuccesful" << std::endl;
      }
    } 
    return message;
  }

  // //
  // template <typename CDemoClass>
  // void Parser::Connect(Event e, void (*f) (const CDemoClass&)) {
  //   callbacks_.Connect(e, 
  //       std::unique_ptr<Callback<const CDemoClass&>>(new Callback<const CDemoClass&>(f)));
  // }

  // //
  // template <typename Object, typename CDemoClass>
  // void Parser::Connect(Event e, void (Object::*f) (const CDemoClass&)) {
  //   callbacks_.Connect(e, 
  //       std::unique_ptr<Callback<const CDemoClass&>>(new Callback<const CDemoClass&>(f)));
  // }
  //
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
  void Parser::CallByDemoType(const TopMessage &message) {
    switch (message.kind) {
      case DEM_FileHeader: {
        std::cout << "header: " << std::endl;
        CDemoFileHeader event;
        event.ParseFromString(message.data);
        // callbacks_.Call(Event::OnCDemoFileHeader, event);
        break;
      }
      case DEM_FileInfo: {
        CDemoFileInfo event;
        std::cout << "fileinfo: " << std::endl;
        event.ParseFromString(message.data);
        // callbacks_.Call(Event::OnCDemoFileInfo, event);
        break;
      }
      default: 
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