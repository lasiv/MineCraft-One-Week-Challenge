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
            inFile >> this->dimensions[0] >> this->dimensions[1] >> this->dimensions[2];
        }
        else if (line == "Layers") {
            int dimX = dimensions[0];
            int dimY = dimensions[1];
            int dimZ = dimensions[2];

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
    std::copy(std::begin(other.dimensions), std::end(other.dimensions), this->dimensions);
}

void Structure::generate_structure()
{
}

void Structure::print_info() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Id: " << this->id << std::endl;
    std::cout << "Dimensions: " << this->dimensions[0]
        << ", " << this->dimensions[1] << ", " << this->dimensions[2]
        << std::endl;
    for(int y = 0; y < dimensions[1]; y++) {
        for(int z = 0; z < dimensions[2]; z++) {
            for(int x = 0; x < dimensions[0]; x++) {
                int index = y * (dimensions[0]*dimensions[2]) + z * (dimensions[0]) + x;
                std::cout << (int)layers[index] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}