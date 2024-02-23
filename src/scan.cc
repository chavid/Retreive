
#include "scan.hh"


//=============================================================================
// Internal utility functions
//=============================================================================

std::vector<std::string> scan_file( Parameters const & ps, fs::path const & file_path )
 {
  // ignored extensions
  std::string ext { file_path.extension().generic_string() } ;
  lower(ext) ;
  if ((ext.empty())||ps.is_ignored(ext))
   { return {} ; }

  // intermediary list of tokens
  std::vector<std::string> words ;

  // optionally parse the file content
  if (ps.is_parsed(ext))
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
  size_ += std::size(local_selected_files_) ;
 }


//=============================================================================
// DirectoryScan
//=============================================================================

void DirectoryScan::scan_for_labels( Parameters const & ps, Labels & labels )
 {
  scan_dir(ps,labels,[]( Parameters const & ps, Labels & labels, fs::path const & file_path, DirectoryScan & )
   { labels.primary(scan_file(ps,file_path)) ; }) ;
 }

void DirectoryScan::scan_for_files( Parameters const & ps, Labels & labels )
 {
  scan_dir(ps,labels,[]( Parameters const & ps, Labels & labels, fs::path const & file_path, DirectoryScan & res )
   {
    if (labels.check(scan_file(ps,file_path)))
     { res.local_selected_files_.emplace_back(file_path.string()) ; }
   });
  count_files() ;
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
    if (size_>ps.print_max_files_per_directory())
     {
      std::cout<<"  "<<path_.string()<<"/...\n" ;
      return ;
     }
   }
  for ( auto const & file : local_selected_files_ )
   { std::cout<<"  "<<file<<"\n" ; }
  for ( auto const & subdir : subdirs_ )
   { subdir.recursive_print_files(ps) ; }
 }
