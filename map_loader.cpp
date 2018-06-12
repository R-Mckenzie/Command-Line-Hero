#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "map_loader.h"

MAP load_map(std::string filepath)
{
    MAP out_map;
    std::ifstream in_file;
    in_file.open(filepath);
    std::string line;

    while(std::getline(in_file, line)){
        if(line=="") 
            continue;

        std::vector<std::string> elements=ssplit(line, ':');
        if(elements[0]=="spawn"){
            out_map.spawn_room=toint(elements[1]);
        }
        if(elements[0]=="exits"){
            std::vector<int> exits=isplit(elements[1], ',');
            out_map.north_exits.push_back(exits[0]);
            out_map.east_exits.push_back(exits[1]);
            out_map.south_exits.push_back(exits[2]);
            out_map.west_exits.push_back(exits[3]);
        }
        else if(elements[0]=="locked"){
            out_map.locked_doors.push_back(toint(elements[1]));
        }
        else if(elements[0]=="item"){
            std::string s=elements[1];
            if(s=="NONE")
                out_map.room_items.push_back(NO_ITEM);
            else if(s=="torch")
                out_map.room_items.push_back(item_torch);
            else if(s=="sword")
                out_map.room_items.push_back(item_sword);
            else if(s=="shield")
                out_map.room_items.push_back(item_shield);
            else if(s=="key")
                out_map.room_items.push_back(item_key);
            else
                std::cerr<<"ERROR: "<<s<<" IS NOT A RECOGNISED ITEM\n";
        }
        else if(elements[0]=="enemy"){
            std::string s=elements[1];
            if(s=="NONE")
                out_map.room_enemies.push_back(NO_ENEMY);
            else if(s=="goblin")
                out_map.room_enemies.push_back(enemy_goblin);
            else if(s=="dragon")
                out_map.room_enemies.push_back(enemy_dragon);
            else
                std::cerr<<"ERROR: "<<s<<" IS NOT A RECOGNISED ENEMY\n";
        }
    }
    return out_map;
}

std::vector<std::string> ssplit(const std::string& in_string, char delimiter)
{
    std::vector<std::string> out_vector;
    std::string token;
    std::istringstream token_stream(in_string);
    while(std::getline(token_stream, token, delimiter))
        out_vector.push_back(token);
    return out_vector;
}

std::vector<int> isplit(const std::string& in_string, char delimiter){
    std::vector<int> out_vector;
    std::string token;
    std::istringstream token_stream(in_string);
    while(std::getline(token_stream, token, delimiter))
        out_vector.push_back(toint(token));
    return out_vector;
}

int toint(std::string in_string)
{
    return atoi(in_string.c_str());
}
