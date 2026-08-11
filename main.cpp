#include "Pipeline.h"
#include<iostream>
#include<string>
#include<stdexcept>
using namespace std;
void printUsage(const char* prog_name) {
    cerr << "Usage: " << prog_name << " <-e|-d> -i <input_file> -o <output_file> [-p <password>]\n"
    <<"  -e    Encrypt\n"<<"  -d    Decrypt\n"<<"  -i    Input file path\n"<<"  -o    Output file path\n"<<"  -p    Password (optional, will prompt if omitted)\n";
}

int main(int argc, char** argv) {
    bool is_encrypt = true;
    bool mode_set = false;
    
    string input_path;
    string output_path;
    string password;
    
    for (int i=1;i<argc;i++) {
        string arg = argv[i]; 
        if (arg == "-e") {
            is_encrypt = true;
            mode_set = true;
        } 
        else if (arg == "-d") {
            is_encrypt = false;
            mode_set = true;
        } 
        else if (arg == "-i" && i + 1 < argc) {
            input_path = argv[++i];
        } 
        else if (arg == "-o" && i + 1 < argc) {
            output_path = argv[++i];
        } 
        else if (arg == "-p" && i + 1 < argc) {
            password = argv[++i];
        } 
        else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } 
        else {
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    } 
    if (!mode_set || input_path.empty() || output_path.empty()) {
        cerr << "Error: Missing required arguments.\n";
        printUsage(argv[0]);
        return 1;
    }
    
    if (password.empty()) {
        cout << "Enter password: ";
        getline(std::cin, password);
    }
    
    if (password.empty()) {
        cerr << "Error: Password cannot be empty.\n";
        return 1;
    }
 
    try {
        Pipeline pipeline(is_encrypt, input_path, output_path, password);
        pipeline.run();
    } catch (const std::exception& e) {
        std::cerr << "\n[Fatal Error] " << e.what() << "\n";
        return 1;
    }

    return 0;
}