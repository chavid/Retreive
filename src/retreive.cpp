
/*

The user provides a list of labels, and wants to find the files and directories
that match all the labels. One can also forbid some labels, with the ~ prefix.

The directories are recursively scanned for files, except the ones whose name
start with a dot, which are considered hidden.

The labels are searched for in directories names, files names, and the files
content if they are text files. In the latter case, the content is parsed for
words starting with %%.

In the configuration file called `~/.labels`, the labels can be given synonyms,
and the user can specify that a label implies another. The tool also check for
a similar file in the current directory, and in the target directory set
with the `-d` option, if different from the current directory.

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <thread>
#include <mutex>
#include <string>
#include <string_view>
#include <format>
#include <regex>

#include "argparse.hpp"

namespace fs = std::filesystem ;

using StringVec = std::vector<std::string> ;
using StringSet = std::set<std::string> ;
using StringViewVec = std::vector<std::string_view> ;
using StringViewSet = std::set<std::string_view> ;

using Files = std::vector<fs::path> ;
using Threads = std::vector<std::thread> ;

struct Parameters
 {
  StringSet ignored ;
  StringSet parsed ;
  std::regex sep ;
  std::regex camel_start ;
  std::size_t print_max_files_per_directory = 5 ;
 } ;

struct PrimaryLabelData
 {
  std::size_t frequence = 0 ;
  StringSet synonyms ;
  StringSet children ;
  StringSet parents ;
  StringSet ancestors ;
  std::vector<fs::path> files ;
 } ;

using PrimaryLabels = std::unordered_map<std::string,PrimaryLabelData> ;

struct Labels
 {
  PrimaryLabels primary ;
  StringSet required ;
  StringSet forbidden ;

  bool check( StringSet const & labels ) const
   {
    // a single forbidden label is enough to reject the file
    for ( auto const & flabel : forbidden )
    for ( auto const & label : labels )
     {
      if (label==flabel)
       { return false ; }
      if (primary.contains(label))
      if (primary.at(label).ancestors.contains(flabel))
       { return false ; }
     }

    // all required labels must be present
    for ( auto const & rlabel : required )
     {
      bool found = false ;
      for ( auto const & label : labels )
       {
        if (label==rlabel)
        { found = true ; break ; }
        if (primary.contains(label))
        if (primary.at(label).ancestors.contains(rlabel))
        { found = true ; break ; }
       }
      if (!found)
       { return false ; }
     }
    return true ;
   }
 } ;

void lower( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
 }

void parse_cfg( std::string const & cfg_path, Parameters & ps, PrimaryLabels & labels )
 {
  std::cout<<"  "<<cfg_path<<"\n"<<std::flush ;
  constexpr std::string_view implies = "=>" ;
  constexpr std::string_view equals = "==" ;
  constexpr std::string_view params = ":" ;
  std::ifstream ifs(cfg_path) ;
  std::string line, word ;
  while (std::getline(ifs,line))
   {
    // remove comments
    auto sharp_pos = line.find('#') ;
    if (sharp_pos!=std::string::npos)
     { line.erase(sharp_pos) ; } 
    if (line.empty()) continue ;
    
    lower(line) ; 
    std::istringstream iss(line) ;
    std::size_t op_implies = 0 ;
    std::size_t op_equals = 0 ;
    std::size_t op_params = 0 ;
    StringVec lhs, rhs ;
    while (iss>>word)
     {
      if (word==implies)
      { op_implies++ ; }
      else if (word==equals)
      { op_equals++ ; }
      else if (word==params)
      { op_params++ ; }
      else if ((op_implies+op_equals+op_params)==0)
      { lhs.emplace_back(word) ; }
      else
      { rhs.emplace_back(word) ; }
     }
    assert((op_implies+op_equals+op_params)==1) ;
    if (op_implies)
     {
      for (auto const & lh : lhs)
      for (auto const & rh : rhs)
       {
        labels[lh].parents.emplace(rh) ;
        labels[rh].children.emplace(lh) ;
       }
     }
    else if (op_equals)
     {
      for (auto const & lh : lhs)
      for (auto const & rh : rhs)
       {
        labels[lh].synonyms.emplace(rh) ;
       }
     }
    else if (op_params)
     {
      if (lhs.size()<1)
       { throw std::runtime_error("Configuration file : lacking parameter on the left hand size.") ; }
      if (lhs.size()>1)
       { throw std::runtime_error("Configuration file : too many parameters on the left hand size.") ; }
      
      if (lhs[0]=="ignored")
       {
        for ( auto rh : rhs )
         {
          lower(rh) ;
          ps.ignored.emplace(rh) ;
         }
        continue ;
       }
      else if (lhs[0]=="parsed")
       {
        for ( auto rh : rhs )
         {
          lower(rh) ;
          ps.parsed.emplace(rh) ;
         }
        continue ;
       }
      
      if (rhs.size()<1)
       { throw std::runtime_error("Configuration file : lacking parameter on the right hand size.") ; }
      if (rhs.size()>1)
       { throw std::runtime_error("Configuration file : too many parameters on the right hand size.") ; }
      
      if (lhs[0]=="sep")
       { ps.sep = std::regex{ rhs[0] } ; continue ; }
      else if (lhs[0]=="camel_start")
       { ps.camel_start = std::regex{ rhs[0] } ; continue ; }
      else if (lhs[0]=="print_max_files_per_directory")
       { ps.print_max_files_per_directory = std::stoi(rhs[0]) ; continue ; }
      
      throw std::runtime_error("Unknown parameter in the configuration file.") ;
     }
   }
 }

void check_cfg( fs::path const & arg_path, Parameters & ps, PrimaryLabels & labels  )
 {
  StringVec cfg_paths ;

  // home
  std::string home_path { std::getenv("HOME") } ;
  home_path.append("/.labels") ;
  if (fs::is_regular_file(home_path))
   { cfg_paths.emplace_back(home_path) ; }

  // pwd
  std::string pwd_path { std::getenv("PWD") } ;
  pwd_path.append("/.labels") ;
  if (fs::is_regular_file(pwd_path))
   { cfg_paths.emplace_back(pwd_path) ; }

  // arg_path
  std::string target_path { arg_path } ;
  target_path.append("/.labels") ;
  if (target_path!=pwd_path)
  if (fs::is_regular_file(target_path))
   { cfg_paths.emplace_back(target_path) ; }

  // parse the files
  if (!cfg_paths.empty())
   {
    std::cout<<"\nCFG:\n" ;
    for ( auto const & cfg_path : cfg_paths )
     { parse_cfg(cfg_path,ps,labels) ; }
   }
 }

void expand_parent( PrimaryLabels const & labels, std::string const & parent, PrimaryLabelData & label_data )
 {
  label_data.ancestors.insert(parent) ;
  for ( auto & label : labels )
  if ( label.first == parent )
  for ( auto & ancestor : label.second.parents )
   { expand_parent(labels,ancestor,label_data) ; }
 }

void expand_ancestors( PrimaryLabels & labels )
 {
  for ( auto & label : labels )
   {
    //label.second.ancestors.insert(label.first) ;
    for ( auto const & parent : label.second.parents )
     { expand_parent(labels,parent,label.second) ; }
   }
 }

StringSet process_synonyms( PrimaryLabels const & cfg_labels, StringSet const & labels )
 {
  StringSet res ;
  for ( auto const & label : labels )
   {
    // it is already a main static label
    if (cfg_labels.contains(label))
     { res.emplace(label) ; }
    // is it in the synonyms ?
    else
     {
      bool found = false ;
      for ( auto const & static_label : cfg_labels )
      if ( static_label.second.synonyms.contains(label) )
       { res.emplace(static_label.first) ; found = true ; break ; }
      // dynamic label
      if(!found)
       { res.emplace(label) ; }
     }
   }
  return res ;
 }

StringSet tokens_to_labels( Parameters const & ps, PrimaryLabels const & cfg_labels, StringVec const & tokens1 )
 {
  // split on non-words characters plus
  StringVec tokens2 ;
  for ( std::string token1 : tokens1 )
   {
    std::istringstream iss{ std::regex_replace(token1,ps.sep," ") } ;
    std::string token2 ;
    while (iss>>token2)
     { tokens2.emplace_back(token2) ; }
   }

  // CamelCase
  StringVec tokens3 ;
  for ( std::string token2 : tokens2 )
   {
    std::smatch m ;
    while ( std::regex_match(token2,m,ps.camel_start) )
     {
      tokens3.emplace_back(m[1].str()) ;
      token2 = m[2].str() ;
     }
    tokens3.emplace_back(token2) ;
   }

  // lower process
  StringSet res ;
  for ( std::string token3 : tokens3 )
   {
    lower(token3) ;
    res.emplace(token3) ;
   }

  // final step
  return process_synonyms(cfg_labels,res) ;
 }

StringSet scan_file( Parameters const & ps, PrimaryLabels const & cfg_labels, fs::path const & path )
 {
  // ignored extensions
  std::string ext { path.extension().generic_string() } ;
  lower(ext) ;
  if ((ext.empty())||ps.ignored.contains(ext))
   { return {} ; }

  // intermediary list of tokens
  StringVec tokens1 ;

  // optionally parse the file content
  if (ps.parsed.contains(ext))
   {
    std::ifstream ifs(path.string()) ;
    std::string word ;
    while (ifs>>word)
     {
      if ((word.size()>2)&&(word[0]=='%')&(word[1]=='%')&(word[2]!='%'))
        { tokens1.emplace_back(word.substr(2)) ; }
     }
   }
   
  // add the file path elements
  tokens1.insert(tokens1.end(),path.begin(),path.end()) ;

  // process the tokens
  StringSet res { tokens_to_labels(ps,cfg_labels,tokens1) } ;

  return res ;
 }

// While browsing recursively the filesystem directories, we want to keep track
// of the hierarchy of selected files and directories. We can later browse this
// hierarchy to display the results in a customizable way.
struct DirectoryResults
 {
  fs::path path ;
  bool selected_dir = false ;
  StringVec local_selected_files ;
  std::vector<DirectoryResults> subdirs ;
  std::size_t size ;

  void count_files()
   {
    size = 0 ;
    for ( auto & subdir : subdirs )
     { subdir.count_files() ; size += subdir.size ; }
    size += std::size(local_selected_files) ;
   }

  void print_files( Parameters const & ps, std::string const & prefix ) const
   {
    if (selected_dir &&(size>ps.print_max_files_per_directory))
     {
      std::cout<<prefix<<path.string()<<"/...\n" ;
      return ;
     }
    for ( auto const & file : local_selected_files )
     { std::cout<<prefix<<file<<"\n" ; }
    for ( auto const & subdir : subdirs )
     { subdir.print_files(ps,prefix) ; }
   }

 } ;

template< class RegularFileFunction > 
void scan_dir( Parameters const & ps, Labels & labels, fs::path const & path, DirectoryResults & res, RegularFileFunction scan_regular_file )
 {
  auto s = fs::status(path) ;
  if (fs::is_symlink(s))
   { return ; }
  if (fs::is_directory(s))
   {
    // ignore hidden directories
    std::string dirname { path.filename().string() } ;
    if ((std::size(dirname)>1)&&(dirname[0]=='.')&&(dirname[1]!='.'))
     { return ; }
    if ((std::size(dirname)>1)&&(dirname[0]=='_')&&(dirname[1]=='_'))
     { return ; }
    // directory characteristics
    res.path = path ;
    StringVec dir_path_tokens { path.begin(), path.end() } ;
    StringSet dir_path_labels = tokens_to_labels(ps,labels.primary,dir_path_tokens) ;
    res.selected_dir = labels.check(dir_path_labels) ;
    // scan the directory
    for ( auto it = fs::directory_iterator(path) ; it != fs::directory_iterator() ; ++it )
     {
      fs::path subpath = *it ;
      auto subs = fs::status(subpath) ;
      if (fs::is_symlink(subs))
       { continue ; }
      if (fs::is_directory(subs))
       {
        res.subdirs.emplace_back() ;
        scan_dir(ps,labels,subpath,res.subdirs.back(),scan_regular_file) ;
       }
      else if (fs::is_regular_file(subs))
       { scan_regular_file(ps,labels,subpath,res) ; }
      else
       { throw std::runtime_error("Unexpected kind of element.") ; }
     }
   }
  else if (fs::is_regular_file(s))
   { throw std::runtime_error("Unexpected regular file.") ; }
  else
   { throw std::runtime_error("Unexpected kind of element.") ; }
 }


// Main program
int main( int argc, char const* argv[] )
 {
  // Parse arguments
  argparse::ArgumentParser program("label_tower", "1.3", argparse::default_arguments::all);
  program.add_description("Find files and directories with the required labels.");
  program.add_argument("-d", "--directory")
    .default_value(fs::current_path().string())
    .help("specify the directory to start from.") ;
  program.add_argument("labels")
    .nargs(argparse::nargs_pattern::any)
    .help("the list of labels to search for."); 
  try
   {
    program.parse_args(argc, argv);
   }
  catch (const std::runtime_error& err)
   {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
   }
  fs::path arg_path = fs::absolute(program.get<std::string>("--directory")) ;

  StringVec arg_labels_vec { program.get<StringVec>("labels") } ;
  StringVec arg_labels_tokens, arg_labels_not_tokens ;
  for ( auto & arg_label : arg_labels_vec )
   {
    if (arg_label[0]=='~')
     { arg_labels_not_tokens.emplace_back(arg_label.substr(1)) ; }
    else
     { arg_labels_tokens.emplace_back(arg_label) ; }
   }

  // Other configuration and constant values
  Parameters ps ;
  ps.sep = { "[^[:alnum:]]+" } ;
  ps.camel_start = { "^([[:upper:]][[:lower:][:digit:]]+)([[:upper:]][[:lower:][:digit:]].*)$" } ;

  // Add parameters and load labels from the configuration
  Labels labels ;
  check_cfg(arg_path,ps,labels.primary) ;

  // process the labels tokens
  labels.required = tokens_to_labels(ps,labels.primary,arg_labels_tokens) ;
  labels.forbidden = tokens_to_labels(ps,labels.primary,arg_labels_not_tokens) ;

  // where to store the results
  DirectoryResults results ;

  // Find all labels
  if (std::empty(labels.required))
   {
    scan_dir(ps,labels,arg_path,results,[]( Parameters const & ps, Labels & labels, fs::path const & path, DirectoryResults & )
     {
      StringSet file_labels { scan_file(ps,labels.primary,path) } ;
      for ( auto const & label : file_labels )
       { labels.primary[label].frequence++ ; }
     }) ;

    std::cout<<"\nLABELS:\n " ;
    int i = 0 ;
    for ( const auto & label : labels.primary )
     {
      std::cout<<" "<<label.first ;
      if (!(++i % 10)) std::cout<<"\n " ;
     }
    if (i % 10) std::cout<<"\n" ;

    std::cout<<std::endl ;
    std::exit(0) ;
   }

  // Scan directory
  expand_ancestors(labels.primary) ;
  scan_dir(ps,labels,arg_path,results,[]( Parameters const & ps, Labels & labels, fs::path const & path, DirectoryResults & res )
   {
    StringSet file_labels { scan_file(ps,labels.primary,path) } ;
    if (labels.check(file_labels))
     {
      res.local_selected_files.emplace_back(path.string()) ;
      for ( auto const & label : file_labels )
       {
        labels.primary[label].frequence++ ;
        for ( auto const & ancestor : labels.primary[label].ancestors )
         { labels.primary[ancestor].frequence++ ; }
       }
     }
   });
  results.count_files() ;

  // Find and print sub labels
  StringSet sublabels ;
  for ( const auto & label : labels.primary )
  if ( (label.second.frequence>1) && (label.second.frequence<results.size) )
   { sublabels.emplace(label.first) ; }
  std::cout<<std::format("\nSUBLABELS: {}\n ",sublabels.size()) ;
  int i = 0 ;
  for ( auto const & sublabel : sublabels )
   {
    std::cout<<" "<<sublabel ;
    if (!(++i % 10)) std::cout<<"\n " ;
   }
  if (i % 10) std::cout<<"\n" ;

  // Print files
  std::cout<<std::format("\nFILES: {}\n",results.size) ;
  results.print_files(ps,"  ") ;

  std::cout<<std::endl ;
  std::exit(0) ;
 }
