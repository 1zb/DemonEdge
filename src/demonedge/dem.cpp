#include "dem.hpp"

#include <iostream>
#include <array>
#include <algorithm>

#include "reader.hpp"

#include "snappy.h"

namespace dota{
  
  // void test1(std::function<void()> func) {}

  // void test2(CDemoFileHeader m) {}

  Parser::Parser() {
    callbacks_.Connect<CDemoFileHeader>(Event::OnCDemoFileHeader, &Parser::OnCDemoFileHeader, this);
    callbacks_.Connect<CDemoFileInfo>(Event::OnCDemoFileInfo, &Parser::OnCDemoFileInfo, this);
    callbacks_.Connect<CDemoSyncTick>(Event::OnCDemoSyncTick, &Parser::OnCDemoSyncTick, this);
    callbacks_.Connect<CDemoSendTables>(Event::OnCDemoSendTables, &Parser::OnCDemoSendTables, this);
    callbacks_.Connect<CDemoClassInfo>(Event::OnCDemoClassInfo, &Parser::OnCDemoClassInfo, this);
    callbacks_.Connect<CDemoStringTables>(Event::OnCDemoStringTables, &Parser::OnCDemoStringTables, this);
    callbacks_.Connect<CDemoPacket>(Event::OnCDemoPacket, &Parser::OnCDemoPacket, this);
    callbacks_.Connect<CDemoPacket>(Event::OnCDemoSignonPacket, &Parser::OnCDemoPacket, this);
    callbacks_.Connect<CDemoFullPacket>(Event::OnCDemoFullPacket, &Parser::OnCDemoFullPacket, this);

    callbacks_.Connect<CSVCMsg_ServerInfo>(Event::OnCSVCMsg_ServerInfo, &Parser::OnCSVCMsg_ServerInfo, this);
    callbacks_.Connect<CSVCMsg_CreateStringTable>(Event::OnCSVCMsg_CreateStringTable, &Parser::OnCSVCMsg_CreateStringTable, this);    
    // callbacks_.Connect<CSVCMsg_PacketEntities>(Event::OnCSVCMsg_PacketEntities, &Parser::OnCSVCMsg_PacketEntities, this);

    // callbacks_.Debug();
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

    // std::cout << "pos: " << stream_.tellg() << std::endl;

    stream_.seekg(8, std::ios::cur);
    // std::cout << "pos: " << stream_.tellg() << std::endl;
  }

