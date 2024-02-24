
#include "utilities.hh"

#include <algorithm>
#include <cctype>

std::string lower( std::string str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
  return str ;
 }
