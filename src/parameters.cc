
#include "parameters.hh"

Parameters::Parameters( Configuration & cfg )
 {
  constexpr std::string_view str_ignore_dir = "ignore_dir" ;
  constexpr std::string_view str_ignore_ext = "ignore_ext" ;
  constexpr std::string_view str_parse = "parse" ;
  constexpr std::string_view str_max_files = "max_entries_per_matching_directory" ;
  cfg.apply(":",[&]( auto name, auto values )
   {
    auto param { lower(name) } ;
    if (param==str_ignore_dir)
     {
      for ( auto value : values )
       { ignore_dir_.insert(value) ; }
     }
    else if (param==str_ignore_ext)
     {
      for ( auto value : values )
       { ignore_ext_.insert(lower(value)) ; }
     }
    else if (param==str_parse)
     {
      for ( auto value : values )
       { parse_.insert(lower(value)) ; }
     }
    else if (param==str_max_files)
     {
      if (std::size(values)>1)
        { throw std::runtime_error(std::format("Too many right hand parameters for: {}.",str_max_files)) ; }
      max_entries_per_matching_directory_ = std::stoi(*(values.begin())) ;
     }
    else
     { throw std::runtime_error(std::format("Unknown parameter: {}.",name)) ; }
   }) ;
 }
