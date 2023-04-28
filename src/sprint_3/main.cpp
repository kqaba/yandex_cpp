/* 
 
 
 
 
#include <algorithm> 
#include <cmath> 
#include <map> 
#include <set> 
#include <string> 
#include <utility> 
#include <vector> 
 
#include <iostream> 
 
using namespace std; 
 
const int MAX_RESULT_DOCUMENT_COUNT = 5; 
 
string ReadLine() { 
    string s; 
    getline(cin, s); 
    return s; 
} 
 
int ReadLineWithNumber() { 
    int result; 
    cin >> result; 
    ReadLine(); 
    return result; 
} 
 
vector<string> SplitIntoWords(const string& text) { 
    vector<string> words; 
    string word; 
    for (const char c : text) { 
        if (c == ' ') { 
            if (!word.empty()) { 
                words.push_back(word); 
                word.clear(); 
            } 
        } 
        else { 
            word += c; 
        } 
    } 
    if (!word.empty()) { 
        words.push_back(word); 
    } 
 
    return words; 
} 
 
 
struct Document { 
    int id; 
    double relevance; 
    int rating; 
}; 
 
enum class DocumentStatus { 
    ACTUAL, 
    IRRELEVANT, 
    BANNED, 
    REMOVED, 
}; 
 
class SearchServer { 
public: 
    void SetStopWords(const string& text) { 
        for (const string& word : SplitIntoWords(text)) { 
            stop_words_.insert(word); 
        } 
    } 
 
    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) { 
        const vector<string> words = SplitIntoWordsNoStop(document); 
        if (!words.empty() || (document_id < 0) || (documents_.count(document_id) > 0)) 
        { 
            const double inv_word_count = 1.0 / words.size(); 
            for (const string& word : words) { 
                word_to_document_freqs_[word][document_id] += inv_word_count; 
            } 
            documents_.emplace(document_id, 
                DocumentData{ 
                    ComputeAverageRating(ratings), 
                    status 
                }); 
 
        } 
 
    } 
 
    template <typename DocumentPredicate> 
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const { 
 
        const Query query = ParseQuery(raw_query); 
 
        auto matched_documents = FindAllDocuments(query, document_predicate); 
 
        sort(matched_documents.begin(), matched_documents.end(), 
            [](const Document& lhs, const Document& rhs) { 
                if (abs(lhs.relevance - rhs.relevance) < 1e-6) { 
                    return lhs.rating > rhs.rating; 
                } 
                else { 
                    return lhs.relevance > rhs.relevance; 
                } 
            }); 
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) { 
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT); 
        } 
        return matched_documents; 
    } 
 
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus document_status) const { 
        return FindTopDocuments( 
            raw_query, [document_status](int document_id, DocumentStatus status, int rating) { 
                return status == document_status; 
            }); 
    } 
 
    vector<Document> FindTopDocuments(const string& raw_query) const { 
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL); 
    } 
 
 
    int GetDocumentCount() const { 
        return documents_.size(); 
    } 
 
    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const { 
        const Query query = ParseQuery(raw_query); 
        vector<string> matched_words; 
        for (const string& word : query.plus_words) { 
            if (word_to_document_freqs_.count(word) == 0) { 
                continue; 
            } 
            if (word_to_document_freqs_.at(word).count(document_id)) { 
                matched_words.push_back(word); 
            } 
        } 
        for (const string& word : query.minus_words) { 
            if (word_to_document_freqs_.count(word) == 0) { 
                continue; 
            } 
            if (word_to_document_freqs_.at(word).count(document_id)) { 
                matched_words.clear(); 
                break; 
            } 
        } 
        return { matched_words, documents_.at(document_id).status }; 
    } 
 
private: 
    struct DocumentData { 
        int rating; 
        DocumentStatus status; 
    }; 
 
    set<string> stop_words_; 
    map<string, map<int, double>> word_to_document_freqs_; 
    map<int, DocumentData> documents_; 
 
    bool IsStopWord(const string& word) const { 
        return stop_words_.count(word) > 0; 
    } 
 
    static bool IsValidWord(const string& word) { 
        // A valid word must not contain special characters 
        return none_of(word.begin(), word.end(), [](char c) { 
            return c >= '\0' && c < ' '; 
            }); 
    } 
 
    vector<string> SplitIntoWordsNoStop(const string& text) const { 
        vector<string> words; 
        for (const string& word : SplitIntoWords(text)) { 
            if (!IsValidWord(word)) { 
                throw invalid_argument("Word "s + word + " is invalid"s); 
            } 
            if (!IsStopWord(word)) { 
                words.push_back(word); 
            } 
        } 
        return words; 
    } 
 
 
 
    static int ComputeAverageRating(const vector<int>& ratings) { 
        if (ratings.empty()) { 
            return 0; 
        } 
        int rating_sum = 0; 
        for (const int rating : ratings) { 
            rating_sum += rating; 
        } 
        return rating_sum / static_cast<int>(ratings.size()); 
    } 
 
    struct QueryWord { 
        string data; 
        bool is_minus; 
        bool is_stop; 
    }; 
 
    QueryWord ParseQueryWord(string text) const { 
        bool is_minus = false; 
        // Word shouldn't be empty 
        if (text[0] == '-') 
        { 
            is_minus = true; 
            text = text.substr(1); 
        } 
        return 
        { 
            text, 
            is_minus, 
            IsStopWord(text) 
        }; 
    } 
 
    struct Query { 
        set<string> plus_words; 
        set<string> minus_words; 
    }; 
 
    Query ParseQuery(const string& text) const { 
        Query query; 
        for (const string& word : SplitIntoWords(text)) { 
            const QueryWord query_word = ParseQueryWord(word); 
            if (!query_word.is_stop) { 
                if (query_word.is_minus) { 
                    query.minus_words.insert(query_word.data); 
                } 
                else { 
                    query.plus_words.insert(query_word.data); 
                } 
            } 
        } 
        return query; 
    } 
 
    // Existence required 
    double ComputeWordInverseDocumentFreq(const string& word) const { 
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size()); 
    } 
 
    template <typename DocumentPredicate> 
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const { 
        map<int, double> document_to_relevance; 
        for (const string& word : query.plus_words) { 
            if (word_to_document_freqs_.count(word) == 0) { 
                continue; 
            } 
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word); 
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) { 
                const auto& document_data = documents_.at(document_id); 
                if (document_predicate(document_id, document_data.status, document_data.rating)) { 
                    document_to_relevance[document_id] += term_freq * inverse_document_freq; 
                } 
            } 
        } 
 
        for (const string& word : query.minus_words) { 
            if (word_to_document_freqs_.count(word) == 0) { 
                continue; 
            } 
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) { 
                document_to_relevance.erase(document_id); 
            } 
        } 
 
        vector<Document> matched_documents; 
        for (const auto [document_id, relevance] : document_to_relevance) { 
            matched_documents.push_back({ 
                document_id, 
                relevance, 
                documents_.at(document_id).rating 
                }); 
        } 
        return matched_documents; 
    } 
}; 
 
 
 
template <typename T, typename U> 
void AssertEqualImpl(T const& t, U const& u, const string& t_str, const string& u_str, const string& file, 
    const string& func, unsigned line, const string& hint) { 
    if (t != u) { 
        cout << boolalpha; 
        cout << file << "("s << line << "): "s << func << ": "s; 
        cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s; 
        cout << t << " != "s << u << "."s; 
        if (!hint.empty()) { 
            cout << " Hint: "s << hint; 
        } 
        cout << endl; 
        abort(); 
    } 
} 
 
 
template <typename Func> 
void RunTest(const Func& func, const string& func_name) 
{ 
    func(); 
    cerr << func_name << " OK" << endl; 
} 
 
 
 
#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s) 
 
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint)) 
 
void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line, 
    const string& hint) { 
    if (!value) { 
        cout << file << "("s << line << "): "s << func << ": "s; 
        cout << "ASSERT("s << expr_str << ") failed."s; 
        if (!hint.empty()) { 
            cout << " Hint: "s << hint; 
        } 
        cout << endl; 
        abort(); 
    } 
} 
 
#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s) 
 
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint)) 
 
#define RUN_TEST(func) RunTest((func), (#func)) 
 
 
 
*/ 
 