  //
  void Parser::Start() {
    while (stream_.good()) {
      TopMessage message = Read();
      tick_ = message.tick;
      // if (message.kind == DEM_ClassInfo)
      //   std::cout << message.kind << ' ' << message.size << ' ' << stream_.tellg() << std::endl;
      // if (kind == DEM_ClassInfo) 
        // std::cout << size << std::endl;
      CallByDemoType(message);
      if (message.kind == DEM_FileInfo) {
        // std::cout << tick_ << std::endl;
        break;
      }
    }
    // std::cout << "end" << std::endl;
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
    char* buffer = new char[0x100000];
    stream_.read(&buffer[0], size);
    // delete buffer;
    // std::copy_n(std::istreambuf_iterator<char>(stream_), size + 1, std::back_inserter<std::string>(buffer) );

    // stream_.seekg(size, std::ios::cur);
    TopMessage message {compressed, tick, kind, std::string(buffer, size), size};

    if (compressed && snappy::IsValidCompressedBuffer(buffer, size)) {
      std::size_t size_uncompressed;
      if (!snappy::GetUncompressedLength(buffer, size, &size_uncompressed)) {
        std::cout << "cannot get uncompressed length" << std::endl;
      }
      message.data.clear();
      message.data.resize(size_uncompressed);
      message.size = size_uncompressed;
      if (!snappy::RawUncompress(buffer, size, &message.data[0])) {
        std::cout << "uncompress unsuccesful" << std::endl;
      }
    } 

    // if (kind == 5)
    //   std::cout << compressed << ' ' << kind << ' ' << size << ' ' << message.data.size() << ' ' << tick << std::endl;

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
      // DEM_FileHeader = 1;
      case DEM_FileHeader: {
        CDemoFileHeader message;
        // std::cout << "header " <<  tmessage.tick << std::endl;
        callbacks_.Call(Event::OnCDemoFileHeader, message, tmessage.data);
        break;
      }
      // DEM_FileInfo = 2;
      case DEM_FileInfo: {
        CDemoFileInfo message;
        // std::cout << "fileinfo " << tmessage.tick << std::endl;
        callbacks_.Call(Event::OnCDemoFileInfo, message, tmessage.data);
        break;
      }
      // DEM_SyncTick = 3;
      case DEM_SyncTick: {
        CDemoSyncTick message;
        callbacks_.Call(Event::OnCDemoSyncTick, message, tmessage.data);
      }
      // DEM_SendTables = 4;
      case DEM_SendTables: {
        CDemoSendTables message;
        callbacks_.Call(Event::OnCDemoSendTables, message, tmessage.data);
        break;
      }
      // DEM_ClassInfo = 5;
      case DEM_ClassInfo: {
        CDemoClassInfo message;
        callbacks_.Call(Event::OnCDemoClassInfo, message, tmessage.data);
        break;
      }
      // DEM_StringTables = 6;
      case DEM_StringTables: {
        // std::cout << "string tables: " << tmessage.tick << std::endl;
        CDemoStringTables message;
        callbacks_.Call(Event::OnCDemoStringTables, message, tmessage.data);
        break;
      }
      // DEM_Packet = 7;
      case DEM_Packet: {
        // std::cout << "packet" << ' ' << tmessage.tick << std::endl;
        CDemoPacket message;
        callbacks_.Call(Event::OnCDemoPacket, message, tmessage.data);
        break;
      }
      // DEM_SignonPacket = 8;
      case DEM_SignonPacket: {
        CDemoPacket message;
        callbacks_.Call(Event::OnCDemoSignonPacket, message, tmessage.data);
      }
      // DEM_FullPacket = 13;
      case DEM_FullPacket: {
        // std::cout << "full packet" << ' ' << tmessage.tick << std::endl;
        CDemoFullPacket message;
        callbacks_.Call(Event::OnCDemoFullPacket, message, tmessage.data);
        break;
      }
      case DEM_Stop: {
        CDemoStop message;
        // std::cout << "stop " <<  tmessage.tick << std::endl;
        callbacks_.Call(Event::OnCDemoStop, message, tmessage.data);
        break;
      }
      default: 
        // std::cout << tick_ << std::endl;
        break;
    }
  }

  //
  void Parser::CallbyPacketType(const EmbeddedMessage &emessage) {
    switch (emessage.kind) {
      // 40
      case svc_ServerInfo : {
        // std::cout << "server_info" << std::endl;
        CSVCMsg_ServerInfo message;
        callbacks_.Call(Event::OnCSVCMsg_ServerInfo, message, emessage.data);
        break;
      }
      // 44
      case svc_CreateStringTable : {
        // std::cout << "string table created " << emessage.tick << std::endl;
        CSVCMsg_CreateStringTable message;
        callbacks_.Call(Event::OnCSVCMsg_CreateStringTable, message, emessage.data);
        break;
      }
      case svc_UpdateStringTable: {
        // std::cout << "updating string table " << emessage.tick << std::endl;
        break;
      }
      // 55
      case svc_PacketEntities: {
        // std::cout << "packet entities" << std::endl;
        CSVCMsg_PacketEntities message;
        callbacks_.Call(Event::OnCSVCMsg_PacketEntities, message, emessage.data);
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

  //
  void Parser::OnCDemoSyncTick(const CDemoSyncTick& message) {
    // std::cout << "sync tick" << std::endl;
  }
  //
  void Parser::OnCDemoSendTables(const CDemoSendTables& message) {
    Reader reader(message.data());
    // std::cout << "reader: " << message.data().size() << std::endl;
    uint32_t size = reader.ReadVarInt();
    char* data = new char[0x100000];
    // std::cout << "sendtables: " << size << std::endl;
    reader.ReadBytes(data, size);
    CSVCMsg_FlattenedSerializer fs;
    fs.ParseFromArray(data, size);
    // std::cout << fs.symbols_size() << std::endl;
    
    // ProtoFlattenedSerializer_t
    // for (int i = 0; i < fs.serializers_size(); i++) {
    for (auto && pfs : fs.serializers()) {
      // std::cout << fs.symbols(fs.serializers(i).serializer_name_sym()) << std::endl;
      std::string dt_name = fs.symbols(pfs.serializer_name_sym());
      // int32_t dt_version = pfs.serializer_version();
      // datatable::Datatable dt {
      //   std::vector<datatable::Field>(),
      //   dt_name,
      //   dt_version,
      //   -1
      // };
      // auto dt = std::make_shared<datatable::Datatable>(datatable::Datatable{
      //   std::vector<datatable::Field>(),
      //   dt_name,
      //   dt_version,
      //   -1
      // });
      auto dt = std::make_shared<datatable::Datatable>();
      // dt->name = fs.symbols(pfs.serializer_name_sym());
      dt->name = dt_name;
      dt->version = pfs.serializer_version();
      for (int j = 0; j < pfs.fields_index_size(); j++) {
        // ProtoFlattenedSerializerField_t
        auto pfsf = fs.fields(pfs.fields_index(j));
        datatable::Field field {
          fs.symbols(pfsf.var_name_sym()),
          fs.symbols(pfsf.var_type_sym()),
          fs.symbols(pfsf.var_encoder_sym()),
          pfsf.encode_flags(),
          pfsf.bit_count(),
          pfsf.low_value(),
          pfsf.high_value(),
        };
        // field.Debug();
        dt->fields.push_back(field);
      }
      datatables_.put(dt_name, dt);
    }
    // datatables_.Debug();
    delete data;
  }

  //
  void Parser::OnCDemoClassInfo(const CDemoClassInfo& message) {
    for (auto&& c : message.classes()) {
      datatables_.set(c.network_name(), c.class_id());
    }
    // datatables_.Debug();
  }

  //
  void Parser::OnCDemoStringTables(const CDemoStringTables& message) {
    // std::cout << "string tables" << std::endl;
    // for (auto&& table : message.tables()) {
    //   std::cout << table.table_name() << std::endl;
    // }
  }
  //
  void Parser::OnCDemoPacket(const CDemoPacket& message) {
    Reader reader(message.data());
    // if (tick_ == 82213 || tick_ == 82243)
    // std::cout << "packet " << message.DebugString() << std::endl;
    while (reader.good()) {
      uint32_t kind = reader.ReadUBitVar();
      uint32_t size = reader.ReadVarInt();
      // if (kind == 40)
      // std::cout << kind << ' ' << size << ' ' << tick_ << std::endl;
      char* data = new char[0x100000];
      reader.ReadBytes(data, size);
      EmbeddedMessage emessage {tick_, kind, std::string(data,size), size};
      // std::cout << emessage.data.size() << std::endl;
      CallbyPacketType(emessage);
      // packet_[kind]++;
      delete data;
    }
  }

  //
  void Parser::OnCDemoFullPacket(const CDemoFullPacket &message) {
    if (message.has_packet()) {
      this->OnCDemoPacket(message.packet());
    }
    if (message.has_string_table()) {
      // std::cout << "has string table" << std::endl;
    }
  }

  //
  void Parser::OnCSVCMsg_ServerInfo(const CSVCMsg_ServerInfo &message) {
    // if (!message.has_max_classes()) return;
    // std::cout << "server info" << std::endl;
    // std::cout << message.DebugString() << std::endl;
    // this->class_id_size_ = std::ceil(std::log2(message.max_classes()));
    // std::cout << "class_id_size " << class_id_size_ << std::endl;
    // std::cout << message.game_dir() << std::endl;
    // std::cout << message.map_name() << std::endl;
    // std::cout << message.max_classes() << std::endl;
    // std::cout << message.host_name() << std::endl;
    // std::cout << std::ceil(std::log2(message.max_classes())) << std::endl;
  }

  //
  void Parser::OnCSVCMsg_CreateStringTable(const CSVCMsg_CreateStringTable& message) {
    // std::cout << "stringtable created" << std::endl;

    auto st = std::make_shared<stringtable::StringTable>();
    // stringtable::StringTable st;
    st->name                  = message.name();
    st->user_data_fixed_size  = message.user_data_fixed_size();
    st->user_data_size        = message.user_data_size();
    st->user_data_size_bits   = message.user_data_size_bits();
    st->flags                 = message.flags();
    // st.Debug();
    std::string string_data = message.string_data();
    if (message.data_compressed()) {
      std::size_t size_uncompressed;
      snappy::GetUncompressedLength(string_data.data(), string_data.size(), &size_uncompressed);
      // std::cout << name << ' ' << string_data.size() << ' ' << size_uncompressed << std::endl;
      std::string uncompressed;
      uncompressed.resize(size_uncompressed);
      snappy::RawUncompress(string_data.data(), string_data.size(), &uncompressed[0]);
      // std::cout << "uncompressed size " << uncompressed.size() << std::endl;
      string_data.clear();
      string_data = uncompressed;
    }
    // std::cout << st.name << ' ' << string_data.size() << std::endl;
    stringtable::DecodeStringTable(*st, message.num_entries(), string_data);
    stringtables_.put(st->name, st);
    // stringtables_.Debug();
  }
  //
  void Parser::OnCSVCMsg_PacketEntities(const CSVCMsg_PacketEntities &message) {
    // std::cout << message.DebugString() << std::endl;
    Reader reader(message.entity_data());
    int32_t entity_index = -1;
    // std::cout << "start " << reader.position() << std::endl;
    int32_t updated_entries = message.updated_entries();
    // for (int i = 0; i < updated_entries; i++) {
    for (int i = 0; i < 1; i++) {
      uint32_t delta = reader.ReadUBitVar();
      entity_index += (int32_t)delta + 1;
      int event_type = 0;
      if (reader.ReadBool()) {
        if (reader.ReadBool()) {
          event_type = 3;
        } else {
          event_type = 4;
        }
      } else {
        if (reader.ReadBool()) {
          event_type = 1;
        } else {
          event_type = 2;
        }
      }
      // std::cout << event_type << std::endl;
      switch (event_type) {
        case 1: {
          int32_t class_id = reader.ReadBits(10);
          int32_t serial = reader.ReadBits(17);
          reader.ReadVarInt();
          std::cout << datatables_.get(class_id)->name << ' ' << serial << std::endl;
        }
      }
    }
  }
}
