#include "search_server.h"


using namespace std::literals;

SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text)) {  // Invoke delegating constructor from string container
    if (!IsValidWord(stop_words_text)) {
        throw std::invalid_argument("Text contain invalid symbols"s);
    }
}

SearchServer::SearchServer(std::string_view stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text)) {
    if (!IsValidWord(stop_words_text)) {
        throw std::invalid_argument("Text contain invalid symbols"s);
    }
}

void SearchServer::AddDocument(int document_id, const std::string_view& document, DocumentStatus status, const std::vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw std::invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);

    const double inv_word_count = 1.0 / words.size();
    for (const std::string_view& word : words) {
        word_to_document_freqs_[std::string(word)][document_id] += inv_word_count;
        document_ids_with_word_[document_id][std::string(word)] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    document_ids_.insert(document_id);
}

std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int id, DocumentStatus document_status, int rating) {
        return document_status == status;
        });
}

std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return static_cast<int>(documents_.size());
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::string_view& raw_query, int document_id) const {
    return MatchDocument(std::execution::seq, raw_query, document_id);
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::sequenced_policy&, const std::string_view& raw_query, int document_id) const {
    // LOG_DURATION_STREAM("Operation time MatchDocument"s, std::cerr);
    const auto query = ParseQuery(raw_query);
    for (const std::string_view& word : query.minus_words) {
        if (word_to_document_freqs_.count(std::string(word)) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(std::string(word)).count(document_id)) {
            return { {}, documents_.at(document_id).status };
        }
    }
    std::vector<std::string_view> matched_words;
    for (const std::string_view& word : query.plus_words) {
        if (word_to_document_freqs_.count(std::string(word)) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(std::string(word)).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::parallel_policy&, const std::string_view& raw_query, int document_id) const {
    //LOG_DURATION_STREAM("Operation time MatchDocument"s, std::cerr);
    const auto query = ParseQuery(raw_query);
    const auto word_checker = [this, document_id](const std::string_view word) {
        const auto it = word_to_document_freqs_.find(std::string(word));
        return it != word_to_document_freqs_.end() && it->second.count(document_id);
    };

    if (std::any_of(std::execution::par, query.minus_words.begin(), query.minus_words.end(), word_checker)) {
        return { {}, documents_.at(document_id).status };
    };

    std::vector<std::string_view> matched_words(query.plus_words.size());
    auto words_end = std::copy_if(
        std::execution::par,
        query.plus_words.begin(), query.plus_words.end(),
        matched_words.begin(),
        word_checker
    );

    std::sort(std::execution::par, matched_words.begin(), words_end);
    words_end = std::unique(matched_words.begin(), words_end);
    matched_words.erase(words_end, matched_words.end());
    return { matched_words, documents_.at(document_id).status };
}

std::set<int>::iterator SearchServer::begin() {
    return document_ids_.begin();
}

std::set<int>::iterator SearchServer::end() {
    return document_ids_.end();
}

const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static std::map<std::string, double> word_frequencies_;

    auto found_document = document_ids_with_word_.find(document_id);

    if (found_document == document_ids_with_word_.end()) { //invalid id
        return word_frequencies_;
    }
    else {
        word_frequencies_ = found_document->second;
        return word_frequencies_;
    }
}

void SearchServer::RemoveDocument(int document_id) {
    if (!document_ids_.count(document_id)) {
        return;
    }

    for (const auto& [word, freq] : document_ids_with_word_.at(document_id)) {
        word_to_document_freqs_.at(word).erase(document_id);
    }
    document_ids_.erase(document_id);
    document_ids_with_word_.erase(document_id);
    documents_.erase(document_id);
}

bool SearchServer::IsStopWord(const std::string_view word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string_view word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

std::vector<std::string_view> SearchServer::SplitIntoWordsNoStop(const std::string_view& text) const {
    std::vector<std::string_view> words;
    for (const std::string_view& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Word "s + std::string(word) + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const std::string_view& text) const {
    if (text.empty()) {
        throw std::invalid_argument("Query word is empty"s);
    }
    std::string_view word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        throw std::invalid_argument("Query word "s + std::string(text) + " is invalid");
    }

    return { word, is_minus, IsStopWord(word) };
}

SearchServer::Query SearchServer::ParseQuery(const std::string_view& text) const {
    Query result;
    for (const std::string_view& word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.insert(query_word.data);
            }
            else {
                result.plus_words.insert(query_word.data);
            }
        }
    }
    return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(const std::string_view& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(std::string(word)).size());
}