#ifndef STRUCTURE_H_INCLUDED
#define STRUCTURE_H_INCLUDED

#include <string>
#include "../World.h"

class Structure {
    public:
        Structure(std::string file_name);
        void generate_structure();
};

#endif