// -------- Начало модульных тестов поисковой системы ---------- 
 
 
void TestExcludeStopWordsFromAddedDocumentContent() { 
    const int doc_id = 42; 
    const string content = "cat in the city"s; 
    const vector<int> ratings = { 1, 2, 3 }; 
    { 
        SearchServer server; 
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings); 
        const auto found_docs = server.FindTopDocuments("in"s); 
        ASSERT_EQUAL(found_docs.size(), 1u); 
        const Document& doc0 = found_docs[0]; 
        ASSERT_EQUAL(doc0.id, doc_id); 
    } 
 
    { 
        SearchServer server; 
        server.SetStopWords("in the"s); 
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings); 
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s); 
    } 
} 
 
////////////////////////////// 
 
 
const string test_document_text = "happy dog walking joyfully"; 
const string test_document_text1 = "dog likes walk and run"; 
const string test_document_text2 = "dog plays"; 
const string test_document_text3 = "dog sits on bed"; 
const string test_document_text4 = "dog was walking with his master at the street"; 
const string test_document_text5 = "lovely dog with big curly tail"; 
const string test_document_text6 = "      "; 
// const string test_document_text6 = "toys for happy dogs with discount"; 
 
constexpr int test_document_id = 1; 
const std::vector<int> test_ratings = { 3, 0, 0, 0 }; 
 
