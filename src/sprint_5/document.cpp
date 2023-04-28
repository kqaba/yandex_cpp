#include "document.h" 
 
 
    std::ostream& operator<<(std::ostream& os, const Document& document) { 
        using namespace std; 
 
        return os << "{ document_id = "s << document.id << ", "s 
            << "relevance = "s << document.relevance << ", "s 
            << "rating = "s << document.rating << " }"s; 
    } 
