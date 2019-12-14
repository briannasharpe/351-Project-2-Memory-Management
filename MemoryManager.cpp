#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

struct Process {
	int ID; // process ID
	int pieces; // number of pieces of memory
	int addressSpace; // total size of address space
	int frames; // number of frames

	// time related variables	
	int arrivalTime, startTime, completionTime, turnaroundTime, waitTime;
};

void printTime(Process*, int, int);
void printQueue(vector<int>);
void printMap(Process, int, std::vector<int>, std::vector<int>&, int);


int main() {
	/* **************************************************** *
	 * ******************** INPUT DATA ******************** *
	 * **************************************************** */
	// input variables
	int memorySize;
	int pageSize;

	string inputFile;
	ifstream infile;

	// get input data
	cout << "Memory size (KB): ";
	//cin >> memorySize;
	memorySize = 2000;
	cout << "Page Size (1:100, 2;200, 3:400): ";
	//cin >> pageSize;
	pageSize = 1;
	cout << "Name of workload file: ";
	//cin.ignore();
	//getline(cin, inputFile);
	inputFile = "in1.txt";
	
	// page size
	switch (pageSize) {
		case 1:
			pageSize = 100;
			break;
		case 2:
			pageSize = 200;
			break;
		case 3:
			pageSize = 400;
			break;
		default:
			cout << "Invalid input";
	}; // switch (pageSize)

	/* **************************************************** *
	 * ******************** INPUT FILE ******************** *
	 * **************************************************** */
	// read workload file
	infile.open(inputFile);

	int processes = 0; // number of processes
	int memoryPieces = 0; // number of pieces in memory

	infile >> processes;
		
	Process* processArray;
	processArray = new Process[processes];

	for (int i = 0; i < processes; i++) {
		// set defaults
		processArray[i].startTime = -1;
		processArray[i].waitTime = 0;
		processArray[i].turnaroundTime = 0;
		processArray[i].frames = int(ceil(double(processArray[i].addressSpace)/pageSize)); // ceil to round up value
		
		// define process values
		infile >> processArray[i].ID;
		infile >> processArray[i].arrivalTime;
		infile >> processArray[i].completionTime;
		infile >> processArray[i].pieces;
		for (int j = 0; j < processArray[i].pieces; j++) {
			infile >> memoryPieces;
			processArray[i].addressSpace += memoryPieces;
		} // for (j < memory pieces)
	} // for (i < processes)
	
	infile.close(); // close file

	/* **************************************************** *
	 * ****************** MEMORY MANAGER ****************** *
	 * **************************************************** */
	int time = 0; // overall time
	int arraySize = memorySize / pageSize; // number of free frames	
	int freeFrames = arraySize; // number of free frames
	int currentProcess = 0; // current process for loops
	int completeProcesses = 0; // number of completed processes
	int count = 0; // counter

	// turnaround variables
	int totalTurnaround = 0;
	double averageTurnaround = 0.0;

	// arrays
	vector<Process> processQueue;
	vector<int> inputQueue; // process queue inputs
	vector<int> memoryArray(arraySize,0); // memory map
	vector<int> pageArray(arraySize,0); // pages in memory

	// while all processes are incomplete
	while (processes != completeProcesses) {
		printTime(processArray, processes, time); // print time

		// for the number of processes
		for (int i = 0; i < processes; i++) {
			if (time == processArray[i].arrivalTime) {
				cout << "Process " << processArray[i].ID << " arrives\n";
				processQueue.push_back(processArray[i]); // add to queue

				// add to output queue and print
				inputQueue.push_back(processArray[i].ID);
				printQueue(inputQueue);
			} // if process arrival time = overall time
		} // for (i < processes)

		// while process queue isn't empty
		while (processQueue.empty() == false) {
			//if process queue frame count is under free frame count
			if (processQueue[count].frames <= freeFrames) {
				currentProcess = processQueue[count].ID;
				freeFrames -= processQueue[count].frames;

				cout << "\tMM moves Process " << currentProcess << " to memory\n";
				
				// for the number of frames
				for (int i = 0; i < processQueue[count].frames; i++) {
					// for size of array
					for (int j = 0; j < arraySize; j++) {
						if (memoryArray[j] == 0) {
							memoryArray[j] = processQueue[count].ID;
							break;
						} // if
					} // for (j < arraySize)
				} // for (i < frames)
				
				// update start times
				processArray[currentProcess-1].startTime = time;
				processQueue[count].startTime = time;
				
				// remove processes from queues
				processQueue.erase(processQueue.begin() + count);
				inputQueue.erase(inputQueue.begin() + count);
				
				// print queue and map
				printQueue(inputQueue);
				printMap(processArray[currentProcess-1], arraySize, memoryArray, pageArray, pageSize);
			} else if (freeFrames < processQueue[count].frames) {
				count++;

				if (processQueue.begin() + processQueue.size() == processQueue.end()) {
					break;
				} // if at end
			} // if
		} // while queue isn't empty
		
		// for the number of processes
		for (int i = 0; i < processes; i++) {
			// if times match
			if (processArray[i].completionTime + processArray[i].startTime == time && processArray[i].startTime != -1) {
				// for size of array
				for (int j = 0; j < arraySize; j++) {
					// if memory array element matches process
					if (memoryArray[j] == processArray[i].ID) {
						memoryArray[j] = 0;
						freeFrames++;
					} // if
				} // for (j < array size)
				
				// print status and map
				cout << "Process " << processArray[i].ID << " completes\n";
				printMap(processArray[i], arraySize, memoryArray, pageArray, pageSize);
				
				// turnaround time
				processArray[i].turnaroundTime = processArray[i].completionTime + (processArray[i].startTime - processArray[i].arrivalTime);
				
				completeProcesses++;
			} // if
		} // for (i < processes)
		time++;
	} // while (processes != completeProcesses)

	// turnaround time calculation	
	for (int i = 0; i < processes; i++) {
		totalTurnaround += processArray[i].turnaroundTime; // add all turnaround times
	} // for
	
	averageTurnaround = double(totalTurnaround) / processes;
	
	// print turnaround times
	cout << "Total Turn Around Time: " << totalTurnaround << endl;
	cout << "Average Turn Around Time: " << averageTurnaround << endl;

	delete[] processArray; // delete the array

	return 0;
} // int main()

