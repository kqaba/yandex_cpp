#pragma once 
 
#include <cassert> 
#include <execution> 
#include <string> 
#include <unordered_map> 
#include <vector> 
#include <list> 
#include <deque> 
#include <tuple> 
#include <optional> 
 
#include "geo.h" 
 
namespace catalogue { 
 
    enum RouteType { 
        Linear, 
        Circle, 
    }; 
 
    namespace info { 
        struct Stop { 
            std::string stop_name; 
            std::vector<std::string> buses_; 
        }; 
 
        struct Bus { 
            std::string bus_name; 
            size_t stops_on_route = 0; 
            size_t unique_stops = 0; 
            double r_length = 0.0; 
            double curvature = 0.0; 
        }; 
    } 
 
    class TransportCatalogue { 
        struct Stop { 
            std::string name; 
            Coordinates coordinates; 
 
            bool operator==(const std::string& name) const; 
            bool operator==(const Stop&) const; 
        }; 
 
        struct Bus { 
            std::string name; 
            RouteType type; 
            std::vector<Stop*> stops; 
 
            bool operator==(const std::string& name) const; 
            bool operator==(const Bus&) const; 
        }; 
 
        struct StopToStopHasher { 
            size_t operator()(const std::pair<Stop*, Stop*> stops) const { 
                return std::hash<const void*>{}(stops.first) + std::hash<const void*>{}(stops.second); 
            } 
        }; 
 
    public: 
        void AddStop(const std::string_view name, const Coordinates); 
 
        //template<typename Container> 
        void AddBus(const std::string_view name, const RouteType type, const std::deque<std::string_view>& stops); 
 
        void AddDistance(const std::string_view from, const std::string_view to, const double distance); 
        std::optional<Stop*> FindStop(const std::string_view) const; 
        std::optional<Bus*> FindBus(const std::string_view) const; 
        std::optional<info::Stop*> GetStopByName(const std::string_view) const; 
        std::optional<info::Bus*> GetBusByName(const std::string_view) const; 
 
    private: 
        std::deque<Stop> stops; 
        std::deque<Bus> buses; 
 
        std::unordered_map<std::string_view, Stop*> stopname_to_stop; 
        std::unordered_map<std::string_view, Bus*> busname_to_bus; 
 
        std::unordered_map<std::string_view, std::deque<std::string_view>> stop_to_buses; 
 
        std::unordered_map<std::pair<Stop*, Stop*>, double, StopToStopHasher> stops_to_distance; 
    }; 
 
} 
