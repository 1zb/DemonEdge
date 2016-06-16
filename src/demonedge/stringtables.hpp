#ifndef _STRINGTABLES_HPP_
#define _STRINGTABLES_HPP_

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

namespace dota {

namespace stringtable {

struct StringTable {
  std::string name;
  int32_t max_entries;
  bool user_data_fixed_size;
  int32_t user_data_size;
  int32_t user_data_size_bits;
  int32_t flags;
  std::vector<std::string> names;
  std::vector<std::string> values;

  void set(int32_t index, const std::string& name, const std::string& value) {
    if (index + 1 > names.size()) {
      names.resize(index + 1);
      values.resize(index + 1);
    }
    names.at(index) = name;
    values.at(index) = value;
  }
  void Debug() {
    std::cout << name << '\t' << user_data_fixed_size << '\t' << user_data_size_bits << std::endl;
  }
};

void DecodeStringTable(StringTable& st, int num_entries, const std::string& data);

class StringTables {
 private:
  std::map<std::string, std::shared_ptr<StringTable>> sts_;
 public:
  void put(const std::string& name, std::shared_ptr<StringTable> st) {
    sts_.emplace(name, st);
  }
  void Debug() {
    for (auto iter = sts_.begin(); iter != sts_.end(); ++iter) {
      std::cout << std::string(8, '-') << std::endl;
      std::cout << iter->first << " " << iter->second->names.size() << std::endl;
      // std::for_each(iter->second->fields.cbegin(), iter->second->fields.cend(), 
      //     [](Field f){f.Debug();});
    }
  }
};

}  // namespace stringtable

}  // namespace dota
#endif  // SRC_DEMONEDGE_STRINGTABLES_HPP_
