#include <iostream>
#include <fstream>
#include <string>
#include <list>  
#include <cmath>

using namespace std; 

bool isAInstruction(const string& str) { 
	// Check if string is at least 2 characteres long 
	if(str.size() < 2 || str[0] != '@') return false; 

	//Extract the part of the string after `@`
	string decimalPart = str.substr(1); 

	// Check if the decimal part is empty or not
	if(decimalPart.empty()) return false; 

	// Ensure that the decimal part is valid
	char* end; 
	double value = strtod(decimalPart.c_str(), &end); 

	// Check if the entire substring was valid and the value is a finite
	// Also ensure there are no invalid characteres in the decimal part
	return *end == '\0' && !std::isinf(value) && !isnan(value); 
}

string decimalToBinary(int value, size_t biLenght){
	string binary; 

	// Handle special case for zero
	if (value == 0) return "0";

	// Convert the integer to binary 
	while (value > 0){
		binary.insert(0,1, (value % 2) ? '1' : '0'); 
		value /= 2;
	}

	return binary;
}



int main()
{

	string filePath = "../MaxL.asm";
	ifstream file(filePath); 
	string line; 
	list<int> binaryList; 

	if (!file.is_open()) { 
		cerr << "Error opening a file." << endl; 
		return 1;
	}

	// Removing empty lines and commentary lines
	while (getline(file, line)) {
		if (!line.empty() && !line.find("//") == 0) {
		 	// Parse instruction line 
			bool aInstruction = isAInstruction(line); 
			if(aInstruction){
				cout << line << endl; 
				//Gerenate the binary based on A instructions rules
				//return; 
			}

			// Gerenate the binary based on C instructions rules 
			//return;
			
		}
	}
 

	file.close(); 
	return 0; 

}