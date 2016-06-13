#include <iostream>
#include <thread>
#include <vector>

#include "dem.hpp"

int main(int argc, char** argv) {
  dota::Parser p;
  p.Open(argv[1]);
  p.Start();
  return 0;
}