
#include "cfg.hh"

Configuration::Configuration( std::string const & file_name, fs::path const & additional_directory_path )
 {
  // list of files to parse
  std::vector<std::string> cfg_paths ;

  // home ?
  fs::path home_path { std::getenv("HOME") } ;
  if (fs::is_directory(home_path))
   {
    home_path.append(file_name) ;
    if (fs::is_regular_file(home_path))
     { cfg_paths.emplace_back(home_path) ; }
   }

  // pwd ?
  fs::path pwd_path { std::getenv("PWD") } ;
  if (fs::is_directory(pwd_path))
   {
    pwd_path.append(file_name) ;
    if (fs::is_regular_file(pwd_path))
     { cfg_paths.emplace_back(pwd_path) ; }
   }

  // arg_path
  fs::path optional_path { additional_directory_path } ;
  if (fs::is_directory(additional_directory_path))
   {
    optional_path.append(file_name) ;
    if (optional_path!=pwd_path)
    if (fs::is_regular_file(optional_path))
     { cfg_paths.emplace_back(optional_path) ; }
   }

  // parse the files
  if (!cfg_paths.empty())
   {
    std::cout<<"\nCFG:\n" ;
    for ( auto const & cfg_path : cfg_paths )
     { parse(cfg_path) ; }
   }
 }

void Configuration::parse( std::string const & cfg_path )
 {
  std::cout<<"  "<<cfg_path<<std::endl ;
  std::ifstream ifs(cfg_path) ;
  std::string line, word ;
  while (std::getline(ifs,line))
   {
    // remove comments
    auto sharp_pos = line.find('#') ;
    if (sharp_pos!=std::string::npos)
     { line.erase(sharp_pos) ; } 
    if (line.empty()) continue ;
    
    // parse the line
    std::istringstream iss(line) ;
    StringSet lhs, rhs ;
    std::string actual_op ;
    while (iss>>word)
     {
      // operator ?
      std::string found_op ;
      for ( auto op : ops )
      if (word==op)
       { found_op = word ; break ; }
      if (!found_op.empty())
       {
        if (!actual_op.empty())
         { throw std::runtime_error(std::format("Multiple operators: {}",line)) ; }
        actual_op = found_op ;
        continue ;
       }
      // lhs & rhs
      if (actual_op.empty())
       { lhs.insert(word) ; }
      else
       { rhs.insert(word) ; }
     }
    if (actual_op.empty())
      { throw std::runtime_error(std::format("Lacking known operator: {}",line)) ; }
    if (std::empty(lhs))
      { throw std::runtime_error(std::format("Lacking left hand parameter: {}",line)) ; }
    if (std::empty(rhs))
      { throw std::runtime_error(std::format("Lacking right hand parameter: {}",line)) ; }
    for (auto const & lh : lhs)
    for (auto const & rh : rhs)
     { data_[actual_op][lh].insert(rh) ; }
   }
 }
