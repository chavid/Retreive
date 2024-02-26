
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

    // utilities
    std::set<std::string> words_to_labels( std::vector<std::string> const & words ) const ;

    // setters
    Labels( Configuration const & cfg ) ;
    void required( std::vector<std::string> const & words ) ;
    void forbidden( std::vector<std::string> const & words ) ;
    void count( std::set<std::string> const & labels ) ;


    // getters
    bool empty() const ; // if no required or forbidden labels
    bool check( std::set<std::string> const & labels ) const ;
    void print_suggested_labels( std::size_t nb_results ) const ;

  private:

    PrimaryLabels primary_ ;
    std::set<std::string> required_ ;
    std::set<std::string> forbidden_ ;

    void print_by_10( std::string const & title, auto filter ) const ;
 
 } ;


