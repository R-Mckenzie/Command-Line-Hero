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
//prints instructions to the game
void print_instructions();
//returns true if input == w, a, s or d
bool move_commanded(std::string input);
//returns true and moves player if there is an available room in the selected direction
int move_room(PLAYER in_player, MAP in_map, char move_direction);
//takes in a series of flags and builds an room description string from them
std::string build_description(MAP in_map, int new_room, bool has_torch);
void print_inventory(PLAYER);
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
    print_instructions();
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
                    if(!player.inventory[item_torch])
                        exit_game(died);
                    if(!fight_sequence(player, map)){
                        player.position=previous_position; 
                    }else{
                        map.room_enemies[player.position]=NO_ENEMY;
                    }
                }
                ITEM room_item=map.room_items[player.position];
                //TODO: Find a way to DRY this
                if(room_item!=NO_ITEM ){
                    if(!player.inventory[item_torch] && room_item==item_torch){
                        player.inventory[room_item]=true;
                        map.room_items[player.position]=NO_ITEM;
                        std::cout<<"You find a "<<YELLOW<<item_names[room_item]<<RESET" burning in the room. You see its use and take it with you.\n";
                    }
                    else{
                        player.inventory[room_item]=true;
                        map.room_items[player.position]=NO_ITEM;
                        std::cout<<"You pick up the "<<YELLOW<<item_names[room_item]<<RESET".\n";
                    }
                }
            }else{
                std::cout<<"You find a wall in your way.\n";
            }
        } 

        if(input=="i"){
            print_inventory(player);
        }
        std::cout<<GREEN<<" >";
    }

    return 0;
}

PLAYER init_player(int spawn)
{
    PLAYER player;
    player.position=spawn;
    for(int i=0; i<ITEM_COUNT; i++){
        player.inventory[i]=false;
    }
    std::cout<<"Greetings, adventurer. What is your name?\n "<<GREEN<<">";
    std::cin>>player.name;
    std::cout<<RESET<<"Welcome, "<<player.name<<'\n';
    std::cin.get();
    return player;
}

void print_instructions()
{
    std::cout<<"Input w,a,s,d to move around the map.\nInput 'i' to see your inventory.\n";
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

void print_inventory(PLAYER in_player)
{
    std::cout<<"This is your inventory:\n"<<YELLOW;
    for(int i=0;i<ITEM_COUNT;i++){
        if(in_player.inventory[i]){
            std::cout<<item_names[i]<<'\n';
        }
    }
    std::cout<<RESET;
}

bool fight(PLAYER in_player, ENEMY opponent)
{
    switch(opponent){
        case enemy_goblin:
            std::cout<<"The "<<RED<<"goblin"<<RESET<<" charges you...\n";
            if(in_player.inventory[item_sword]){
                std::cout<<"You sidestep it and swing your sword...\n";
                std::cout<<"You have won the battle!\n";
                return true;
            }else{
                int win_decider=rand();
                if(win_decider%2==0){
                    //WIN
                    std::cout<<"You get lucky and it trips!\n";
                    std::cout<<"You don't give it a chance to get back up...\n";
                    return true;
                }else{
                    std::cout<<"You don't get out of its way...\nMaybe if you had found a sword you could have done something.\n";
                }
            }
            break;
        case enemy_dragon:
            std::cout<<"The "<<RED<<"dragon"<<RESET<<" rears up and breathes a stream of fire at you...\n";
            if(in_player.inventory[item_shield]){
                std::cout<<"You hide behind your shield and manage to avoid it!\n";
                std::cout<<"The "<<RED<<"dragon"<<RESET<<" swoops down and tries to bite you...\n";
                if(in_player.inventory[item_sword]){
                    std::cout<<"You raise your sword and stab it in the neck!\n";
                    exit_game(win);
                }
                std::cout<<"You have nothing to stop it with. Maybe if you had found a sword...\n";
            }
            std::cout<<"You have nothing to hide behind, and are caught directly in the stream of fire...\n";
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
