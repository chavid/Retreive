
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
    void scan_for_labels( Parameters const & ps, Labels & labels ) ;
    void scan_for_files( Parameters const & ps, Labels & labels ) ;

    // getters
    std::size_t size() const { return size_ ; }
    void print_files( Parameters const & ps ) const ;
  
  private:

    fs::path path_ ;
    bool selected_dir_ = false ;
    std::vector<std::string> local_selected_files_ ;
    std::vector<DirectoryScan> subdirs_ ;
    std::size_t size_ ;

    void recursive_print_files( Parameters const & ps ) const ;
    void scan_dir( Parameters const & ps, Labels & labels, auto scan_regular_file ) ;
    void count_files() ;

 } ;

// Template implementation

void DirectoryScan::scan_dir( Parameters const & ps, Labels & labels, auto scan_regular_file )
 {
  auto s = fs::status(path_) ;
  if (fs::is_symlink(s))
   { return ; }
  if (fs::is_directory(s))
   {
    // ignore hidden directories
    std::string dirname { path_.filename().string() } ;
    if ((std::size(dirname)>1)&&(dirname[0]=='.')&&(dirname[1]!='.'))
     { return ; }
    if ((std::size(dirname)>1)&&(dirname[0]=='_')&&(dirname[1]=='_'))
     { return ; }
    // directory characteristics
    std::vector<std::string> dir_path_tokens { path_.begin(), path_.end() } ;
    selected_dir_ = labels.check(dir_path_tokens,false) ;
    // scan the directory
    for ( auto it = fs::directory_iterator(path_) ; it != fs::directory_iterator() ; ++it )
     {
      fs::path subpath = *it ;
      auto subs = fs::status(subpath) ;
      if (fs::is_symlink(subs))
       { continue ; }
      if (fs::is_directory(subs))
       {
        subdirs_.emplace_back(subpath) ;
        subdirs_.back().scan_dir(ps,labels,scan_regular_file) ;
       }
      else if (fs::is_regular_file(subs))
       { scan_regular_file(ps,labels,subpath,*this) ; }
      else
       { throw std::runtime_error("Unexpected kind of element.") ; }
     }
   }
  else if (fs::is_regular_file(s))
   { throw std::runtime_error("Unexpected regular file.") ; }
  else
   { throw std::runtime_error("Unexpected kind of element.") ; }
 }
  
