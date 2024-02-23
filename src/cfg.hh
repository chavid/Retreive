
#pragma once

#include "utilities.hh"

// This class is used to find and parse configuration files.
// The files are searched in the home directory, the current directory, and an optional additional directory.
// Comments are allowed in the files, and start with a sharp.
// The lines are expected to be of the form `names: values`, `names == values`, or `names => values`.
// Individual names and values are separated by spaces.

class Configuration
 {
  public:

    static constexpr std::array<std::string,3> ops { ":", "==", "=>" } ; // allowed operators
    Configuration( std::string const & file_name, fs::path const & additional_directory_path = "" ) ;
    void apply( std::string const & op, auto name_values_func ) const ;

  private:

    void parse( std::string const & cfg_path ) ;
    std::unordered_map<std::string,std::unordered_map<std::string, std::set<std::string>>> data_ ;

 } ;

// Template implementation

void Configuration::apply( std::string const & op, auto name_values_func ) const
 {
  for ( auto const & [ name, values ] : data_.at(op) )
   { name_values_func(name, values) ; }
 }
