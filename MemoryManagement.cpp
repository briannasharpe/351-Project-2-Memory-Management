#include <iostream>
#include <fstream>

using namespace std;

int main() {
	// input variables
	int memorySize, pageSize;
	
	string inputFile;
	string outputFile;
	
	// get input data
	cin.ignore();
	cout << "Memory size (KB): ";
	cin >> memorySize;
	cout << "Page Size (1:100, 2;200, 3:400): ";
	cin >> pageSize;
	cout << "Name of workload file: ";
	getline(cin, inputFile);
	cout << "Name for output file: ";
	getline(cin, outputFile);
	
	// page size
	switch (pageSize) {
		case 1:
			PageSize = 100;
			break;
		case 2:
			PageSize = 200;
			break;
		case 3:
			PageSize = 400;
			break;
		default:
			cout << "Invalid input";
	};
	
	// read workload file
	ifstream infile;
	infile.open(inputFile);
	if (infile.is_open()) {
		infile << ***in1 file stuff;
    
		// close file
		infile.close();
	} // if (infile.is_open())

} // int main()