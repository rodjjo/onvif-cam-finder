/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include <utility>
#include <regex>  // NOLINT google likes re2
#include "cam_finder/cam_finder_parsers.h"


namespace camfinder {
namespace parser {

namespace {
    std::regex regex_token(".*token[ ]*=[ ]*\"([^\"]+)\".*");
}

std::string parse_token(const std::string& line) {
    std::cmatch base_match;
    auto matched = std::regex_match(line.c_str(), base_match, regex_token);
    if (matched) {
        return base_match[1].str();
    }
    return std::string();
}

profile_list_t get_profiles(const std::string& xml) {
    std::list<std::string> splited_profiles = parser_all(
        xml.c_str(), "<trt:Profiles", "</trt:Profiles");
    profile_list_t result;

    const char *lt;
    const char *p;
    std::string resolution;

    for (const std::string& profile : splited_profiles) {
        p = profile.c_str();
        lt = strstr(p, ">");
        if (!lt) continue;
        profile_t item;
        item.token = parse_token(std::string(p, lt));
        if (item.token.empty()) continue;
        resolution = parser_first(p, "<tt:Resolution>", "</tt:Resolution>");
        item.width = parser_first(
            resolution.c_str(), "<tt:Width>", "</tt:Width>");
        item.height = parser_first(
            resolution.c_str(), "<tt:Height>", "</tt:Height>");
        item.fps = parser_first(
            p, "<tt:FrameRateLimit>", "</tt:FrameRateLimit>");
        item.name = parser_first(lt, "<tt:Name>", "</tt:Name>");
        result.push_back(item);
    }

    return result;
}

stream_info_t get_stream_info(
    const profile_t& profile,
    const std::string& xml
) {
    stream_info_t result;
    result.stream_uri = parser_first(xml.c_str(), "<tt:Uri>", "</tt:Uri>");
    if (!result.stream_uri.empty()) {
        result.profile_name = profile.name;
        result.profile_token = profile.token;
        result.width = profile.width;
        result.height = profile.height;
        result.fps = profile.fps;
    }
    return result;
}

std::string parser_first(
    const char *data,
    const char *open_tag,
    const char *close_tag
) {
    const char *p1 = strstr(data, open_tag);
    if (p1) {
        data = p1;
        p1 = strstr(data, close_tag);
        if (p1) {
            return std::string(data + strlen(open_tag), p1);
        }
    }
    return std::string();
}

const char* parse_next(const char* last, const char* open_tag) {
    if (last) {
        const char *p1 = strstr(last, open_tag);
        if (p1) {
            return p1 + strlen(open_tag);
        }
    }
    return NULL;
}

std::list<std::string> parser_all(
    const char* data,
    const char* open_tag,
    const char *close_tag
) {
    std::list<std::string> result;
    const char *next = parse_next(data, open_tag);
    const char *end = NULL;
    while (next) {
        end = strstr(next, close_tag);
        if (!end) {
            break;
        }
        result.push_back(std::string(next, end));
        next = parse_next(end + strlen(close_tag), open_tag);
    }
    return result;
}

}  // namespace parser
}  // namespace camfinder