/* ******************** printTime ********************* *
 * print time when processes arrive and complete
 * ********************* "t = #" ********************** */
void printTime(Process* array, int processes, int time) {
	// for all the processes
	for (int i = 0; i < processes; i++) {
		if ((array[i].startTime != -1 && array[i].completionTime + array[i].startTime == time) || array[i].arrivalTime == time) {
			cout << "t = " << time << ": ";
			break;
		} // if at overall time
	} // for
} // void printTime

/* ******************** printQueue ******************** *
 * print process queue
 * **************** "Input Queue: [#]" **************** */
void printQueue(vector<int> queue) {
	cout << "\tInput Queue: [";

	if (queue.empty() == false) { // queue isn't empty
		// for processes in a queue
		for (int i = 0; i < queue.size(); i++) {
			if (i == queue.size()-1) { // add a closing bracket
				cout << queue[i] << "]\n";
			} else { // or add a space
				cout << queue[i] << " ";
			} // if at end of queue
		} // for (i < size of queue)
	} else { // queue is empty
		cout << "]\n";
	} // if queue is empty or not
} // void printQueue

/* ********************* printMap ********************* *
 * print memory map with frames, processes, and pages
 * printMap()
 * **************************************************** */
void printMap(Process array, int size, std::vector<int> memory, std::vector<int>& page, int pageSize) {
	int count = 0; // page counter
	
	cout << "\tMemory Map: \n";
	
	// for elements in the array
	for (int i = 0; i < size; i++) {
		if (memory[i] != 0) { // process, page
			if (array.ID == memory[i]) { 
				count++;
				page[i] = count;
			} // if
			cout << "\t\t" << (pageSize * i) << "-" << (pageSize * (i+1)-1) << ": Process " << memory[i] << ", Page " << page[i] << endl;
		} else { // free frame
			cout << "\t\t" << (pageSize * i) << "-" << (pageSize * (i+1)-1) << ": " << "Free Frame\n"; 
		} // if free frame or not
	} // for (i < size)
} // void printMap