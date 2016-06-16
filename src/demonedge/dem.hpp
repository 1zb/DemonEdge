#ifndef _DEM_HPP_
#define _DEM_HPP_

#include <fstream>

#include "callback.hpp"
#include "datatables.hpp"
#include "stringtables.hpp"

#include "demo.pb.h"
#include "netmessages.pb.h"

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

  ~Parser() = default;
  
  void Open(std::string path);

  //
  void Start();

  //
  TopMessage Read();

  // std::map<int, int> packet_;
 private:
  //
  std::ifstream stream_;

  //
  Callbacks callbacks_;

  //
  uint32_t tick_;

  //
  datatable::Datatables datatables_;
  
  //
  stringtable::StringTables stringtables_;
  
  // varint
  uint32_t ReadVarInt();

  //
  void CallByDemoType(const TopMessage &tmessage);

  //
  void CallbyPacketType(const EmbeddedMessage &emessage);

  // DEM_FileHeader = 1
  void OnCDemoFileHeader(const CDemoFileHeader&);

  // DEM_FileInfo = 2
  void OnCDemoFileInfo(const CDemoFileInfo&);

  // DEM_SyncTick = 3;
  void OnCDemoSyncTick(const CDemoSyncTick&);

  // DEM_SendTables = 4;
  void OnCDemoSendTables(const CDemoSendTables&);

  // DEM_ClassInfo = 5;
  void OnCDemoClassInfo(const CDemoClassInfo&);

  // DEM_StringTables = 6;
  void OnCDemoStringTables(const CDemoStringTables&);

  // DEM_Packet = 7
  void OnCDemoPacket(const CDemoPacket&);

  // DEM_FullPacket = 13
  void OnCDemoFullPacket(const CDemoFullPacket&);

  // svc_ServerInfo = 40;
  void OnCSVCMsg_ServerInfo(const CSVCMsg_ServerInfo&);

  // svc_CreateStringTable = 44;
  void OnCSVCMsg_CreateStringTable(const CSVCMsg_CreateStringTable&);

  // svc_PacketEntities = 55;
  void OnCSVCMsg_PacketEntities(const CSVCMsg_PacketEntities&);
};

}
#endif