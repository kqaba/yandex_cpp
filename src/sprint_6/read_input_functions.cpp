#include "read_input_functions.h"   
#include <iostream>  
#include <vector>  
using namespace std; 
string ReadLine() { 
    string s; 
    getline(cin, s); 
    return s; 
} 
int ReadLineWithNumber() { 
    int result = 0; 
    cin >> result; 
    ReadLine(); 
    return result; 
} 
vector<int> ReadLineOfNumbers() { 
    int numbers_count = 0; 
    cin >> numbers_count; 
    vector<int> numbers(numbers_count, 0); 
    for (auto& num : numbers) { 
        cin >> num; 
    } 
    ReadLine(); 
    return numbers; 
} 
