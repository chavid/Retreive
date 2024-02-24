
#include "scan.hh"


//=============================================================================
// Internal utility functions
//=============================================================================

std::vector<std::string> scan_file( Parameters const & ps, fs::path const & file_path )
 {
  // ignore extensions
  std::string ext { file_path.extension().generic_string() } ;
  lower(ext) ;
  if ((ext.empty())||ps.is_ignore(ext))
   { return {} ; }

  // intermediary list of tokens
  std::vector<std::string> words ;

  // optionally parse the file content
  if (ps.is_parse(ext))
   {
    std::ifstream ifs(file_path.string()) ;
    std::string word ;
    while (ifs>>word)
     {
      if ((word.size()>2)&&(word[0]=='%')&(word[1]=='%')&(word[2]!='%'))
        { words.emplace_back(word.substr(2)) ; }
     }
   }
   
  // add the file path elements
  words.insert(words.end(),file_path.begin(),file_path.end()) ;

  return words ;
 }

void DirectoryScan::count_files()
 {
  size_ = 0 ;
  for ( auto & subdir : subdirs_ )
   { subdir.count_files() ; size_ += subdir.size_ ; }
  assert(std::empty(local_files_)||(local_size_==std::size(local_files_))) ;
  // local_files_ is empty if was done without storing the file names
  size_ += local_size_ ;
 }

void DirectoryScan::sort()
 {
  std::sort(local_files_.begin(),local_files_.end()) ;
  std::sort(subdirs_.begin(),subdirs_.end(),[]( auto const & subdir1, auto const & subdir2 )
   { return (subdir1.path_.filename()<subdir2.path_.filename()) ; }) ;
  for ( auto & subdir : subdirs_ )
   { subdir.sort() ; }
 }


//=============================================================================
// DirectoryScan
//=============================================================================

void DirectoryScan::recursive_scan_dir( Parameters const & ps, Labels & labels )
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
      // Note : if a link is dangling, is_symlink returns false
      if (fs::is_symlink(subs))
       { continue ; }
      if (fs::is_directory(subs))
       {
        subdirs_.emplace_back(subpath) ;
        subdirs_.back().recursive_scan_dir(ps,labels) ;
       }
      else if (fs::is_regular_file(subs))
       {
        if (labels.check(scan_file(ps,subpath)))
         {
          local_size_++ ;
          if (!labels.empty())
           { local_files_.emplace_back(subpath.string()) ; }
         }
       }
     }
   }
  else if (fs::is_regular_file(s))
   { throw std::runtime_error("Unexpected regular file.") ; }
  else
   { throw std::runtime_error("Unexpected kind of element.") ; }
 }
  
void DirectoryScan::scan( Parameters const & ps, Labels & labels )
 {
  recursive_scan_dir(ps,labels) ;
  count_files() ;
  sort() ;
 }

void DirectoryScan::print_files( Parameters const & ps ) const
 {
  std::cout<<std::format("\nFILES: {}\n",size_) ;
  recursive_print_files(ps) ;
 }

void DirectoryScan::recursive_print_files( Parameters const & ps ) const
 {
  if (selected_dir_)
   {
    if (size_==0)
     {
      std::cout<<"  "<<path_.string()<<"/ ?\n" ;
      return ;
     }
    if (size_>ps.max_entries_per_matching_directory())
     {
      std::cout<<"  "<<path_.string()<<"/...\n" ;
      return ;
     }
   }
  for ( auto const & file : local_files_ )
   { std::cout<<"  "<<file<<"\n" ; }
  for ( auto const & subdir : subdirs_ )
   { subdir.recursive_print_files(ps) ; }
 }
