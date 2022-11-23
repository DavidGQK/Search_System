#pragma once
#include <vector>
#include <string>
#include <set>


std::vector<std::string_view> SplitIntoWords(std::string_view text);

using StringSet = std::set<std::string, std::less<>>;

template <typename StringContainer>
StringSet MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    StringSet non_empty_strings;
    for (std::string_view str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(std::string(str));
        }
    }
    return non_empty_strings;
}