void TestAddingDocumentsToTheServer() 
{ 
    const std::string words_absent_in_test_document = "earth worm"s; 
 
 
    SearchServer server; 
    ASSERT(server.GetDocumentCount() == 0); 
 
    server.AddDocument(test_document_id, test_document_text, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 1, test_document_text1, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 2, test_document_text2, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 3, test_document_text3, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 4, test_document_text4, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 5, test_document_text5, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 6, test_document_text6, DocumentStatus::ACTUAL, test_ratings); 
 
    server.AddDocument(test_document_id + 7, words_absent_in_test_document, DocumentStatus::ACTUAL, 
        test_ratings); 
    
 
    for (const auto& word : SplitIntoWords(test_document_text)) { 
        const auto found_documents = server.FindTopDocuments(word); 
        ASSERT(found_documents.size() > 0); 
    } 
} 
 
 
 
 
 
 
 
void TestMinusWords() { 
    const std::string kQueryWithMinusWords = "dog -walking"s; 
    SearchServer server; 
 
    server.AddDocument(test_document_id, test_document_text, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 1, test_document_text1, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 2, test_document_text2, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 3, test_document_text3, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 4, test_document_text4, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 5, test_document_text5, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 6, test_document_text6, DocumentStatus::ACTUAL, test_ratings); 
 
 
    auto found_documents = server.FindTopDocuments(kQueryWithMinusWords); 
    ASSERT_EQUAL_HINT(found_documents.size(), 4, "Server returns ONLY the documents without minus words, which were in query"s); 
 
 
    found_documents = server.FindTopDocuments("dog walking -walking"); 
    ASSERT_EQUAL(found_documents.size(), 4); 
 
} 
 
 
void TestMatching() { 
    SearchServer server; 
    server.AddDocument(test_document_id, test_document_text, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 1, test_document_text1, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 2, test_document_text2, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 3, test_document_text3, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 4, test_document_text4, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 5, test_document_text5, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(test_document_id + 6, test_document_text6, DocumentStatus::ACTUAL, test_ratings); 
 
    for (int did = 0; did < 6; ++did) 
    { 
        // проверка будет ли искать пустую строку 
        auto [matching_words, _] = server.MatchDocument(""s, test_document_id + did); 
        ASSERT_HINT(matching_words.empty(), "matching empty word for docid"s); 
 
        // проверка будет ли искать слово не из документа 
        std::tie(matching_words, _) = server.MatchDocument("earth worm"s, test_document_id + did); 
        ASSERT_HINT(matching_words.empty(), "matching na words for docid"s); 
 
 
        // находит все слова 
        if (test_document_id + did == 5) 
        { 
            std::tie(matching_words, _) = server.MatchDocument("dog street master"s, test_document_id + did); 
            ASSERT_EQUAL_HINT(matching_words.size(), 3, "matching existing words for docid"s); 
 
 
            std::tie(matching_words, _) = server.MatchDocument("dog -walking"s, test_document_id + did); 
            ASSERT_EQUAL_HINT(matching_words.size(), 0, "no matching minus words for docid"s); 
        } 
    } 
 
} 
 
 
void TestSortByRelevance() { 
    SearchServer server; 
 
    server.AddDocument(1, "dog"s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(2, "dog was"s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(3, "dog was happy"s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(4, "dog was happy to"s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(5, "dog was happy to walk"s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(6, "dog was happy to walk in"s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(7, "dog was happy to walk in park"s, DocumentStatus::ACTUAL, test_ratings); 
 
 
    const auto foundDocuments = server.FindTopDocuments("happy dog park walk"s); 
    const std::vector<int> expectedDocumentIds = { 7, 5, 6, 3, 4 }; 
 
    ASSERT_EQUAL(server.GetDocumentCount(), 7); 
 
    ASSERT_EQUAL(foundDocuments.size(), expectedDocumentIds.size()); 
 
 
    bool is_sorted_by_relevance = 
        std::is_sorted(foundDocuments.begin(), foundDocuments.end(), 
            [](const Document& left, const Document& right) { return left.relevance > right.relevance; }); 
 
    ASSERT_HINT(is_sorted_by_relevance, "sorted by relevance"s); 
} 
 
 
void TestRatingCalculation() { 
    SearchServer server; 
 
    vector<int> test_ratings1 = { 1, 2, 4 }; 
    vector<int> test_ratings2 = { 1, 2, 5 }; 
    vector<int> test_ratings3 = { -1, -2, -4 }; 
    vector<int> test_ratings4 = { -1, -2, -5 }; 
    vector<int> test_ratings5 = { -1, 2, 5 }; 
 
    vector<int> expected_rating_ = { 2, 2, -2,-2,2 }; 
 
    server.AddDocument(1, ""s, DocumentStatus::ACTUAL, test_ratings); 
    server.AddDocument(2, test_document_text, DocumentStatus::ACTUAL, test_ratings1); 
    server.AddDocument(3, test_document_text, DocumentStatus::ACTUAL, test_ratings2); 
    server.AddDocument(4, test_document_text, DocumentStatus::ACTUAL, test_ratings3); 
    server.AddDocument(5, test_document_text, DocumentStatus::ACTUAL, test_ratings4); 
    server.AddDocument(6, test_document_text, DocumentStatus::ACTUAL, test_ratings5); 
 
    for (const Document& document : server.FindTopDocuments(test_document_text)) 
    { 
        ASSERT_EQUAL(document.rating, expected_rating_[document.id - 2]); 
 
    } 
} 
 
 
void TestFindDocumentsWithCustomDocumentStatusFilterFunction() { 
    static const int kDocumentsCount = 3; 
    SearchServer server; 
 
    for (int document_id = test_document_id; document_id < test_document_id + kDocumentsCount; ++document_id) { 
        server.AddDocument(document_id, test_document_text, DocumentStatus::ACTUAL, test_ratings); 
    } 
 
    auto custom_document_filter_function = [](int document_id, DocumentStatus status, int rating) { 
        return document_id > test_document_id; 
    }; 
 
    const auto found_documents = server.FindTopDocuments(test_document_text, custom_document_filter_function); 
    ASSERT_EQUAL_HINT(found_documents.size(), kDocumentsCount - 1, "Server found expected number of documents with custom document filter function"s); 
 
 
} 
 
void TestFindDocumentsWithDefaultDocumentStatusFilterFunction() { 
    { 
        static const int kDocumentStatusCount = 4; 
        static const int kDocumentsCount = 9; 
        std::map<DocumentStatus, std::vector<int>> expected_documents; 
 
        SearchServer server; 
 
        for (int document_id = 0; document_id < kDocumentsCount; ++document_id) { 
            DocumentStatus status = static_cast<DocumentStatus>(document_id % kDocumentStatusCount); 
            server.AddDocument(document_id, test_document_text, status, test_ratings); 
            expected_documents[status].emplace_back(document_id); 
        } 
 
        auto check_status = [=](DocumentStatus status, const std::string& status_string, 
            const std::vector<int>& expected_document_indexes) { 
                const auto documents = server.FindTopDocuments(test_document_text, status); 
 
                std::vector<int> actual_document_indexes; 
                std::transform(documents.begin(), documents.end(), std::back_inserter(actual_document_indexes), 
                    [](const Document& document) { return document.id; }); 
                std::sort(actual_document_indexes.begin(), actual_document_indexes.end()); 
 
                ASSERT_EQUAL_HINT(documents.size(), expected_document_indexes.size(), "Server found correct number of documents"s); 
 
 
                ASSERT_HINT(std::equal(actual_document_indexes.begin(), actual_document_indexes.end(), expected_document_indexes.begin()), 
                    "Server found correct document ID"s); 
 
        }; 
 
        check_status(DocumentStatus::ACTUAL, "ACTUAL", expected_documents[DocumentStatus::ACTUAL]); 
        check_status(DocumentStatus::IRRELEVANT, "IRRELEVANT", expected_documents[DocumentStatus::IRRELEVANT]); 
        check_status(DocumentStatus::BANNED, "BANNED", expected_documents[DocumentStatus::BANNED]); 
        check_status(DocumentStatus::REMOVED, "REMOVED", expected_documents[DocumentStatus::REMOVED]); 
    } 
 
    { 
        SearchServer server; 
        server.AddDocument(test_document_id, test_document_text, DocumentStatus::ACTUAL, test_ratings); 
 
        const auto found_documents = server.FindTopDocuments(test_document_text); 
        ASSERT_EQUAL(found_documents.size(), 1); 
 
        ASSERT_EQUAL_HINT(found_documents[0].id, test_document_id, "Server found correct document with ACTUAL status and IMPLICIT ACTUAL function argument"s); 
 
    } 
} 
 
 
void TestStatus() { 
 
    const int doc_id = 42; 
    const string content = "cat in the city"s; 
    const vector<int> ratings = { 1, 2, 3 }; 
 
    const int doc_id2 = 43; 
    const string content2 = "cat and dog in the home"s; 
    const vector<int> ratings2 = { 3, 3, 3 }; 
 
    const int doc_id3 = 44; 
    const string content3 = "cat and dog in the black box"s; 
    const vector<int> ratings3 = { 4, 4, 4 }; 
 
    { 
        SearchServer server; 
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings); 
        server.AddDocument(doc_id2, content2, DocumentStatus::IRRELEVANT, ratings2); 
        server.AddDocument(doc_id3, content3, DocumentStatus::REMOVED, ratings3); 
        const auto found_docs = server.FindTopDocuments("cat"s, DocumentStatus::IRRELEVANT); 
        ASSERT_EQUAL(found_docs.size(), 1); 
        ASSERT_EQUAL(found_docs[0].id, 43); 
        const auto found_docs2 = server.FindTopDocuments("cat"s, DocumentStatus::REMOVED); 
        ASSERT_EQUAL(found_docs2.size(), 1); 
        ASSERT_EQUAL(found_docs2[0].id, 44); 
 
    } 
 
 
} 
 
// Корректное вычисление релевантности найденных документов 
 
void TestCountingRelevants() { 
 
    const int doc_id = 42; 
    const string content = "cat in the city"s; 
    const vector<int> ratings = { 1, 2, 3 }; 
 
    const int doc_id2 = 43; 
    const string content2 = "cat and dog in the home"s; 
    const vector<int> ratings2 = { 3, 3, 3 }; 
 
    const int doc_id3 = 44; 
    const string content3 = "cat and dog in the black box"s; 
    const vector<int> ratings3 = { 4, 4, 4 }; 
 
    { 
        SearchServer server; 
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings); 
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings); 
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings); 
        const auto found_docs = server.FindTopDocuments("cat dog -city"s, DocumentStatus::ACTUAL); 
        ASSERT_EQUAL(found_docs.size(), 2); 
        ASSERT_EQUAL(found_docs[0].id, 43); 
        ASSERT_EQUAL(found_docs[1].id, 44); 
    } 
} 
 
 
 
 
void TestSearchServer() { 
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent); 
    RUN_TEST(TestAddingDocumentsToTheServer); 
    RUN_TEST(TestMinusWords); 
    RUN_TEST(TestMatching); 
    RUN_TEST(TestSortByRelevance); 
    RUN_TEST(TestRatingCalculation); 
    RUN_TEST(TestFindDocumentsWithCustomDocumentStatusFilterFunction); 
    RUN_TEST(TestFindDocumentsWithDefaultDocumentStatusFilterFunction); 
    RUN_TEST(TestStatus); 
    RUN_TEST(TestCountingRelevants); 
} 
 
// --------- Окончание модульных тестов поисковой системы ----------- 
 
int main() { 
    TestSearchServer(); 
    // Если вы видите эту строку, значит все тесты прошли успешно 
    cout << "Search server testing finished"s << endl; 
}
