
#pragma once

#include "utilities.hh"
#include "cfg.hh"

struct PrimaryLabelData
 {
  std::size_t frequence = 0 ;
  std::set<std::string> synonyms ;
  std::set<std::string> children ;
  std::set<std::string> parents ;
  std::set<std::string> ancestors ;
  std::vector<fs::path> files ;
 } ;

using PrimaryLabels = std::unordered_map<std::string,PrimaryLabelData> ;

class Labels
 {
  public:

    // setters
    Labels( Configuration const & cfg ) ;
    void required( std::vector<std::string> const & words ) ;
    void forbidden( std::vector<std::string> const & words ) ;
    void primary( std::vector<std::string> const & words ) ;

    // in between...
    bool check( std::vector<std::string> const & labels, bool count = true ) ;

    // getters
    bool empty() const ; // if no required or forbidden labels
    void print_primary() const ;
    void print_sub_labels( std::size_t nb_results ) const ;

  private:

    PrimaryLabels primary_ ;
    std::set<std::string> required_ ;
    std::set<std::string> forbidden_ ;

    std::set<std::string> words_to_labels( std::vector<std::string> const & words ) const ; 

 } ;


