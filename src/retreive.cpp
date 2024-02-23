
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

#include "argparse.hpp"
#include "utilities.hh"
#include "cfg.hh"
#include "parameters.hh"
#include "labels.hh"
#include "scan.hh"

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

  // Interpret arguments
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

  // Configuration
  Configuration cfg(".labels",arg_path) ;
  Parameters ps(cfg) ;
  Labels labels(cfg) ;
  labels.required(arg_labels_tokens) ;
  labels.forbidden(arg_labels_not_tokens) ;

  // Recursively scan the top directory
  DirectoryScan top_dir(arg_path) ;
  if (labels.empty())
   {
    top_dir.scan_for_labels(ps,labels) ;
    labels.print_primary() ;
   }
  else
   {
    top_dir.scan_for_files(ps,labels) ;
    labels.print_sub_labels(top_dir.size()) ;
    top_dir.print_files(ps) ;
   }

  // That's all folks
  std::cout<<std::endl ;
  std::exit(0) ;
 }
