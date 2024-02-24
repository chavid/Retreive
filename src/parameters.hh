
#pragma once

#include "utilities.hh"
#include "cfg.hh"

class Parameters
 {
  public:

    Parameters( Configuration & ) ;

    // Test extensions
    bool is_ignore( std::string const & ext ) const { return ignore_.contains(ext) ; }
    bool is_parse( std::string const & ext ) const { return parse_.contains(ext) ; }

    // Other values
    std::size_t max_entries_per_matching_directory() const { return max_entries_per_matching_directory_ ; }

  private:

    StringSet ignore_ ;
    StringSet parse_ ;

    std::size_t max_entries_per_matching_directory_ = 5 ;

 } ;
