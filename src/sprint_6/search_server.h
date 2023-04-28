#pragma once    
 
#include <algorithm>    
#include <map>    
#include <set>    
#include <string>    
#include <utility>    
#include <vector>    
#include <cmath>    
#include "document.h"    
#include "string_processing.h"    
 
const int MAX_RESULT_DOCUMENT_COUNT = 5; 
class SearchServer { 
public: 
    template <typename StringContainer> 
    explicit SearchServer(const StringContainer& stop_words) 
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words    
    { 
        using namespace std::literals; 
        if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) { 
 
            throw std::invalid_argument("Some of stop words are invalid"s); 
        } 
    } 
    explicit SearchServer(const std::string& stop_words_text) 
        : SearchServer(SplitIntoWords(stop_words_text)) 
    { 
    } 
    std::set<int>::const_iterator begin() const; 
    std::set<int>::const_iterator end() const; 
    const std::map<std::string, double>& GetWordFrequencies(int document_id) const; 
    void RemoveDocument(int document_id); 
    void AddDocument(int document_id, const std::string& document, DocumentStatus status, 
        const std::vector<int>& ratings); 
    template <typename DocumentPredicate> 
    std::vector<Document> FindTopDocuments(const std::string& raw_query, 
        DocumentPredicate document_predicate) const; 
 
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status) const; 
    std::vector<Document> FindTopDocuments(const std::string& raw_query) const; 
    int GetDocumentCount() const; 
 
    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, 
        int document_id) const; 
private: 
    struct DocumentData { 
        int rating; 
        DocumentStatus status; 
    }; 
    const std::set<std::string> stop_words_; 
    std::map<std::string, std::map<int, double>> word_to_document_freqs_; 
    std::map<int, std::map<std::string, double>> document_to_word_freqs_; 
    std::map<int, DocumentData> documents_; 
    std::set<int> document_ids_; 
    bool IsStopWord(const std::string& word) const { 
        return stop_words_.count(word) > 0; 
    } 
    static bool IsValidWord(const std::string& word) { 
        return none_of(word.begin(), word.end(), [](char c) { 
            return c >= '\0' && c < ' '; 
            }); 
    } 
    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const; 
    static int ComputeAverageRating(const std::vector<int>& ratings) { 
        int rating_sum = 0; 
        for (const int rating : ratings) { 
            rating_sum += rating; 
        } 
        return rating_sum / static_cast<int>(ratings.size()); 
    } 
 
    struct QueryWord { 
        std::string data; 
        bool is_minus; 
        bool is_stop; 
    }; 
    QueryWord ParseQueryWord(const std::string& text) const { 
        using namespace std::literals; 
        if (text.empty()) { 
            throw std::invalid_argument("Слово запроса пусто"s); 
        } 
        std::string word = text; 
        bool is_minus = false; 
        if (word[0] == '-') { 
            is_minus = true; 
            word = word.substr(1); 
        } 
        if (word.empty() || word[0] == '-' || !IsValidWord(word)) { 
            throw std::invalid_argument("Слово запроса "s + text + " недопустимо"); 
        } 
        return { word, is_minus, IsStopWord(word) }; 
    } 
    struct Query { 
        std::set<std::string> plus_words; 
        std::set<std::string> minus_words; 
    }; 
    Query ParseQuery(const std::string& text) const; 
    double ComputeWordInverseDocumentFreq(const std::string& word) const { 
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size()); 
    } 
 
    template <typename DocumentPredicate> 
    std::vector<Document> FindAllDocuments(const Query& query, 
        DocumentPredicate document_predicate) const; 
}; 
template <typename DocumentPredicate> 
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, 
    DocumentPredicate document_predicate) const { 
    const auto query = ParseQuery(raw_query); 
 
    auto matched_documents = FindAllDocuments(query, document_predicate); 
 
    std::sort(matched_documents.begin(), matched_documents.end(), 
        [](const Document& lhs, const Document& rhs) { 
            return lhs.relevance > rhs.relevance 
                || (std::abs(lhs.relevance - rhs.relevance) < 1e-6 && lhs.rating > rhs.rating); 
        }); 
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) { 
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT); 
    } 
    return matched_documents; 
} 
template <typename DocumentPredicate> 
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, 
    DocumentPredicate document_predicate) const { 
    std::map<int, double> document_to_relevance; 
    for (const std::string& word : query.plus_words) { 
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
    for (const std::string& word : query.minus_words) { 
        if (word_to_document_freqs_.count(word) == 0) { 
            continue; 
        } 
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) { 
            document_to_relevance.erase(document_id); 
        } 
    } 
 
    std::vector<Document> matched_documents; 
    for (const auto [document_id, relevance] : document_to_relevance) { 
        matched_documents.push_back( 
            { document_id, relevance, documents_.at(document_id).rating }); 
    } 
    return matched_documents; 
} 
