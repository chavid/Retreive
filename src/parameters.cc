
#include "parameters.hh"

Parameters::Parameters( Configuration & cfg )
 {
  constexpr std::string_view str_ignored = "ignored" ;
  constexpr std::string_view str_parsed = "parsed" ;
  constexpr std::string_view str_max_files = "print_max_files_per_directory" ;
  cfg.apply(":",[&]( auto name, auto values )
   {
    auto param { name } ;
    lower(param) ;
    if (name==str_ignored)
     {
      for ( auto value : values )
       {
        lower(value) ;
        ignored_.insert(value) ;
       }
     }
    else if (name==str_parsed)
     {
      for ( auto value : values )
       {
        lower(value) ;
        parsed_.insert(value) ;
       }
     }
    else if (name==str_max_files)
     {
      if (std::size(values)>1)
        { throw std::runtime_error(std::format("Too many right hand parameters for: {}.",str_max_files)) ; }
      print_max_files_per_directory_ = std::stoi(*(values.begin())) ;
     }
    else
     { throw std::runtime_error(std::format("Unknown parameter: {}.",name)) ; }
   }) ;
 }
