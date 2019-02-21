#include <iostream>
#include <string>


using namespace std;


struct process {
	string name; //process name **ONLY FOR IMPLEMENTATION**
	int qlevel = 1; 
	int processArray[20]; // array of processes
	int arraylength; // passing length 
	int i = 0; // indicates array element of processArray **ONLY FOR IMPLEMENTATION**
	int arrivalTime = 0; //indicates arrival time **ONLY FOR IMPLEMENTATION**
	int state = 1; //1-waiting 2-exe 3- IO 4- completed
	process *next;
	int rtime = -1; // calculating rtime
	int wtime = 0; //wtime
	int ttime; //turnaround time
};

class processQ {
	public:
		int time;
		int idletime; // used for CPU utilization. Idle time.
		int numOfProcesses = 0;
		int tq1; // time quantum tq1
		int tq2; // time quantum tq2
		process *head, *tail;
		processQ() {// constructor sets all the values
			idletime = 0;
			time = 0;
			numOfProcesses = 0;
			tq1 = 6;
			tq2 = 12;
			head = NULL;
			tail = NULL;
		}
		void getProcess(string name, int length, int arr[]); //function to copy array elements into struct and creating a linked list
		void Enqueue(process *temp); //Function to simulate execution of process
		struct process* nextProcess(); // Function to select next process for execution
		bool isCompleted(); // function to check if all processes are completed
		void increment(); //increment all processes in I/O or waiting
		void display(const process *temp); //Function to display information after context switch
		void displayTotals(); //Function to display totals of each process and as a whole
		struct process* isPreempt(process *temp); // function that checks if another process is ready to preempt current process.

};

void processQ::getProcess(string name, int length, int arr[]) {
	process *temp = new process;
	temp->name = name;
	temp->arraylength = length; //function used to obtain name, length and array of CPU and I/O times
	for (int i = 0; i < length; i++) {
		temp->processArray[i] = arr[i]; //copy (length) number of elements into struct
	}
	if (head == NULL) { //linked list of obtained objects
		head = temp;	//Singly-linked list implementation
		tail = temp;
	}
	else {
		tail->next = temp;
		tail = temp;
	}
	numOfProcesses++; // number of objects (to check if all are done later)
}

void processQ::Enqueue(process *temp) {
	int counta = 0;

	if (temp == NULL) { // if function returns empty temp, no processes - CPU is Idling
		cout << "\n\nCurrent Time: " << time << endl;
		cout << "...................\n";
		cout << "Idle" << endl;
		cout << "...................\n\n";
		while (nextProcess() == NULL) { //increment all the processes in I/O and increment time until process.
			time++;//increment time
			idletime++; //increment idletime
			increment(); //increment I/O processes
		}
	}


	else {

		temp->state = 2; //change state of selected process
		display(temp); //display information
		if (temp->qlevel == 1) {
			counta = tq1; // change quantum to 6 for level1
		}
		else if (temp->qlevel == 2) { // change quantum to 12 for level2
			counta = tq2;
		}
		else { //else made it -1 so it does not interfere
			counta = -1;
		}

		while (temp->processArray[temp->i] > 0 && counta != 0 &&temp->state == 2 ) {//while the burst is more than 0, subtract 1 from current burst		
			if (temp->rtime == -1) {		//increment all processes in I/O and Ready Queue
				temp->rtime = time; //if rtime is -1 (initialized value) change it to current value(only happens first time)
			}
			temp->processArray[temp->i]--; //decrement current burst
			time++; //increment time
			increment();// increment all I/O and ready objects
			counta--; // subtract 1 quantum 
			
			if (temp->processArray[temp->i] == 0 && temp->state == 2) { // when burst reaches 0, i is incremented(moving to the next element)
				temp->i++;
				if (temp->i == temp->arraylength) { //if i is last element, change state to 4 (completed)
					temp->ttime = time;
					temp->state = 4;
				}
				else {
					temp->state = 3; //else change to I/O
				}

			}
			else if (isPreempt(temp) != NULL&&temp->state == 2) { // if isPreempt does not return object change state to waiting
				temp->state = 1;								//set arrivalTime to current Time and enqueue next process
				temp->arrivalTime = time;			
				Enqueue(nextProcess());
			}
			else if (counta == 0 && temp->state==2){ //if tq reaches 0 and there are CPU bursts left
				temp->qlevel = temp->qlevel++;		// add one to qlevel
				temp->state = 1;					//change state to waiting
				temp->arrivalTime = time;			//set arrivalTime to current Time
			}
		}
	}

}
bool processQ::isCompleted()
{
	
	int a = 0;
	process *temp = new process;
	for (temp = head; temp != tail->next; temp = temp->next) {//go through each element in Linked List and count how many are completed
		if (temp->state == 4)
			a++;
	}
	if (a == numOfProcesses) {
		return true; //compare completed processes to total number of processes- if equal return true
	}
	else {
		return false;
	}
}

