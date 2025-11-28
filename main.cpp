#include<bits/stdc++.h> 
#include "huffman.h"
using namespace std ; 


int main(int argc , char* argv[]) {
    vector<string> args;
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.size() < 4) {
        cout << "Usage:" << endl;
        cout << "  Encode: huffman.exe [-v] -e input_file code_file output_file" << endl;
        cout << "  Decode: huffman.exe [-v] -d input_file code_file output_file" << endl;
        return 1;
    }

    bool verbose = false;
    string mode = "";
    string inputFile , codeFile , outputFile;

    size_t argIndex = 1;
    
    if (args[argIndex] == "-v") {
        verbose = true;
        argIndex++;
    }

    if (argIndex >= args.size()) return 1;
    mode = args[argIndex++];
    
    if (argIndex >= args.size()) { cerr << "Missing input file." << endl; return 1; }
    inputFile = args[argIndex++];

    if (argIndex >= args.size()) { cerr << "Missing code file." << endl; return 1; }
    codeFile = args[argIndex++];

    if (argIndex >= args.size()) { cerr << "Missing output file." << endl; return 1; }
    outputFile = args[argIndex++];

    HuffManTree huffman ;

    if (mode == "-e") {
        huffman.encode(inputFile , codeFile , outputFile , verbose);
    } else if (mode == "-d") {
        huffman.decode(inputFile , codeFile , outputFile , verbose);
    } else {
        cerr << "Unknown mode: " << mode << endl;
        return 1;
    }

    return 0;
}