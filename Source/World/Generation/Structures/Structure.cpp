#include "Structure.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

Structure::Structure(std::string fileName)
{
    std::ifstream inFile("Res/Structures/" + fileName + ".structure");

    if (!inFile.is_open()) {
        throw std::runtime_error("Unable to open block file: " + fileName +
                                 "!");
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line == "Name") {
            std::getline(inFile, this->name);
        }
        else if (line == "Id") {
            inFile >> this->id;
        }
        else if (line == "Dimensions") {
            inFile >> this->dimX >> this->dimY >> this->dimZ;
        }
        else if (line == "Layers") {
            layers.resize(dimX*dimY*dimZ);
            
            for(int y = 0; y < dimY; y++) {
                for(int z = 0; z < dimZ; z++) {
                    for(int x = 0; x < dimX; x++) {
                        int index = y * (dimX*dimZ) + z * (dimX) + x;
                        int val;
                        inFile >> val;
                        layers[index] = (Block_t)(val);
                    }
                }
            }
        }
    }
}

Structure::Structure(const Structure& other)
{
    this->name = other.name;
    this->id = other.id;
    this->layers = other.layers;
    this->dimX = other.dimX;
    this->dimY = other.dimY;
    this->dimZ = other.dimZ;
}

void Structure::generate_structure(Chunk* chunk, sf::Vector3i center) {
    int x_offset = -dimX/2;
    int z_offset = -dimZ/2;

    sf::Vector3i offset(x_offset, 0, z_offset);

    for(int y = 0; y < dimY; y++) {
        for(int z = 0; z < dimZ; z++) {
            for(int x = 0; x < dimX; x++) {
                int index = y * (dimX*dimZ) + z * dimX + x;
                sf::Vector3i position(x, y, z);
                position += center + offset;

                if(layers[index] == 255) 
                    continue;

                chunk->setBlock(position.x, position.y, position.z, layers[index]);
            }
        }
    }
}

void Structure::print_info() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Id: " << this->id << std::endl;
    std::cout << "Dimensions: " << this->dimX
        << ", " << this->dimY << ", " << this->dimZ
        << std::endl;
    for(int y = 0; y < dimY; y++) {
        for(int z = 0; z < dimZ; z++) {
            for(int x = 0; x < dimX; x++) {
                int index = y * (dimX*dimZ) + z * (dimX) + x;
                std::cout << (int)layers[index] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}