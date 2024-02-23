
#include "utilities.hh"
#include "cfg.hh"
#include <functional>

// test program for cfg.{hh,cc}

int main()
 { 
  Configuration cfg(".labels") ;

  // prepare test apply function
  auto test_apply = []( std::string const & op, auto name, auto values )
   {
    std::cout << name << " " << op ;
    for ( auto value : values )
     { std::cout << " " << value ; }
    std::cout << std::endl ;
   } ;

  // browse configuration
  using namespace std::placeholders ;
  std::cout << std::endl ;
  cfg.apply(":",std::bind(test_apply,":",_1,_2)) ;
  cfg.apply("==",std::bind(test_apply,"==",_1,_2)) ;
  cfg.apply("=>",std::bind(test_apply,"=>",_1,_2)) ;

  // end
  exit(0) ;
 }
 