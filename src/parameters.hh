
#pragma once

#include "utilities.hh"
#include "cfg.hh"

class Parameters
 {
  public:

    Parameters( Configuration & ) ;

    // Test extensions
    bool is_ignored( std::string const & ext ) const { return ignored_.contains(ext) ; }
    bool is_parsed( std::string const & ext ) const { return parsed_.contains(ext) ; }

    // Other values
    std::size_t print_max_files_per_directory() const { return print_max_files_per_directory_ ; }

  private:

    StringSet ignored_ ;
    StringSet parsed_ ;

    std::size_t print_max_files_per_directory_ = 5 ;

 } ;
