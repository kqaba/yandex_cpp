#pragma once 
 
#include <iostream> 
#include <stdio.h> 
#include <string> 
#include <queue> 
#include "transport_catalogue.h" 
 
namespace catalogue { 
    namespace reader { 
        class StatReader { 
        public: 
            StatReader(TransportCatalogue& catalogue, std::istream& stream = std::cin) : catalogue{ catalogue }, stream{ stream } {}; 
 
            void ReadQueues( std::istream & = std::cin) const; 
 
            void PrintBus(const std::string_view queue, std::ostream& out) const; 
            void PrintStop(const std::string_view queue, std::ostream& out) const; 
 
        private: 
            std::string_view Simplified(const std::string_view) const; 
 
            TransportCatalogue& catalogue; 
            std::istream& stream; 
        }; 
    } 
} 
