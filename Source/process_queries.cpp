#include "process_queries.h"
#include "search_server.h"
#include "document.h"

#include <vector>
#include <algorithm>
#include <execution>


std::vector<std::vector<Document>> ProcessQueries(const SearchServer& search_server, const std::vector<std::string>& queries) {
	std::vector<std::vector<Document>> query_results(queries.size());

	std::transform(
		std::execution::par, queries.begin(), queries.end(), query_results.begin(),
		[&search_server](std::string query) {
			return search_server.FindTopDocuments(query);
		}
	);
	return query_results;
}

std::list<Document> ProcessQueriesJoined(const SearchServer& search_server, const std::vector<std::string>& queries) {
	std::list<Document> query_results;

	for (auto const& document : ProcessQueries(search_server, queries)) {
		query_results.splice(query_results.end(), std::list(document.begin(), document.end()));
	}
	return query_results;
}