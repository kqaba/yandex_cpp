#include "search_server.h"   
#include "document.h"   
 
#include <cmath>   
#include <iostream>   
#include <numeric>   
#include <stdexcept>   
#include <string>    
#include <bitset>   
#include <optional> 
 
 
 
void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, 
    const std::vector<int>& ratings) { 
    using namespace std::literals; 
    if ((document_id < 0) || (documents_.count(document_id) > 0)) { 
        throw std::invalid_argument("Неверный документ айди"s); 
    } 
    const auto words = SplitIntoWordsNoStop(document); 
    const double inv_word_count = 1.0 / words.size(); 
    for (const std::string& word : words) { 
        word_to_document_freqs_[word][document_id] += inv_word_count; 
        document_to_word_freqs_[document_id][word] += inv_word_count; 
    } 
    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status }); 
    document_ids_.insert(document_id); 
} 
 
std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const { 
    std::vector<std::string> words; 
    using namespace std::literals; 
    for (const std::string& word : SplitIntoWords(text)) { 
        if (!IsValidWord(word)) { 
            throw std::invalid_argument("слово "s + word + " недопустимо"s); 
        } 
        if (!IsStopWord(word)) { 
            words.push_back(word); 
        } 
    } 
    return words; 
} 
std::set<int>::const_iterator SearchServer::begin() const 
{ 
    return document_ids_.cbegin(); 
} 
 
std::set<int>::const_iterator SearchServer::end() const 
{ 
    return document_ids_.cend(); 
} 
const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const 
  { 
    static const std::map<std::string, double> empty_map{}; 
 
    if (document_to_word_freqs_.count(document_id)==1) 
    { 
        return document_to_word_freqs_.at(document_id); 
    } 
    else return 
        empty_map; 
} 
void SearchServer::RemoveDocument(int document_id) 
{  
    if (documents_.count(document_id == 0)) 
    { 
        return; 
    } 
    documents_.erase(document_id); 
    document_ids_.erase(document_id); 
    auto word_freq = GetWordFrequencies(document_id); 
    if (word_freq.empty()) 
    { 
        return; 
    } 
    for (const auto& [word, freq] : word_freq) 
    { 
        word_to_document_freqs_[word].erase(document_id); 
        if (word_to_document_freqs_[word].empty()) 
        { 
            word_to_document_freqs_.erase(word); 
        } 
    } 
     
       
} 
 
SearchServer::Query SearchServer::ParseQuery(const std::string& text) const { 
    Query result; 
    for (const std::string& word : SplitIntoWords(text)) { 
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
 
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const { 
    return FindTopDocuments( 
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) { 
            return document_status == status; 
        }); 
} 
 
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const { 
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL); 
} 
 
int SearchServer::GetDocumentCount() const { 
    return document_ids_.size(); 
} 
 
std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, 
    int document_id) const { 
    const auto query = ParseQuery(raw_query); 
    std::vector<std::string> matched_words; 
    for (const std::string& word : query.plus_words) { 
        if (word_to_document_freqs_.count(word) == 0) { 
            continue; 
        } 
        if (word_to_document_freqs_.at(word).count(document_id)) { 
            matched_words.push_back(word); 
        } 
    } 
    for (const std::string& word : query.minus_words) { 
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
