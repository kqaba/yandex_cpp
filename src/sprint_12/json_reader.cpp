#include "json_reader.h" 
#include "map_renderer.h" 
 
#include <sstream>  
 
using namespace std::literals; 
 
namespace json_reader { 
    void FillStopsByRequestBody(transport_catalogue::TransportCatalogue& transport_catalogue, const json::Node& request_body)  
    { 
        geo::Coordinates coordinates{ request_body.AsMap().at("latitude"s).AsDouble(), request_body.AsMap().at("longitude"s).AsDouble() }; 
        transport_catalogue.AddStop(request_body.AsMap().at("name"s).AsString(), coordinates); 
    } 
 
    void FillStopToStopDistances(transport_catalogue::TransportCatalogue& transport_catalogue, const json::Node& stop_to_stop_distance)  
    { 
        for (const auto& [second_stop_name, distance] : stop_to_stop_distance.AsMap().at("road_distances"s).AsMap())  
        { 
            transport_catalogue.SetStopToStopDistances(stop_to_stop_distance.AsMap().at("name"s).AsString(), second_stop_name, distance.AsDouble()); 
        } 
    } 
 
    void FillRoutesByRequestBody(transport_catalogue::TransportCatalogue& transport_catalogue, const json::Node& add_buses_request)  
    { 
        std::vector<std::string> stops; 
        for (const auto& stop : add_buses_request.AsMap().at("stops"s).AsArray())  
        { 
            stops.push_back(stop.AsString()); 
        } 
        transport_catalogue.AddBus(add_buses_request.AsMap().at("name"s).AsString(), stops, (add_buses_request.AsMap().at("is_roundtrip"s).AsBool())); 
    } 
 
 
    void GenerateResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses)  
    { 
        if (request_body.AsMap().at("type"s).AsString() == "Stop"s) { 
            AddStopInfoResponse(request_handler, request_body, responses); 
        } 
        if (request_body.AsMap().at("type"s).AsString() == "Bus"s) { 
            AddBusInfoResponse(request_handler, request_body, responses); 
        } 
        if (request_body.AsMap().at("type"s).AsString() == "Map"s) { 
            AddMapInfoResponse(request_handler, request_body, responses); 
        } 
    } 
 
    void AddStopInfoResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses) 
    { 
        json::Builder builder; 
 
        std::optional<transport_catalogue::StopInfo> stop_info = request_handler.GetStopInfo(request_body.AsMap().at("name"s).AsString()); 
 
        if (stop_info && stop_info.value().is_empty == false)  
        { 
            json::Array stop_buses; 
            for (const auto& bus : stop_info.value().stop_buses)  
            { 
                std::string string_result = { bus.begin(), bus.end() }; 
                stop_buses.push_back(string_result); 
            } 
            builder.StartDict() 
                .Key("buses"s).Value(stop_buses) 
                .Key("request_id"s).Value(request_body.AsMap().at("id"s).AsInt()) 
                .EndDict(); 
             
            responses.push_back(builder.Build().AsMap()); 
            return; 
        } 
 
        builder.StartDict() 
            .Key("request_id"s).Value(request_body.AsMap().at("id"s).AsInt()) 
            .Key("error_message"s).Value("not found"s) 
            .EndDict(); 
        responses.push_back(builder.Build().AsMap()); 
        return; 
    } 
 
    void AddBusInfoResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses) 
    { 
        json::Builder builder; 
        std::optional<transport_catalogue::BusInfo> bus_info = request_handler.GetBusInfo(request_body.AsMap().at("name"s).AsString()); 
        if (bus_info && bus_info.value().is_empty == false)  
        { 
            builder.StartDict() 
                .Key("curvature"s).Value(bus_info.value().curvature) 
                .Key("request_id"s).Value(request_body.AsMap().at("id"s).AsInt()) 
                .Key("route_length"s).Value(bus_info.value().route_length) 
                .Key("stop_count"s).Value(static_cast<int>(bus_info.value().count_all_stops)) 
                .Key("unique_stop_count"s).Value(static_cast<int>(bus_info.value().count_unique_stops)) 
                .EndDict(); 
 
            responses.push_back(builder.Build().AsMap()); 
            return; 
        } 
        builder.StartDict() 
            .Key("request_id"s).Value(request_body.AsMap().at("id"s).AsInt()) 
            .Key("error_message"s).Value("not found"s) 
            .EndDict(); 
 
        responses.push_back(builder.Build().AsMap()); 
        return; 
    } 
 
    void AddMapInfoResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses) 
    { 
        json::Builder builder; 
 
        std::ostringstream strm; 
        request_handler.RenderMap(strm); 
        std::string svg_line = strm.str(); 
        builder.StartDict() 
            .Key("map"s).Value(strm.str()) 
            .Key("request_id"s).Value(request_body.AsMap().at("id"s).AsInt()) 
            .EndDict(); 
 
        responses.push_back(builder.Build().AsMap()); 
        return; 
    } 
 
    Json_Reader::Json_Reader(std::istream& input) 
        : doc_(json::Load(input)) { 
    } 
 
    const json::Array& Json_Reader::GetBaseRequests() const { 
        return doc_.GetRoot().AsMap().at("base_requests"s).AsArray(); 
    } 
 
    const json::Dict& Json_Reader::GetRenderSettings() const { 
        return doc_.GetRoot().AsMap().at("render_settings"s).AsMap(); 
    } 
 
    const json::Array& Json_Reader::GetStatRequests() const { 
        return doc_.GetRoot().AsMap().at("stat_requests"s).AsArray(); 
    } 
 
    void Json_Reader::AddRouteCoordinates(geo::Coordinates coordinates) { 
        route_coordinates_.push_back(coordinates); 
    } 
 
    const std::vector<geo::Coordinates>& Json_Reader::GetRoutesCoordinate() const { 
        return route_coordinates_; 
    } 
 
    void Json_Reader::AddStopToStopRequest(const json::Node& node) { 
        requests_.stop_requests.push_back(node); 
    } 
 
    void Json_Reader::AddBusRequest(const json::Node& node) { 
        requests_.bus_requests.push_back(node); 
    } 
 
    const Requests& Json_Reader::GetRequests() const { 
        return requests_; 
    } 
 
    void Json_Reader::AddBusName(const std::string& bus_name) { 
        buses_names_.insert(bus_name); 
    } 
 
    const std::set<std::string>& Json_Reader::GetBusNames() const { 
        return buses_names_; 
    } 
 
    void SequentialRequestProcessing(transport_catalogue::TransportCatalogue& transport_catalogue, MapRenderer& map_render, std::istream& input, std::ostream& output, request_handler::RequestHandler& request_handler) 
    { 
        Json_Reader json_reader(input); 
 
        for (const auto& request_body : json_reader.GetBaseRequests()) 
        { 
            if (request_body.AsMap().at("type"s).AsString() == "Stop"s) 
            { 
                FillStopsByRequestBody(transport_catalogue, request_body); 
                json::Builder builder; 
                builder.StartDict() 
                    .Key("name"s) 
                    .Value(request_body.AsMap().at("name"s).AsString()) 
                    .Key("road_distances"s) 
                    .Value(request_body.AsMap().at("road_distances"s)) 
                    .EndDict(); 
 
                json_reader.AddStopToStopRequest(builder.Build().AsMap()); 
                json_reader.AddRouteCoordinates({ request_body.AsMap().at("latitude"s).AsDouble(), request_body.AsMap().at("longitude"s).AsDouble() }); 
                continue; 
            } 
 
            if (request_body.AsMap().at("type"s).AsString() == "Bus"s) 
            { 
                json_reader.AddBusRequest(request_body); 
            } 
        } 
 
        for (const auto& add_stop_to_stop_distance_request : json_reader.GetRequests().stop_requests) 
        { 
            FillStopToStopDistances(transport_catalogue, add_stop_to_stop_distance_request); 
        } 
 
 
        for (const auto& add_buses_request : json_reader.GetRequests().bus_requests) 
        { 
            FillRoutesByRequestBody(transport_catalogue, add_buses_request); 
            json_reader.AddBusName(add_buses_request.AsMap().at("name"s).AsString()); 
 
        } 
 
        MapVisualizationSettings settings(json_reader.GetRenderSettings().at("width"s).AsDouble(), json_reader.GetRenderSettings().at("height"s).AsDouble(), json_reader.GetRenderSettings().at("padding"s).AsDouble()); 
        auto route_coordinates = request_handler.GetCoordinates(); 
        SphereProjector projector(route_coordinates.begin(), route_coordinates.end(), settings.max_width, settings.max_height, settings.padding); 
        map_render.SetPossibleColors(json_reader.GetRenderSettings().at("color_palette"s).AsArray()); 
 
        for (const auto& bus : json_reader.GetBusNames()) { 
            const auto& b = request_handler.GetBus(bus); 
            if (b != nullptr) { 
                size_t pos = (b->stops.size()) / 2; 
                map_render.AddNewPointByRouteName(bus, projector(b->stops.front()->coordinates), json_reader.GetRenderSettings()); 
                map_render.AddNewTextForRoute(bus, projector(b->stops.front()->coordinates), json_reader.GetRenderSettings()); 
                map_render.AddNewCircleForStop(b->stops.front()->name, projector(b->stops.front()->coordinates), json_reader.GetRenderSettings()); 
                map_render.AddNewTextForStop(b->stops.front()->name, projector(b->stops.front()->coordinates), json_reader.GetRenderSettings()); 
                if (b->is_circle == false && b->stops.at(pos) != b->stops.front()) { 
                    map_render.AddNewTextForRoute(bus, projector(b->stops.at(pos)->coordinates), json_reader.GetRenderSettings()); 
                } 
                size_t i = 1; 
                for (; i < b->stops.size() - 1; ++i) { 
                    map_render.AddNewPointByRouteName(bus, projector(b->stops.at(i)->coordinates), json_reader.GetRenderSettings()); 
                    map_render.AddNewCircleForStop(b->stops.at(i)->name, projector(b->stops.at(i)->coordinates), json_reader.GetRenderSettings()); 
                    map_render.AddNewTextForStop(b->stops.at(i)->name, projector(b->stops.at(i)->coordinates), json_reader.GetRenderSettings()); 
                } 
                map_render.AddNewPointByRouteName(bus, projector(b->stops.back()->coordinates), json_reader.GetRenderSettings()); 
                map_render.ChangeCurrentColor(); 
            } 
        } 
  
        json::Array responses; 
 
        for(const auto& request_body : json_reader.GetStatRequests()) { 
            GenerateResponse(request_handler, request_body, responses); 
        } 
 
        json::Print(json::Document{ responses }, output); 
    } 
} 
