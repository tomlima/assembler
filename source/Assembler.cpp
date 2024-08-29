#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <bitset>

using namespace std;

// Define constants
const int RAM_START_ADDRESS = 16; // Starting address for variables

/** 
 * Helper function to remove comments and trim whitespace 
 * @param line - The instruction line 
 * @returns The processed given instruction line 
*/
string preprocessLine(const string& line) {
    string processed = line;
    size_t commentPos = processed.find("//");
    if (commentPos != string::npos) {
        processed = processed.substr(0, commentPos); // Remove comment
    }
    // Trim whitespace
    processed.erase(processed.find_last_not_of(" \n\r\t") + 1);
    processed.erase(0, processed.find_first_not_of(" \n\r\t"));
    return processed;
}

/** 
 * Function to parse labels and instructions
 * @param  fileName The asm file name to be assembled
 * @param labels - The map of labels
 * @param instructions - The instructions vector
 * @return void
*/
void parseFile(const string& fileName,unordered_map<string, int>& labels,vector<string>& instructions) {
    ifstream file(fileName);
    string line;
    int lineNumber = 0;

    // First pass: collect labels
    while (getline(file, line)) {
        string processed = preprocessLine(line);
        if (processed.empty()) continue;

        if (processed[0] == '(') { // Label
            size_t endPos = processed.find(')');
            if (endPos != string::npos) {
                string label = processed.substr(1, endPos - 1);
                labels[label] = lineNumber;
            }
        } else {
            ++lineNumber; // Increment line number for instructions
        }
    }

    file.clear();
    file.seekg(0, ios::beg); // Reset file read pointer

    // Second pass: convert instructions
    while (getline(file, line)) {
        string processed = preprocessLine(line);
        if (processed.empty()) continue;

        if (processed[0] == '(') {
            continue; // Skip label lines
        }

        // Add instruction
        instructions.push_back(processed);
    }
}

/** 
 * Convert decimal value to binary string 
 * @param value An given decimal value
 * @param bits The number of bits of the binary value
 * @returns An n bits binary string 
*/
string toBinary(int value, int bits) {
    return bitset<16>(value).to_string().substr(16 - bits);
}


// Maps for C-instruction fields
unordered_map<string, string> compMap = {
    {"0", "0101010"}, {"1", "0111111"}, {"-1", "0111010"}, {"D", "0001100"},
    {"A", "0110000"}, {"M", "1110000"}, {"!D", "0001101"}, {"!A", "0110001"},
    {"!M", "1110001"}, {"-D", "0001111"}, {"-A", "0110011"}, {"-M", "1110011"},
    {"D+1", "0011111"}, {"A+1", "0110111"}, {"M+1", "1110111"}, {"D-1", "0001110"},
    {"A-1", "0110010"}, {"M-1", "1110010"}, {"D+A", "0000010"}, {"D+M", "1000010"},
    {"D-A", "0010011"}, {"D-M", "1010011"}, {"A-D", "0000111"}, {"M-D", "1000111"},
    {"D&A", "0000000"}, {"D&M", "1000000"}, {"D|A", "0010101"}, {"D|M", "1010101"}
};

unordered_map<string, string> destMap = {
    {"", "000"}, {"M", "001"}, {"D", "010"}, {"MD", "011"},
    {"A", "100"}, {"AM", "101"}, {"AD", "110"}, {"AMD", "111"}
};

unordered_map<string, string> jmpMap = {
    {"", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
    {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
};

/** 
 * Convert an instruction to binary machine code 
 * @param instruction - An given instruction 
 * @param labels - The labels map
 * @param variableAddresss - The variabel address number
 * @returns The binary code of an given instruction
*/
string convertInstruction(const string& instruction,unordered_map<string, int>& labels,int& variableAddress) {
    string binaryCode;

    if (instruction[0] == '@') { // A-instruction
        string value = instruction.substr(1);
        int address;

        // Handle label addresses
        if (isdigit(value[0])) {
            address = stoi(value);
        } else {
            if (labels.find(value) != labels.end()) {
                address = labels.at(value);
            } else {
                // Allocate new address for variable
                address = variableAddress++;
                labels[value] = address;
            }
        }

        binaryCode = "0" + toBinary(address, 15); // 15-bit address
    } else { // C-instruction
        // Split the instruction into dest=comp;jump parts
        size_t eqPos = instruction.find('=');
        size_t semiPos = instruction.find(';');
        string dest = (eqPos != string::npos) ? instruction.substr(0, eqPos) : "";
        string comp = (semiPos != string::npos) ? instruction.substr(eqPos + 1, semiPos - eqPos - 1) : instruction.substr(eqPos + 1);
        string jump = (semiPos != string::npos) ? instruction.substr(semiPos + 1) : "";

        string compBinary = compMap.count(comp) ? compMap[comp] : "0000000";
        string destBinary = destMap.count(dest) ? destMap[dest] : "000";
        string jumpBinary = jmpMap.count(jump) ? jmpMap[jump] : "000";

        binaryCode = "111" + compBinary + destBinary + jumpBinary;
    }

    return binaryCode;
}

int main() {
    unordered_map<string, int> labels;
    vector<string> instructions;
    int variableAddress = RAM_START_ADDRESS; // Initialize the variable address counter

    // Parse the input assembly file
    parseFile("example.asm", labels, instructions);

    // Open the output file for writing
    ofstream outFile("example.bin");

    if (!outFile) {
        cerr << "Error: File could not be opened for writing: output.hack" << endl;
        return 1; // Return with an error code
    }

    // Write the binary code to the file
    for (const string& instruction : instructions) {
        string binaryCode = convertInstruction(instruction, labels, variableAddress);
        outFile << binaryCode << endl; // Write each binary code on a new line
    }

    // Close the output file
    outFile.close();

    cout << "Binary code has been written to output.hack" << endl;

    return 0;
}
