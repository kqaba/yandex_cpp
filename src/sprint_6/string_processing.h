#pragma once  
#include <set>  
#include <string>  
#include <vector>  
#include <stdexcept>  
 
    bool IsValidWord(const std::string& word);  
    std::vector<std::string> SplitIntoWords(const std::string& text);  
    template <typename StringContainer>  
    std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {  
        using namespace std::literals;  
        std::set<std::string> non_empty_strings;  
        for (const auto& str : strings) {  
              if (!str.empty())  
                non_empty_strings.insert(str); 
               else  
               throw std::invalid_argument("Недопустимое слово в документе: "s + str);  
              
        }  
        return non_empty_strings;  
    }  
 