struct process* processQ::nextProcess(){

	process *temp = new process;
	process *temp2 = new process;
	int min = INT_MAX;
	temp2 = NULL;
	temp = head;
	while (temp != tail->next) {
		if (min > temp->arrivalTime && temp->state == 1 && temp->qlevel == 1) {
			min = temp->arrivalTime; //first select qlevel 1 object with lowest arrivalTime
			temp2 = temp;
		}
		temp = temp->next;
	}
	
	temp = head;
	if (temp2 == NULL) {// if no qlevel1
		while (temp != tail->next) {//select qlevel 2 object with lowest arrivalTime
			if (min > temp->arrivalTime && temp->state == 1 && temp->qlevel == 2) {
				min = temp->arrivalTime;
				temp2 = temp;
			}
			temp = temp->next;
		}
	}
	temp = head;
	min = INT_MAX;
	if (temp2 == NULL) {
		while (temp != tail->next ) { //if no qlevel1 and qlevel2 processes 
			if (min > temp->arrivalTime && temp->state == 1) { //select process with lowest arrivalTime
				min = temp->arrivalTime;
				temp2 = temp;
			}
			temp = temp->next;
		}
	}
	return temp2;

}

void processQ::increment()
{
	//Function for incrementing Waiting (state = 1) and I/O (state = 3) objects
	process *temp = new process;
	temp = head;
	while (temp != tail->next) {// go through linked list
		if (temp->state == 1) { 
			temp->wtime++; // increment wait time for waiting objects
		}
		else if (temp->state == 3) {
			temp->processArray[temp->i]--; // decrement I/O time for I/O object
			if (temp->processArray[temp->i] == 0) { // if I/O reaches 0 change state to waiting and move to the next element
				temp->state = 1;					
				temp->i++;
				temp->arrivalTime = time; //set arrival time to current time
			}
		}
		temp = temp->next;// next object in Linked List
	}
}




void processQ::display(const process * temp)
{
	int b = 0;
	process *temp2 = new process;
	cout << "Current Time: " << time << endl << endl;
	cout << ".................................................." << endl;
	cout << "Now Running:  " << temp->name << endl;
	cout << ".................................................." << endl << endl;
	cout << "Ready Queue:    Process     Burst" << endl;
	temp2 = head;
	while (temp2 != tail->next) {
		if (temp2->state == 1 && temp2->processArray[temp2->i] != 0) { //Printing Ready object info
			cout << "                " << temp2->name << "          " << temp2->processArray[temp2->i] << endl;
			b++;
		}
		temp2 = temp2->next;

	}
	if (b == 0) {
		cout << "                 [empty]" << endl;
	}
	cout << endl;
	cout << "Now in I/O:     Process      Remaining Time" << endl;
	temp2 = head;
	b = 0;
	while (temp2 != tail->next) {
		if (temp2->state == 3 && temp2->processArray[temp2->i] != 0) { // Printing I/O object info
			cout << "                " << temp2->name << "           " << temp2->processArray[temp2->i] << endl;
			b++;
		}
		temp2 = temp2->next;
	}
	if (b == 0) {
		cout << "                [empty]" << endl;
	}
	cout << endl;

	cout << "Completed:   ";
	temp2 = head;
	while (temp2 != tail->next) {//Printing completed objects
		if (temp2->state == 4) {
			cout << temp2->name << ", ";
			b++;
		}
		temp2 = temp2->next;

	}
	cout << endl;

	if (b == 0) {
		cout << "      [empty]" << endl;
	}
	cout << endl;
}

