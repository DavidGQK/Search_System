#include "string_processing.h"


using namespace std::literals;

std::vector<std::string_view> SplitIntoWords(std::string_view text) {
    std::vector<std::string_view> words;
    std::string_view t_text = text; t_text.remove_prefix(std::min(t_text.size(), t_text.find_first_not_of(" ")));
    auto pos = t_text.find(' ', 0);
    for (; !t_text.empty();) {
        if (pos == 0) {
            t_text.remove_prefix(pos + 1);
            pos = t_text.find(' ', 0);
            continue;
        }
        words.push_back(t_text.substr(0, pos == t_text.npos ? t_text.npos : pos));
        if (pos == t_text.npos) break;
        t_text.remove_prefix(std::min(t_text.size(), pos + 1));
        pos = t_text.find(' ', 0);
    }
    return words;
}