#include "src.hpp"
#include <iostream>
#include <string>

int main() {
  // Minimal driver: read commands to exercise any_ptr
  // Format:
  // n
  // ops...
  // set_int x | set_str s | copy | assign_null | print_int | print_str
  sjtu::any_ptr a;
  int n;
  if (!(std::cin >> n)) return 0;
  for (int i = 0; i < n; ++i) {
    std::string op; std::cin >> op;
    if (op == "set_int") {
      long long x; std::cin >> x; a = new long long(x);
    } else if (op == "set_str") {
      std::string s; std::cin >> s; a = new std::string(s);
    } else if (op == "copy") {
      sjtu::any_ptr b = a; a = b; // no-op but exercises copy
    } else if (op == "assign_null") {
      a = (long long*)nullptr;
    } else if (op == "print_int") {
      try { std::cout << a.unwrap<long long>() << "\n"; }
      catch (...) { std::cout << "bad_cast" << "\n"; }
    } else if (op == "print_str") {
      try { std::cout << a.unwrap<std::string>() << "\n"; }
      catch (...) { std::cout << "bad_cast" << "\n"; }
    }
  }
  return 0;
}

