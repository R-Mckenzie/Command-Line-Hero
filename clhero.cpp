#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include "map_loader.h"

#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define RESET "\e[0m"

std::string item_names[ITEM_COUNT]={"torch", "sword", "shield", "key"};
std::string enemy_names[ENEMY_COUNT]={"goblin", "dragon"};

struct PLAYER
{
    std::string name;
    int position;
    bool inventory[ITEM_COUNT];
};

enum QUIT_CONDITION{quit, win, died};

//creates a player with an empty inventory and given spawn point
PLAYER init_player(int spawn);
//returns true if input == w, a, s or d
bool move_commanded(std::string input);
//returns true and moves player if there is an available room in the selected direction
int move_room(PLAYER in_player, MAP in_map, char move_direction);
//takes in a series of flags and builds an room description string from them
std::string build_description(MAP in_map, int new_room, bool has_torch);
//begins another input loop within the main one, to capture whether the player
//wants to fight or run
bool enemy_present(MAP, int);
//Returns true if the player wins the fight
bool fight(ENEMY);
//returns true if won, false if player ran
bool fight_sequence(PLAYER, MAP);
//exits the game with an appropriate message depending on quit_condition parameter.
void exit_game(QUIT_CONDITION condition);

int main()
{
    MAP map=load_map("map_1.map");
    PLAYER player=init_player(map.spawn_room);

    std::string input="";
    std::cout<<GREEN<<" >";
    while(std::getline(std::cin, input)){
        std::cout<<RESET;
        if(input=="q"){
            exit_game(quit);
        }

        int previous_position=player.position;
        if(input=="w"||input=="a"||input=="s"||input=="d"){
            player.position=move_room(player, map, input[0]);
            std::cout<<'\n';
            std::cout<<"position: "<<player.position<<'\n';

            if(previous_position!=player.position){
                std::cout<<build_description(map, player.position, player.inventory[item_torch]); 
                if(map.room_enemies[player.position]!=NO_ENEMY){
                    if(!fight_sequence(player, map)){
                        player.position=previous_position; 
                   }
                }
            }else{
                std::cout<<"You find a wall in your way.\n";
            }
        } 
        std::cout<<GREEN<<" >";
    }

    return 0;
}

PLAYER init_player(int spawn)
{
    PLAYER player;
    player.position=spawn;
    for(int i=0; i<ITEM_COUNT; i++)
        player.inventory[i]=NO_ITEM;
    std::cout<<"Greetings, adventurer. What is your name?\n "<<GREEN<<">";
    std::cin>>player.name;
    std::cout<<RESET<<"Welcome, "<<player.name<<'\n';
    std::cin.get();
    return player;
}

int move_room(PLAYER in_player, MAP in_map, char move_direction)
{
    switch(move_direction){
        case 'w':
            if(in_map.north_exits[in_player.position]!=-1){
                return in_map.north_exits[in_player.position]; 
            }
            break;
        case 'a':
            if(in_map.west_exits[in_player.position]!=-1){
                return in_map.west_exits[in_player.position]; 
            }
            break;
        case 's':
            if(in_map.south_exits[in_player.position]!=-1){
                return in_map.south_exits[in_player.position]; 
            }
            break;
        case 'd':
            if(in_map.east_exits[in_player.position]!=-1){
                return in_map.east_exits[in_player.position]; 
            }
            break;
        default:
            std::cerr<<"INVALID INPUT SENT TO 'move_room'\n";
    }
    //There is no available room in the selected direction, so we return the
    //original position
    return in_player.position;
}

std::string build_description(MAP map, int new_room, bool has_torch)
{
    std::string description="";

    if(has_torch){
        description="You carefully walk into the next room.\n";
        if(map.room_enemies[new_room]!=NO_ENEMY){
            std::string enemy_name=enemy_names[map.room_enemies[new_room]];
            description+="There is a ";
            description+=RED;
            description+=enemy_name;
            description+=RESET;
            description+=" In the middle of the room!\n";
        }if(map.room_items[new_room]!=NO_ITEM){
            std::string item_name=item_names[map.room_items[new_room]];
            description+="In the corner of the room you can see a ";
            description+=YELLOW;
            description+=item_name;
            description+=RESET;
            description+=" In the middle of the room!\n";
        }
    }else{
        description="You manage to stumble through the darkness into the next room.\n";
        if(map.room_enemies[new_room]!=NO_ENEMY)
            description+="You hear an ear splitting shriek coming from somewhere in the room...\n";
    }
    return description;
}

bool fight(PLAYER in_player, ENEMY opponent)
{
    switch(opponent){
        case enemy_goblin:
            if(in_player.inventory[item_sword]){
                std::cout<<"The "<<RED<<"goblin"<<RESET<<" charges you...\n";
                std::cout<<"You sidestep it and swing your sword...\n";
                std::cout<<"You have won the battle!\n";
                return true;
            }else{
                //50:50 chance
            }
            break;
        case enemy_dragon:
            if(in_player.inventory[item_sword]&&in_player.inventory[item_shield]){
                std::cout<<"The "<<RED<<"dragon"<<RESET<<" rears up and breathes a stream of fire at you...\n";
                std::cout<<"You hide behind your shield and manage to avoid it...\n";
                std::cout<<"The "<<RED<<"dragon"<<RESET<<" swoops down and tries to bite you...\n";
                std::cout<<"You raise your sword and stab it in the neck!\n";
                
                return true;
            }else{
                //lose
            }
            break;
        default:
            break;
    }
    exit_game(died);
    return false;
}

bool fight_sequence(PLAYER in_player, MAP in_map)
{
    std::cout<<"Do you want to fight the monster?\n(y/n)\n";
    std::cout<<GREEN<<" >";
    std::string input;
    while(std::getline(std::cin, input))
    {
        std::cout<<RESET;
        if(input=="y"){
            std::cout<<"\nYou prepare to fight...\n";
            if(fight(in_player, in_map.room_enemies[in_player.position])){
                return true;
                break;
            }
        }else if(input=="n"){
            std::cout<<"You run back through the way you came...\n";
            return false;
        }else{
            std::cout<<GREEN<<" >";
            continue;
        }
    }
    return false;
}

void exit_game(QUIT_CONDITION condition)
{
    std::cout<<'\n';
    switch(condition){
        case quit:
            std::cout<<"You have successfully quit the game.\nThanks for playing!"<<'\n';
            std::cout<<"------------------------------------"<<'\n';
            break;
        case win:
            std::cout<<"You have killed the dragon and completed the game!"<<'\n';
            std::cout<<"Congratulations!"<<'\n';
            std::cout<<"------------------------------------"<<'\n';
            break;
        case died:
            std::cout<<"You have died."<<'\n';
            std::cout<<"GAME OVER"<<'\n';
            std::cout<<"----------------"<<'\n';
            break;
        default:
            std::cerr<<"INVALID QUIT CONDITION\n";
    }
    std::cout<<'\n';
    exit(0);
}
