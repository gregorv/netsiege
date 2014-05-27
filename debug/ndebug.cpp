#include "ndebug.h"

#include "config.h"
#include <iostream>

nDebug::nDebug()
{

}

nDebug& nDebug::operator<<(std::string s)
{
#ifdef DISABLE_DEBUG_OUTPUT
  std::cout << s.c_str() << " ";
#endif
  return *this;
}

nDebug& nDebug::operator<<(int number)
{
#ifdef DISABLE_DEBUG_OUTPUT
  std::cout << number << " ";
#endif
  return *this;
}

nDebug::~nDebug()
{
#ifdef DISABLE_DEBUG_OUTPUT
  std::cout << "\n";
#endif
}