void processQ::displayTotals()
{
	double cpU;
	double averageWait;
	double totalWait = 0;
	double totalTurn = 0;
	double totalResponse = 0;
	process *temp = new process;
	temp = head;
	cpU = ((double)time - (double)idletime) / (double)time * 100;
	cout << "Total time:           " << time << endl;
	cout << "CPU Utilization:       " << cpU << "%" << endl << endl;
	cout << "Waiting Times:          Process      Time" << endl;
	while (temp != tail->next) { //calculating and printing wait times for each process
		cout << "                        " << temp->name << "           " << temp->wtime << endl;
		totalWait = totalWait + temp->wtime;
		temp = temp->next;
	}
	cout << endl << "Average Wait Time: " << totalWait / (double)numOfProcesses << endl << endl; //average Wait
	cout << "Turnaroud Times:         Process     Time" << endl;
	temp = head;
	while (temp != tail->next) { // calculating and printing turnaround times for each process
		cout << "                        " << temp->name << "           " << temp->ttime << endl;
		totalTurn = totalTurn + temp->ttime;
		temp = temp->next;
	}
	temp = head;
	cout << endl << "Average Turnaround Time: " << totalTurn / (double)numOfProcesses << endl << endl;// average Turnaround
	cout << "Response Times:       Process        Time" << endl;
	while (temp != tail->next) {//calculating and printing response time
		cout << "                        " << temp->name << "           " << temp->rtime << endl;
		totalResponse = totalResponse + temp->rtime;
		temp = temp->next;
	}
	cout << endl << "Average Response Time: " << totalResponse / (double)numOfProcesses << endl << endl;// average response
}

struct process* processQ::isPreempt(process * temp)
{
	process *temp2 = new process;
	temp2 = head;
	while (temp2 != tail->next) { 
		if (temp2->qlevel < temp->qlevel && temp2->state == 1) { //Check if there is a process with a lower q level and waiting
			return temp2;	//return that process							
		}
		temp2 = temp2->next;
	}
	return NULL; //if none return null
}



int main() {

	//I cheated by counting elements of each array to make it easier
	int P1[20] = { 4, 15, 5, 31, 6, 26, 7, 24, 6, 41, 4, 51, 5, 16, 4 };//15
	int P2[20] = { 9, 28, 11, 22, 15, 21, 12, 28, 8, 34, 11, 34, 9, 29, 10, 31, 7 }; //17
	int P3[20] = { 24, 28, 12, 21, 6, 27, 17, 21, 11, 54, 22, 31, 18 };//13
	int	P4[20] = { 15, 35, 14, 41, 16, 45, 18, 51, 14, 61, 13, 54, 16, 61, 15 };//15
	int	P5[20] = { 6, 22, 5, 21, 15, 31, 4, 26, 7, 31, 4, 18, 6, 21, 10, 33, 3 };//17
	int	P6[20] = { 22, 38, 27, 41, 25, 29, 11, 26, 19, 32, 18, 22, 6, 26, 6 };//15
	int	P7[20] = { 4, 36, 7, 31, 6, 32, 5, 41, 4, 42, 7, 39, 6, 33, 5, 34, 6, 21, 9 };//19
	int	P8[20] = { 5, 14, 4, 33, 6, 31, 4, 31, 6, 27, 5, 21, 4, 19, 6, 11, 6 };//17

	processQ var;
	//passing process name, length and array into 
	var.getProcess("P1", 15, P1);
	var.getProcess("P2", 17, P2);
	var.getProcess("P3", 13, P3);
	var.getProcess("P4", 15, P4);
	var.getProcess("P5", 17, P5);
	var.getProcess("P6", 15, P6);
	var.getProcess("P7", 19, P7);
	var.getProcess("P8", 17, P8);
	var.Enqueue(var.head);
	while (var.isCompleted() == false) {
		var.Enqueue(var.nextProcess());

	}
	cout<<endl<<endl << "--------------------------" << endl << "Done!!!"<<endl << "--------------------------" << endl<<endl;
	var.displayTotals();
	//var.display();
	system("pause");



	return 0;
}