enum ITEM{item_torch, item_sword, item_shield, item_key, ITEM_COUNT, NO_ITEM=-1};
enum ENEMY{enemy_goblin, enemy_dragon, ENEMY_COUNT, NO_ENEMY=-1};

struct MAP
{
    std::vector<int> north_exits;
    std::vector<int> east_exits;
    std::vector<int> south_exits;
    std::vector<int> west_exits;
    std::vector<int> locked_doors;
    std::vector<ITEM> room_items;
    std::vector<ENEMY> room_enemies;
    int spawn_room;
};

MAP load_map(std::string filepath);
std::vector<std::string> ssplit(const std::string&, char);
std::vector<int> isplit(const std::string&, char);
int toint(std::string);
