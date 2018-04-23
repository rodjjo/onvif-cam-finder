/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef CAM_FINDER_CAM_FINDER_PARSERS_H_
#define CAM_FINDER_CAM_FINDER_PARSERS_H_

#include <string>
#include <list>

namespace camfinder {
namespace parser {

std::list<std::string> get_profile_tokens(const std::string& xml);
std::string get_stream_uri(const std::string& xml);

}  // namespace parser
}  // namespace camfinder

#endif  // CAM_FINDER_CAM_FINDER_PARSERS_H_
