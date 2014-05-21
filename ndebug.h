#ifndef NDEBUG_H
#define NDEBUG_H

#include <stdio.h>

namespace std
{
  class string;
}
class nDebug;

class nDebug
{
public:
  nDebug();
  virtual ~nDebug();
  nDebug &operator<<(std::string s);
  nDebug &operator<<(int number);
  
private:
};

#endif