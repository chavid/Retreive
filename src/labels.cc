

#include "labels.hh"


//=============================================================================
// Internal utility functions
//=============================================================================

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
    for ( auto const & parent : label.second.parents )
     { expand_parent(labels,parent,label.second) ; }
   }
 }

std::set<std::string> process_synonyms( PrimaryLabels const & cfg_labels, std::set<std::string> const & labels )
 {
  std::set<std::string> res ;
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

std::set<std::string> Labels::words_to_labels( std::vector<std::string> const & words ) const
 {
  // regular expressions
  std::regex sep { "[^[:alnum:]]+" } ;
  std::regex camel_start { "^([[:upper:]][[:lower:][:digit:]]+)([[:upper:]][[:lower:][:digit:]].*)$" } ;

  // split on non-words characters plus
  std::vector<std::string> tokens2 ;
  for ( std::string token1 : words )
   {
    std::istringstream iss{ std::regex_replace(token1,sep," ") } ;
    std::string token2 ;
    while (iss>>token2)
     { tokens2.emplace_back(token2) ; }
   }

  // CamelCase
  std::vector<std::string> tokens3 ;
  for ( std::string token2 : tokens2 )
   {
    std::smatch m ;
    while ( std::regex_match(token2,m,camel_start) )
     {
      tokens3.emplace_back(m[1].str()) ;
      token2 = m[2].str() ;
     }
    tokens3.emplace_back(token2) ;
   }

  // lower
  std::set<std::string> labels ;
  for ( std::string token3 : tokens3 )
   {
    lower(token3) ;
    labels.emplace(token3) ;
   }

  // synonyms
  std::set<std::string> primary_labels ;
  for ( auto const & label : labels )
   {
    // it is already a main static label
    if (primary_.contains(label))
     { primary_labels.emplace(label) ; }
    // is it in the synonyms ?
    else
     {
      bool found = false ;
      for ( auto const & primary_label : primary_ )
      if ( primary_label.second.synonyms.contains(label) )
       { primary_labels.emplace(primary_label.first) ; found = true ; break ; }
      // dynamic label
      if(!found)
       { primary_labels.emplace(label) ; }
     }
   }

  // final step
  return primary_labels ;
 }


//=============================================================================
// Labels setters
//=============================================================================

Labels::Labels( Configuration const & cfg )
 {
  // "same"
  cfg.apply("==",[this]( auto name, auto values )
   {
    auto label { name } ;
    lower(label) ;
    for ( auto value : values )
     {
      lower(value) ;
      primary_[label].synonyms.insert(value) ;
     }
   }) ;
  
  // interpret config "imply"
  cfg.apply("=>",[this]( auto name, auto values )
   {
    auto child { name } ;
    lower(child) ;
    for ( auto parent : values )
     {
      lower(parent) ;
      primary_[child].parents.insert(parent) ;
      primary_[parent].children.insert(child) ;
     }
   }) ;

  // post process the labels
  expand_ancestors(primary_) ;
 }

void Labels::required( std::vector<std::string> const & words )
 { required_.merge(words_to_labels(words)) ; }

void Labels::forbidden( std::vector<std::string> const & words )
 { forbidden_.merge(words_to_labels(words)) ; }

void Labels::primary( std::vector<std::string> const & words )
 {
  for ( auto const & label : words_to_labels(words) )
   {
    primary_[label].frequence++ ;
    for ( auto const & ancestor : primary_[label].ancestors )
     { primary_[ancestor].frequence++ ; }
   }
 }


//=============================================================================
// Labels check
//=============================================================================

bool Labels::check( std::vector<std::string> const & words, bool count )
 {
  // process the words into labels
  auto labels = words_to_labels(words) ;

  // a single forbidden label is enough to reject the file
  for ( auto const & flabel : forbidden_ )
  for ( auto const & label : labels )
   {
    if (label==flabel)
     { return false ; }
    if (primary_.contains(label))
    if (primary_.at(label).ancestors.contains(flabel))
     { return false ; }
   }

  // all required labels must be present
  for ( auto const & rlabel : required_ )
   {
    bool found = false ;
    for ( auto const & label : labels )
     {
      if (label==rlabel)
      { found = true ; break ; }
      if (primary_.contains(label))
      if (primary_.at(label).ancestors.contains(rlabel))
      { found = true ; break ; }
     }
    if (!found)
     { return false ; }
   }

  // count
  if (count)
  for ( auto const & label : labels )
   {
    primary_[label].frequence++ ;
    for ( auto const & ancestor : primary_[label].ancestors )
     { primary_[ancestor].frequence++ ; }
   }

  // end
  return true ;
 }


//=============================================================================
// Labels getters
//=============================================================================

bool Labels::empty() const
 { return (required_.empty()&&forbidden_.empty()) ; }

void Labels::print_by_10( std::string const & title, auto filter ) const
 {
  // apply filter
  std::set<std::string> all_labels ;
  for ( const auto & [ name, data ] : primary_ )
  if (filter(data))
   { all_labels.insert(name) ; }

  // favor ancestors (to reduce the number of results)
  std::set<std::string> top_labels ;
  for ( const auto & label : all_labels )
   {
    bool ancestor_found = false ;
    for ( const auto & ancestor : primary_.at(label).ancestors )
    if (all_labels.contains(ancestor))
     { ancestor_found = true ; break ; }
    if (!ancestor_found)
     { top_labels.insert(label) ; }
   }

  // print
  std::cout<<"\n"<<title<<": "<<top_labels.size()<<"\n " ;
  int i = 0 ;
  for ( const auto & label : top_labels )
   {
    std::cout<<" "<<label ;
    if (!(++i % 10)) std::cout<<"\n " ;
   }
  if (i % 10) std::cout<<"\n" ;
 }

void Labels::print_found() const
 {
  print_by_10("LABELS",[]( PrimaryLabelData const & data )
   { return (data.frequence>0) ; }) ;
 }

void Labels::print_sub_labels( std::size_t nb_results ) const
 {
  print_by_10("SUBLABELS",[nb_results]( PrimaryLabelData const & data )
   { return ((data.frequence>1)&&(data.frequence<nb_results)) ; }) ;
 }
