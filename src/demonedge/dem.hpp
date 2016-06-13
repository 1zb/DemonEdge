#ifndef _DEM_HPP_
#define _DEM_HPP_

#include <fstream>

#include "callback.hpp"

#include "demo.pb.h"
namespace dota {

struct TopMessage {
  bool compressed;
  uint32_t tick;
  uint32_t kind;
  // const char* data;
  std::string data;
  std::size_t size;
};

struct EmbeddedMessage {
  uint32_t tick;
  uint32_t kind;
  // const char* data;
  std::string data;
  std::size_t size;
};

class Parser {
 public:
  Parser();

  void Open(std::string path);

  //
  void Start();

  //
  TopMessage Read();

 private:
  //
  std::ifstream stream_;

  //
  // Callbacks callbacks_;

  //
  uint32_t tick_;

  // //
  // template <typename CDemoClass>
  // void Connect(Event e, void (*f) (const CDemoClass&));

  // //
  // template <typename Object, typename CDemoClass>
  // void Connect(Event e, void (Object::*f) (const CDemoClass&));
  
  // varint
  uint32_t ReadVarInt();

  //
  void CallByDemoType(const TopMessage &message);

  //
  void OnCDemoFileHeader(const CDemoFileHeader&);

  //
  void OnCDemoFileInfo(const CDemoFileInfo&);
};

}
#endif