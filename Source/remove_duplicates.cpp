#include "remove_duplicates.h"


using namespace std::literals;

void RemoveDuplicates(SearchServer& search_server) {
    SearchServer add_server = search_server;
    std::set<std::set<std::string>> worlds_in_document;
    size_t worlds_count = 0;

    for (const auto& id : add_server) {
        worlds_in_document.insert(ExtractOfKeys(add_server.GetWordFrequencies(id)));
        if (worlds_in_document.size() == worlds_count) {
            cout << "Found duplicates document id "s << id << endl;
            search_server.RemoveDocument(id);
        }
        else {
            ++worlds_count;
        }
    }
}