#include "ndebug.h"

#include "../flags.h"
#include <iostream>

nDebug::nDebug()
{

}

nDebug& nDebug::operator<<(std::string s)
{
#ifdef NDEBUG
  std::cout << s.c_str() << " ";
#endif
  return *this;
}

nDebug& nDebug::operator<<(int number)
{
#ifdef NDEBUG
  std::cout << number << " ";
#endif
  return *this;
}

nDebug::~nDebug()
{
#ifdef NDEBUG
  std::cout << "\n";
#endif
}
