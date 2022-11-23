#pragma once
#include "document.h"
#include "search_server.h"

#include <vector>
#include <string>
#include <deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        ++time_;

        if (!requests_.empty() && ((time_ - requests_.front().time_created) >= kSecondsInADay)) {
            requests_.pop_front();
        }

        const std::vector<Document>& results = server_.FindTopDocuments(raw_query, document_predicate);

        if (results.empty()) {
            requests_.push_back(QueryResult(time_));
        }

        return results;
    };

    std::vector<Document> AddFindRequest(const std::string& raw_query,
        DocumentStatus status = DocumentStatus::ACTUAL);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        QueryResult(uint64_t current_time) : time_created(current_time) {}
        uint64_t time_created;
    };

    static constexpr int kSecondsInADay = 1440;
    std::deque<QueryResult> requests_;
    const SearchServer& server_;
    uint64_t time_ = 0;
};