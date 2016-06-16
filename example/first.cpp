#include <iostream>
#include <thread>
#include <vector>

#include "dem.hpp"

int main(int argc, char** argv) {
  dota::Parser p;
  p.Open(argv[1]);
  p.Start();
  // for (auto&& i : p.packet_) {
  //   std::cout << i.first << ' ' << i.second << std::endl;
  // }
  return 0;
}