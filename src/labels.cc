

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

std::set<std::string> Labels::words_to_labels( std::vector<std::string> const & words ) const
 {
  // transformation pipeline
  auto labels = words
   | vw::transform([]( std::string iword ) // split on non-words characters
     {
      std::regex sep { "[^[:alnum:]]+" } ;
      std::vector<std::string> owords ;
      std::istringstream iss{ std::regex_replace(iword,sep," ") } ;
      std::string word ;
      while (iss>>word)
       { owords.emplace_back(word) ; }
      return owords ;
     })
   | vw::join
   | vw::transform([]( std::string iword ) // CamelCase split
     {
      std::regex camel_start { "^([[:upper:]][[:lower:][:digit:]]+)([[:upper:]][[:lower:][:digit:]].*)$" } ;
      std::vector<std::string> owords ;
      std::smatch m ;
      while ( std::regex_match(iword,m,camel_start) )
       {
        owords.emplace_back(m[1].str()) ;
        iword = m[2].str() ;
       }
      owords.emplace_back(iword) ;
      return owords ;
     })
   | vw::join
   | vw::filter([]( std::string const & word )     // remove single characters
     {
      return (std::size(word)>1) ;
     })
   | vw::transform([]( std::string const & word )  // lower case
     {
      return lower(word) ;
     })
   | vw::transform([&]( std::string const & label ) // synonyms
     {
      // it is a primary label ?
      if (primary_.contains(label))
       { return label ; }
      else
       {
        // is it in the synonyms ?
        for ( auto const & primary_label : primary_ )
        if ( primary_label.second.synonyms.contains(label) )
         { return primary_label.first ; }
        // dynamic label
        return label ;
       }
     }) ;

  // final step (waiting for availability of std::ranges::to)
  std::set<std::string> res ;
  for ( auto const & label : labels )
   { res.insert(label) ; }
  return res ;
 }


//=============================================================================
// Labels setters
//=============================================================================

Labels::Labels( Configuration const & cfg )
 {
  // "same"
  cfg.apply("==",[this]( auto name, auto values )
   {
    auto label { lower(name) } ;
    for ( auto value : values )
     { primary_[label].synonyms.insert(lower(value)) ; }
   }) ;
  
  // interpret config "imply"
  cfg.apply("=>",[this]( auto name, auto values )
   {
    auto child { lower(name) } ;
    for ( auto value : values )
     {
      auto parent { lower(value) } ;
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

bool Labels::check( std::set<std::string> const & labels ) const
 {
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

  // end
  return true ;
 }

void Labels::count( std::set<std::string> const & labels )
 {
  for ( auto const & label : labels )
   {
    primary_[label].frequence++ ;
    for ( auto const & ancestor : primary_[label].ancestors )
     { primary_[ancestor].frequence++ ; }
   }
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

void Labels::print_suggested_labels( std::size_t nb_results ) const
 {
  print_by_10("SUGGESTED LABELS",[nb_results]( PrimaryLabelData const & data )
   { return ((data.frequence>1)&&(data.frequence<nb_results)) ; }) ;
 }
