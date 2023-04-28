#include "remove_duplicates.h" 
 
using std::string_literals::operator""s; 
 
void RemoveDuplicates(SearchServer& search_server) 
{ 
	std::map<std::set<std::string>, std::vector<int>> doc_delete; 
	std::set<int> remove_documents; 
	for (const int document_id : search_server) 
	{ 
		std::set<std::string> words; 
		std::map<std::string, double> word_with_freq = search_server.GetWordFrequencies(document_id); 
		for (auto [word, freq] : word_with_freq) 
		{ 
			words.insert(word); 
		} 
 
		if (doc_delete.count(words) == 0) 
		{ 
			doc_delete[words].push_back(document_id); 
		} 
		else 
		{ 
			remove_documents.insert(document_id); 
		} 
	} 
 
	 for (const int document_id : remove_documents) { 
        std::cout << "Found duplicate document id " << document_id << std::endl; 
        search_server.RemoveDocument(document_id); 
    } 
} 
