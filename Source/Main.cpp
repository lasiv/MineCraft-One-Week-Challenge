#include "Application.h"

#include <fstream>
#include <iostream>

#include "Config.h"

#ifdef __WIN32
extern "C" {
// Enable dedicated graphics
__declspec(dllexport) bool NvOptimusEnablement = true;
__declspec(dllexport) bool AmdPowerXpressRequestHighPerformance = true;
}
#endif // __WIN32

namespace {
    /**
     * @brief Loads configuration settings from a file into a Config object.
     *
     * @param config Reference to the Config object to populate.
     *
     * @details
     * Tries to open an existing configuration file (e.g., "config.ini").
     * - If the file exists: parses and loads settings such as resolution, fullscreen, and vsync.
     * - If the file does not exist: creates a new config file using default values from the Config class.
     * After parsing or creating the file, values are written into the provided Config object.
     * 
     * This ensures that the program always starts with valid configuration data,
     * even on the first launch or after deletion of the config file.
     */
    void loadConfig(Config &config);

    /**
     * @brief Displays startup information from a text file.
     *
     * @details
     * Reads and prints the contents of a text file (e.g., "Res/info.txt") to the console.
     * This can include version information, controls, or other relevant details.
     */
    void displayInfo();
} // namespace


/**
 * @brief Entry point of the program.
 * 
 * @details
 * - Loads configuration settings from a config file into a Config object.
 * - Displays startup information by reading a text file (e.g., version, controls).
 * - Instantiates the Application class, passing the loaded config.
 * - Starts the main game loop via Application::run(), which handles input, updates, and rendering.
 * 
 * @return int Exit code (0 on successful execution).
 */
int main()
{
    Config config;
    loadConfig(config);
    displayInfo();

    std::cin.ignore();
    std::cout << "Loading game...\n";

    Application app(config);
    app.runLoop();
}

namespace {
void loadConfig(Config &config)
{
    std::ifstream configFile("config.txt");
    std::string key;

    // If the config file is missing or "bad"
    if(!configFile.good())
    {
        std::cout << "Configuration file invalid,\n";
        std::cout << "writing 'new' configuration." << "\n";
        std::cout << "\n";

        std::ofstream outfile("config.txt");

        if(outfile.is_open())
        {
            outfile << "renderdistance " << "8";
            outfile << "fullscreen " << "0";
            outfile << "windowsize " << "1600 " << "900";
            outfile << "fov " << "105";

            outfile.close();
            configFile.close(); // Close so it can be reopened safely.
        }

        std::cout << "\n";
        std::cout << "New configuration file created." << "\n";
    }

    try
    {
        // Open 'new' config file.
        if(!configFile.is_open())
        {
            configFile.open("config.txt");
        }

        // If the file is still creating errors
        if(configFile.fail())
        {
            std::cout << "Error: The program failed to load the configuration files." << "\n";
            std::cout << "To understand why this error may have occured,\n";
            std::cout << "please examine your 'config.txt' file. Thank you." << "\n";

            // Because this is thrown before runtime, no memory needs to be freed.
            throw "Unable to load configuration file.";
        }

        if (configFile.is_open())
        {
            while (configFile >> key)
            {
                if (key == "renderdistance") {
                    configFile >> config.renderDistance;
                    std::cout << "Config: Render Distance: "
                            << config.renderDistance << '\n';
                }
                else if (key == "fullscreen") {
                    configFile >> config.isFullscreen;
                    std::cout << "Config: Full screen mode: " << std::boolalpha
                            << config.isFullscreen << '\n';
                }
                else if (key == "windowsize") {
                    configFile >> config.windowX >> config.windowY;
                    std::cout << "Config: Window Size: " << config.windowX << " x "
                            << config.windowY << '\n';
                }
                else if (key == "fov") {
                    configFile >> config.fov;
                    std::cout << "Config: Field of Vision: " << config.fov << '\n';
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
    }
}

void displayInfo()
{
    std::ifstream inFile;
    inFile.open("Res/info.txt");
    std::string line;
    while (std::getline(inFile, line)) {
        std::cout << line << "\n";
    }
}
} // namespace
