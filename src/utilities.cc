
#include "utilities.hh"

#include <algorithm>
#include <cctype>

void lower( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
 }
