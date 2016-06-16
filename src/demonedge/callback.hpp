#ifndef _CALLBACK_HPP_
#define _CALLBACK_HPP_

#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <iostream>

#include <google/protobuf/message.h>

#include "delegate.hpp"

namespace dota {

enum class Event {
  OnCDemoFileHeader,
  OnCDemoFileInfo,
  OnCDemoSyncTick,
  OnCDemoSendTables,
  OnCDemoClassInfo,
  OnCDemoStringTables,
  OnCDemoPacket,
  OnCDemoSignonPacket,
  OnCDemoFullPacket,
  OnCDemoStop,

  OnCSVCMsg_ServerInfo,
  OnCSVCMsg_CreateStringTable,
  OnCSVCMsg_PacketEntities,
};


class Callbacks {
 private:
  Delegate<Event> callbacks_;

  template<typename CDemoClass>
  void Connect(Event e, std::function<void(CDemoClass)>&& func) {
    callbacks_.Connect(e, std::move(func));
  }

 public:
  template<typename CDemoClass, typename Function>
  void Connect(Event e, Function&& func) {
    this->Connect(e, std::function<void(CDemoClass)>(std::forward<Function>(func)));
  }

  template<typename CDemoClass, typename Function, typename Object>
  void Connect(Event e, Function&& func, Object* obj) {
    this->Connect(e, 
        std::function<void(CDemoClass)>(
            std::bind(std::forward<Function>(func), 
                obj, std::placeholders::_1)));
    // std::function<void(CDemoClass)> f = [=](CDemoClass m) {
    //   obj->func(m);
    // };
  }

  template<typename CDemoClass>
  void Call(Event e, CDemoClass m) {
    callbacks_.Call(e, m);
  }

  template<typename CDemoClass>
  void Call(Event e, CDemoClass m, std::string data) {
    // m.ParseFromString(data);
    if (!m.ParseFromString(data)) {
      std::exit(0);
    }
    Call(e, m);
  }

  void Debug() {
    callbacks_.Debug();
  }
};

}
#endif