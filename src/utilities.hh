
#pragma once


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Utilities useful to the whole project
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//==============================================================================
// C specific includes
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/fnmatch.html
//==============================================================================

#include <fnmatch.h> // for bash-like global pattern matching


//==============================================================================
// C++ standard library includes
//==============================================================================

#include <cstdlib>
#include <cassert>

#include <string>
#include <string_view>

#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

#include <array>
#include <vector>
#include <set>
#include <unordered_map>

#include <algorithm>
#include <ranges>

#include <filesystem>
#include <regex>


//==============================================================================
// Shortcuts
//==============================================================================

namespace fs = std::filesystem ;
namespace rg = std::ranges ;
namespace vw = std::views ;

using StringVec = std::vector<std::string> ;
using StringSet = std::set<std::string> ;
using StringMap = std::unordered_map<std::string,StringSet> ;

using StringViewVec = std::vector<std::string_view> ;
using StringViewSet = std::set<std::string_view> ;

using Files = std::vector<fs::path> ;


//==============================================================================
// Utility functions
//==============================================================================

std::string lower( std::string ) ;

