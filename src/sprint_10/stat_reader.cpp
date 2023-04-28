#include "stat_reader.h" 
#include <cassert> 
#include <iomanip> 
 
using namespace::catalogue::reader; 
 
static const std::string BusQueryIdentifier = "Bus"; 
static const std::string StopQueryIdentifier = "Stop"; 
static const std::string DistanceSeparator = " to "; 
 
void StatReader::ReadQueues( std::istream& stream) const 
{ 
    size_t queue_count = 0; 
    stream >> queue_count; 
 
    while (queue_count-- > 0) { 
        std::string queue; 
        std::getline(stream, queue); 
        if (queue.empty()) { 
            ++queue_count; 
            continue; 
        } 
 
        if (auto iter = queue.find(BusQueryIdentifier); iter != std::string::npos) { 
            PrintBus(queue.substr(iter + BusQueryIdentifier.size()), std::cout); 
        } 
        else if (auto iter = queue.find(StopQueryIdentifier); iter != std::string::npos) { 
            PrintStop(queue.substr(iter + StopQueryIdentifier.size()), std::cout); 
        } 
 
    } 
} 
 
void StatReader::PrintBus(const std::string_view queue, std::ostream& out) const 
{ 
    const std::string_view name = Simplified(queue); 
    const auto& info = this->catalogue.GetBusByName(name); 
    out << "Bus " << name << ": "; 
    if (info == std::nullopt) { 
        std::cout << "not found"; 
    } 
    else { 
        catalogue::info::Bus* info_bus = info.value(); 
        out << info_bus->stops_on_route << " stops on route, "; 
        out << info_bus->unique_stops << " unique stops, "; 
        out << std::setprecision(6) << info_bus->r_length << " route length, "; 
        out << std::setprecision(6) << info_bus->curvature << " curvature"; 
    } 
    out << std::endl; 
} 
 
void StatReader::PrintStop(const std::string_view queue, std::ostream& out) const 
{ 
    const std::string_view name = Simplified(queue); 
    const auto& info = this->catalogue.GetStopByName(name); 
    out << "Stop " << name << ": "; 
    if (info == std::nullopt) { 
        out << "not found"; 
    } 
    else { 
        catalogue::info::Stop* info_stop = info.value(); 
        const auto& buses = info_stop->buses_; 
        if (buses.empty()) { 
            out << "no buses"; 
        } 
        else { 
            out << "buses"; 
            for (const auto& bus : buses) { 
                out << ' ' << bus; 
            } 
        } 
    } 
    out << std::endl; 
} 
 
std::string_view StatReader::Simplified(const std::string_view text) const 
{ 
    assert(!text.empty()); 
 
    auto left = text.find_first_not_of(' '); 
    auto right = text.find_last_not_of(' '); 
 
    assert(left != std::string::npos); 
 
    return text.substr(left, right - left + 1); 
} 
