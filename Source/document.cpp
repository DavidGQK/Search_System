#include "document.h"


using namespace std::literals;

Document::Document(int id, double relevance, int rating)
    : id(id)
    , relevance(relevance)
    , rating(rating) {
};

std::ostream& operator<<(std::ostream& out, const Document& document) {
    out << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s;
    return out;
}

std::ostream& operator<<(std::ostream& out, const DocumentStatus& status) {
    switch (status) {
        case DocumentStatus::ACTUAL:
            out << "ACTUAL"s;
            break;
        case DocumentStatus::IRRELEVANT:
            out << "IRRELEVANT"s;
            break;
        case DocumentStatus::BANNED:
            out << "BANNED"s;
            break;
        case DocumentStatus::REMOVED:
            out << "REMOVED"s;
            break;
    }
    return out;
}