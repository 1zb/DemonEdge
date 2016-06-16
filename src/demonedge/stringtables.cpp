#include "stringtables.hpp"

#include "reader.hpp"
namespace dota {

namespace stringtable {

void DecodeStringTable(StringTable& st, int num_entries, const std::string& data) {
  // std::cout << st.name << ' ' << "num entries: " << num_entries << std::string(10, '-') << std::endl;
  
  Reader reader(data);
  std::vector<std::string> key_history;

  int index = -1;
  std::string name;
  for (int i = num_entries; i != 0; i--) {
    if (reader.ReadBool()) {
      index++;
      // std::cout << "index " << index << std::endl;
    } else {
      // std::cout << "index " << index << std::endl;
      index = reader.ReadVarInt() + 1;
    }
    name.resize(0);
    if (reader.ReadBool()) {
      if (reader.ReadBool()) {
        int basis = reader.ReadBits(5);
        int length = reader.ReadBits(5);
        // std::cout << basis << ' ' << length << std::endl;
        name += key_history.at(basis).substr(0, length);
        name += reader.ReadString();
      } else {
        // std::cout << reader.ReadString() << std::endl;
        name += reader.ReadString();
      }
      if (key_history.size() == 32) {
        key_history.erase(key_history.begin());
      }
      key_history.push_back(name);
    }

    std::string value;
    if (reader.ReadBool()) {
      int bit_length;
      // int byte_length;
      bool is_compressed = false;
      if (st.user_data_fixed_size) {
        bit_length = st.user_data_size_bits;
        // byte_length = st.user_data_size;
      } else {
        if ((st.flags & 0x1) != 0) {
          is_compressed = reader.ReadBool();
        }
        bit_length = reader.ReadBits(17) * 8;
      }
      int byte_length = (bit_length + 7) / 8;
      if (is_compressed) {
        std::cout << "compressed!" << std::endl;
      } else {
        reader.ReadBits(value, bit_length);
      }
      // std::cout << index << ' ' << bit_length << ' ' << byte_length << ' ' << value.size() << ' ' << is_compressed << std::endl;
    }
    // std::cout << index << '\t' << name << '\t' << value.size() << std::endl;
    // if (value.size() != 0) std::cout << value << std::endl;
    st.set(index, name, value);
  } 
}

}

}
