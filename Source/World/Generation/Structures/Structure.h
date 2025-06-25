#ifndef STRUCTURE_H_INCLUDED
#define STRUCTURE_H_INCLUDED

#include <string>
#include <vector>
#include "../../World.h"
#include "../../Block/BlockId.h"

class Structure {
    public:
        Structure(const Structure& other);
        Structure(std::string file_name);
        void generate_structure(Chunk* chunk, sf::Vector3i center);
        void print_info();
        inline int get_id() {
            return id;
        }
    private:
        std::string name;
        int id;
        std::vector<Block_t> layers;
        int dimX, dimY, dimZ;
};

#endif