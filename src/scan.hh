
#pragma once

#include "utilities.hh"
#include "cfg.hh"
#include "parameters.hh"
#include "labels.hh"

std::vector<std::string> scan_file( Parameters const &, fs::path const & path ) ;

// While browsing recursively the filesystem directories, we want to keep track
// of the hierarchy of selected files and directories. We can later browse this
// hierarchy to display the results in a customizable way.
class DirectoryScan
 {
  public:

    // setters
    DirectoryScan( fs::path const & path = "" ) : path_(path) {}
    void scan( Parameters const & ps, Labels & labels ) ;

    // getters
    std::size_t size() const { return size_ ; }
    void print_files( Parameters const & ps ) const ;
  
  private:

    fs::path path_ ;
    bool selected_dir_ = false ;
    std::vector<std::string> local_files_ ;
    std::size_t local_size_ = 0 ;
    std::vector<DirectoryScan> subdirs_ ;
    std::size_t size_ = 0 ;

    void recursive_print_files( Parameters const & ps ) const ;
    void recursive_scan_dir( Parameters const & ps, Labels & labels ) ;
    void count_files() ;
    void sort() ;

 } ;


