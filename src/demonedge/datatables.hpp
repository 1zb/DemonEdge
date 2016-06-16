#ifndef _DATATABLES_HPP_
#define _DATATABLES_HPP_

#include <string>
#include <functional>
#include <vector>
#include <map>
// #include <ostream>
#include <iostream>
#include <initializer_list>
#include <memory>

#include "reader.hpp"

namespace dota {

namespace datatable {

struct Field {
  std::string name;
  std::string type;
  std::string encoder;
  int32_t flags;
  int32_t bit_count;
  float low_value;
  float high_value;
  void Debug() {
    std::cout << name << ' ' 
        << type << ' ' 
        << encoder << ' ' 
        << flags << ' '
        << flags << ' '
        << bit_count << ' '
        << low_value << ' '
        << high_value << std::endl;
  }
};

struct Datatable {
  std::vector<Field> fields;
  std::string name;
  int32_t version;
  int32_t class_id;
  Datatable() = default;
  Datatable(const Datatable& dt) = default;
};

class Datatables {
 private:
  std::map<int32_t, std::shared_ptr<Datatable>> by_class_id_;
  std::map<std::string, std::shared_ptr<Datatable>> by_dt_name_;

 public:
  Datatables() {}
  void put(const std::string& name, std::shared_ptr<Datatable> dt) {
    by_dt_name_.emplace(name, dt);
  }
  void put(const int32_t& id, std::shared_ptr<Datatable> dt) {
    by_class_id_.emplace(id, dt);
  }
  std::shared_ptr<Datatable> get(const std::string& name) {
    auto iter = by_dt_name_.find(name);
    return iter->second;
  }
  std::shared_ptr<Datatable> get(const int32_t& id) {
    auto iter = by_class_id_.find(id);
    return iter->second;
  }
  void set(const std::string& name, const int32_t& id) {
    auto iter = by_dt_name_.find(name);
    iter->second->class_id = id;
    put(id, iter->second);
  }
  void Debug() {
    for (auto iter = by_dt_name_.begin(); iter != by_dt_name_.end(); ++iter) {
      std::cout << std::string(8, '-') << std::endl;
      std::cout << iter->first << " " << iter->second->class_id << std::endl;
      std::for_each(iter->second->fields.cbegin(), iter->second->fields.cend(), 
          [](Field f){f.Debug();});
    }
  }
};

}

}
#endif  // SRC_DEMONEDGE_DATATABLES_HPP_
