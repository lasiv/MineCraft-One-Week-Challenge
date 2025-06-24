#ifndef STRUCTURE_H_INCLUDED
#define STRUCTURE_H_INCLUDED

#include <string>
#include <vector>
#include "../../World.h"
#include "../../Block/BlockId.h"

class Structure {
    public:
        Structure(std::string file_name);
        void generate_structure();
        void print_info();
    private:
        std::string name;
        int id;
        std::vector<Block_t> layers;
        int dimensions[3];
};

void load_all_structures();

#endif