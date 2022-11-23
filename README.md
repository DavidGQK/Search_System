# Search_System
Document search engine with the ability to specify negative keywords that are not taken into account in the search results. </br>

About the system:

- The search engine can be initialized with stop words.
- If there are no plus words in the query, nothing will be found
- If the same word is in the query and with a minus, it is considered a negative keyword
- The result is ranked by **TF-IDF**
- The function of finding documents has sequence and parallel versions
- Rating is based on users data activity, here it is initialized artificially
- C++ Language Standard - ISO C++20

For example, the system has the following documents: 
1. `white cat and yellow hat`
2. `curly cat curly tail`
3. `nasty dog with big eyes`
4. `nasty pigeon john`

The query `cat` will find documents 1 and 2. </br>
The query `curly -nasty -cat` returns nothing because `-cat` excluded documents 1 and 2 and `-nasty` excluded documents 3 and 4. Word order doesn't matter. </br>


## Example
### Query:
```C++
    SearchServer search_server("and with"s); // initializing with stop words

    int id = 0;
    for (
        const string& text : {
            "white cat and yellow hat"s,
            "curly cat curly tail"s,
            "nasty dog with big eyes"s,
            "nasty pigeon john"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    cout << "ACTUAL by default:"s << endl;
    // sequence version
    for (const Document& document : search_server.FindTopDocuments("curly nasty cat"s)) {
        PrintDocument(document);
    }

    cout << "BANNED:"s << endl;
    // sequence version
    for (const Document& document : search_server.FindTopDocuments(execution::seq, "curly nasty cat and"s, DocumentStatus::BANNED)) {
        PrintDocument(document);
    }

    cout << "Even ids:"s << endl;
    // parallel version
    for (const Document& document : search_server.FindTopDocuments(execution::par, "curly nasty cat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }
 ```
### Result:
```
ACTUAL by default:
{ document_id = 2, relevance = 0.866434, rating = 1 }
{ document_id = 4, relevance = 0.231049, rating = 1 }
{ document_id = 1, relevance = 0.173287, rating = 1 }
{ document_id = 3, relevance = 0.173287, rating = 1 }
BANNED:
Even ids:
{ document_id = 2, relevance = 0.866434, rating = 1 }
{ document_id = 4, relevance = 0.231049, rating = 1 }
```
---
