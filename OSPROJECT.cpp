#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

void showManagerMenu();
void readCSV(const string& filename, vector<string>& userList);
void printCSV(vector<string> userList);
int verifyCN(const string cardNumber, vector<string> userList);
void* verifyCNrunner(void* args);

int verifyOperation(const string operation, vector<string> transactionList);
void* verifyOperationRunner(void* arg);

void balanceInquiry(int index, const vector<string>& userList);
void clearScreen();
void writeCSV(const string& filename, const vector<string>& data);
bool verifyAmount(float amount,int index,const vector<string> userList);
bool verifyDeposit(float amount);
void sendMoney(float amount,int index, vector<string>& userList);
void AddMoney(float amount,int index, vector<string>& userList);
void receipt(int index, int mode, const vector<string> userList);
void writeTransaction(const string filename,const string cardNum,const string operation,float amount);
void displayLogs(vector<string> transactionlist);
void displayTotalDeposited(vector<string> transactionlist);
void displayTotalWithdrawn(vector<string> transactionlist);
void printLogOperation(const string operation, vector<string> transactionList);
void* OperationRunner(void* arg);
void search(vector<string> transactionList);
void* searchRunner(void* arg);

struct ThreadArgs {
	vector<string>* userList;
	int startIdx;
	int length;
	string targetCN;
};

struct TransactionThreadArgs {
	vector<string>* transactionList;
	int startIdx;
	int length;
	string targetOperation;
};

// Struct to hold thread arguments
struct AmountThreadArgs {
	vector<string>* transactionList;
	int startIdx;
	int length;
};

volatile bool found = false; // Shared memory variable to indicate if target is found
double totalDepositAmount = 0.0; //shared variables used my multiple thread
double totalWithdrawalAmount = 0.0;

// Semaphore for synchronization
sem_t depositSemaphore;
sem_t withdrawalSemaphore;



int main() {
	int factor;
	do{
	clearScreen();
	cout<<"E-wallet\n\n1- User Mode\n2- Manager Mode\n3- Quit\nselect: ";
	cin>>factor;
	if(factor == 1){
		break;
	}
	else if(factor == 2){
		showManagerMenu();
		return 0;	
	}
	else if(factor == 3){
		return 0;
	}
	else{
	cout<<"\nInvalid Input";
	cin.ignore();
	cin.get();
	
	}
	}
	while(true);
	
	vector<string> userList;
	readCSV("Users.csv", userList); // fill the userList

	// Print the contents of the list (for verification)
	// printCSV(userList); return 0;

	string cardNumber;

	// Loop to prompt user for card number input
	// Log in Screen
	
	int index;

Login:

	do {
		found = false; // Reset flag for each search
		clearScreen();
		cout << "Enter card number (or 'quit' to exit): ";
		cin >> cardNumber;

		if (cardNumber == "quit") {
			cout << "Exiting program." << endl;
			return 0;
		}
		/*
		if(cardNumber == "manager") {
			showManagerMenu();
			return 0;
		}*/

		index = verifyCN(cardNumber, userList);
		if (index >= 0) {
			cout << "Card number verified. User found at index " << index << "." << endl;
			break;
		} else {
			cout << "Card number not found. Please try again." << endl;

			cin.ignore();
			cin.get(); //safe replacement for system("pause");

		}
	} while (true);

	writeTransaction("transaction.csv", cardNumber, "Login", 0);

	//When verified the loop breaks to the menu below: result-> index is now aqcuired
	do {
		clearScreen();
		// Display menu
		cout << "E-Wallet Menu\n";
		cout << "1. Send Money\n";
		cout << "2. Deposit Money\n";
		cout << "3. Balance Inquiry\n";
		cout << "4. Log Out\n";
		cout << "Choose an option (1-4): ";

		// Read user choice
		int choice;
		cin >> choice;

		// Switch structure for menu actions
		switch (choice) {
			case 1:
				float amount;
				clearScreen();
				cout << "Enter the amount you want to send: ";
				cin >> amount;

				if (verifyAmount(amount,index,userList)) {
					cout<<"\nAmount verified!";
					sendMoney(amount,index, userList);
					writeCSV("Users.csv", userList);
					cout<<endl<<amount<<" Sent";


					writeTransaction("transaction.csv", cardNumber, "Withdraw", amount);
					receipt(index, 0, userList);
					cin.ignore();
					cin.get();
				} else {

					cout << "\nInsufficient Balance. Check your Balance via Balance Inquiry.";
					cout << "\nIncrease your Balance using Deposit Money.";
					cin.ignore();
					cin.get();

				}

				break;

			case 2:
				float Deposit;
				clearScreen();
				cout << "Enter the amount you want to Deposit: ";
				cin >> Deposit;

				if (verifyDeposit(Deposit)) {

					cout<<"\nAmount verified!";
					AddMoney(Deposit,index, userList);
					writeCSV("Users.csv", userList);
					cout<<endl<<Deposit<<" received";
					writeTransaction("transaction.csv", cardNumber, "Deposit", Deposit);
					cin.ignore();
					cin.get();
				} else {

					cout << "\nDeposit Limit Exceeded.";
					cout << "\nRs. 20,000 at a time";
					cin.ignore();
					cin.get();

				}
				break;
			case 3:

				balanceInquiry(index,userList);
				break;

			case 4:
				clearScreen();
				cout << "Logged Out... Press any key to resume" << endl;
				cin.ignore();
				cin.get();
				goto Login; // Exit the loop and go to card input again

			default:
				clearScreen();
				cout << "Invalid choice. Please select a number between 1 and 4." << endl;
				cout << "Press any key to resume" << endl;
				cin.ignore();
				cin.get();
		}

	} while (true);


	return 0;
}

bool verifyAmount(float amount,int index,const vector<string> userList) {
//cout<<"float"<<amount<<"index+2"<<userList[index+2]<<endl;
//cout<<stof(userList[index+2])<<endl;
	if(amount > stof(userList[index+2]))
		return false;
	return true;
}



bool verifyDeposit(float amount) {
//cout<<"float"<<amount<<"index+2"<<userList[index+2]<<endl;
//cout<<stof(userList[index+2])<<endl;
	if(amount > 20000)
		return false;
	return true;
}

void sendMoney(float amount,int index, vector<string>& userList) {
	float old = stof(userList[index+2]);
	old -= amount;
	userList[index+2] = to_string(old);
}

void AddMoney(float amount,int index, vector<string>& userList) {
	float old = stof(userList[index+2]);
	old += amount;
	userList[index+2] = to_string(old);
}

void clearScreen() {
	cout << "\033[2J\033[H"; // safe replacement for system("clear");
}

void readCSV(const string& filename, vector<string>& userList) {
	/*
	This function runs at program start.
	It reads the CSV and creates a list of the users.
	*/
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error opening file: " << filename << endl;
		exit(0);
	}

	string line;
	while (getline(file, line)) {
		// push each string separated by , into userList
		stringstream ss(line);
		string token;
		while (getline(ss, token, ',')) {
			userList.push_back(token);
		}
	}

	file.close();
}

void receipt(int index, int mode, const vector<string> userList) {

	cout<<"\n--- Receipt ---"<<endl;
	if(mode ==0) {
		cout<<"After Deducted: ";
	} else {
		cout<<"After Deposited: ";
	}
	cout<<userList[index+2];
	cout<<"\nCard Number: "<<userList[index]<<"\nFull name: "<<userList[index+1]<<endl;
}

void writeCSV(const string& filename, const vector<string>& data) {
	// Open the file in write mode
	ofstream file(filename);

	if (!file.is_open()) {
		cerr << "Error opening file: " << filename << endl;
		exit(1);
	}
	int liner = 0;
	// Write the data to the file, separating each element with a comma
	for (size_t i = 0; i < data.size(); i++) {
		liner++;
		file << data[i];


		if (i < data.size() - 1 && liner != 3) {
			file << ","; // Separate elements with commas
		}
		if(liner==3) {
			file<<endl;
			liner = 0;
		}


	}

	file.close(); // Close the file
}


void printCSV(vector<string> userList) {
	/*
	    This function prints the Vector which
	    represents the CSV file
	*/
	int nl=3;
	for (const auto& user : userList) {
		if(nl==0 || nl==3) {
			nl = 3;
			cout << endl << user;
			nl--;
		} else {
			cout <<", " << user;
			nl--;
		}
	}
}

int verifyCN(const string cardNumber, vector<string> userList) {
	/*
	    This function creates n number of threads
	    and implements fair load for linear search
	    among multiple threads [data parallelism].
	    used in other functions also
	*/
	int numUsers = userList.size();
	int numThreads = 4;
	// Calculate the smallest multiple of numThreads that is greater than or equal to numUsers
	int chunkSize = (numUsers / numThreads) + ((numUsers % numThreads != 0) ? 1 : 0);

	pthread_t threads[numThreads];

	// Variable to hold result index
	int resultIndex = -1;

	// Create threads
	for (int i = 0; i < numThreads; ++i) {
		int startIdx = i * chunkSize;
		int length = min(chunkSize, numUsers - startIdx); // Ensure length doesn't exceed the number of users

		ThreadArgs* args = new ThreadArgs;
		args->userList = &userList;
		args->startIdx = startIdx;
		args->length = length;
		args->targetCN = cardNumber;

		pthread_create(&threads[i], nullptr, verifyCNrunner, args);
		cout<<"D> thread created: "<< threads[i]<<endl;
	}


	// Join threads
	int* returnedIndex;
	for (int i = 0; i < numThreads; ++i) {
		pthread_join(threads[i], reinterpret_cast<void**>(&returnedIndex));
		cout<<"D> Thread "<<threads[i]<<" joined"<<endl;
		if (*returnedIndex != -1) {
			resultIndex = *returnedIndex;
		}
	}
	cin.ignore();
	cin.get();
	// Return result index
	return resultIndex;
}

// Define verifyCNrunner function
void* verifyCNrunner(void* arg) {
	ThreadArgs* args = reinterpret_cast<ThreadArgs*>(arg);
	vector<string>& userList = *(args->userList);
	int startIdx = args->startIdx;
	int length = args->length;
	string targetCN = args->targetCN;
	int* resultIndex = new int(-1);
	pthread_t tid = pthread_self(); // get current thread id
	cout<<"D> Thread "<<tid<<" searching ("<<startIdx<<" - "<<startIdx+length<<")"<<endl;

	for (int i = startIdx; i < startIdx + length; i++) {
		if(found) {
			cout<<"D> Found! while "<<tid<<" was searching ["<<i<<"], Exiting..."<<endl;
			pthread_exit(resultIndex);
		}
		if (userList[i] == targetCN) {
			cout<<"D> Found at ["<<i<<"] by Thread "<<tid<<" Exiting..."<<endl;

			*resultIndex = i;
			found = true; //set flag true if found
			pthread_exit(resultIndex);

		}
	}
	cout<<"D> Thread "<<tid<<" Finished Search Exiting..."<<endl;
	pthread_exit(resultIndex);
}

//Shaheer Edit


void balanceInquiry(int index, const vector<string>& userList) {
// Prints the current users details
	clearScreen();
	cout << "\tBalance inquiry\n\nCard Number: " << userList[index] << endl;

	cout << "Full Name: " <<  userList[index+1] << endl;

	cout << "Current Balance: $" <<  userList[index+2] << endl;

	cin.ignore();
	cin.get();
}

void writeTransaction(const string filename,const string cardNum,const string operation,float amount) {
	ofstream file(filename, ios::app); // Open the file in append mode
	if (!file.is_open()) {
		cerr << "Error opening file: " << filename << endl;
		return;
	}

	file << cardNum;
	file << ",";
	file << operation;
	file << ",";
	file << amount;
	file.close();
}

void showManagerMenu() {

	vector<string> transactionlist;
	readCSV("transaction.csv", transactionlist);
	//printCSV(transactionlist);
//	cin.ignore();
//	cin.get();
	int choice;
	do {
		clearScreen();
		printf("Manager Mode:\n");
		printf("1. Look up for logs\n");
		printf("2. Display total deposited amount\n");
		printf("3. Display total withdrawn amount\n");
		printf("4. Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch(choice) {
			case 1:
				clearScreen();
				displayLogs(transactionlist);
				break;
			case 2:
				//clearScreen();
				displayTotalDeposited(transactionlist);
				break;
			case 3:
				//clearScreen();
				displayTotalWithdrawn(transactionlist);
				break;
			case 4:
				printf("Exiting...\n");
				break;
			default:
				printf("Invalid choice! Please enter a number between 1 and 4.\n");
				cin.ignore();
				cin.get();
		}
	} while (choice != 4);
}

void printLogOperation(string operation, vector<string> transactionList) {
	int numTransactions = transactionList.size();
	int numThreads = 4;
	int chunkSize = (numTransactions / numThreads) + ((numTransactions % numThreads != 0) ? 1 : 0);

	pthread_t threads[numThreads];
// 70 total
// 4 number
// 18
// 18 36  54  72
	for (int i = 0; i < numThreads; ++i) {
		int startIdx = i * chunkSize;
		int length = min(chunkSize, numTransactions - startIdx);

		TransactionThreadArgs* args = new TransactionThreadArgs;
		args->transactionList = &transactionList;
		args->startIdx = startIdx;
		args->length = length;
		args->targetOperation = operation;

		pthread_create(&threads[i], nullptr, OperationRunner, args);
	}

	for (int i = 0; i < numThreads; ++i) {
		pthread_join(threads[i], nullptr);
	}
	cin.ignore();
	cin.get();
}

void* OperationRunner(void* arg) {
	TransactionThreadArgs* args = reinterpret_cast<TransactionThreadArgs*>(arg);
	vector<string>& transactionList = *(args->transactionList);
	int startIdx = args->startIdx;
	int length = args->length;
	string targetOperation = args->targetOperation;

	for (int i = startIdx; i < startIdx + length; i++) {
		if (transactionList[i] == targetOperation) {
			// Print previous and next index if available

			
			if(targetOperation != "Login") {
cout << "Card#: " << transactionList[i - 1] << endl;
cout<<"Operation: " << transactionList[i] << endl;
cout<< "amount: "<< transactionList[i + 1];
cout << endl <<"---------------------"<<endl;;	


			}else{
cout << "Card#: " << transactionList[i - 1] << endl;
cout<<"Operation: " << transactionList[i] << endl;
cout << endl <<"---------------------"<<endl;;	
			}

			
		}
	}
	pthread_exit(nullptr);
}

void displayLogs(vector<string> transactionList) {
	int choice;
	string toOperation;
	do {
		clearScreen();
		cout << "1. Display All Logs\n";
		cout << "2. Filter by logs\n";
		cout << "3. Filter by deposits\n";
		cout << "4. Filter by withdrawals\n";
		cout << "5. Exit\n";

		cout << "Enter your choice: ";
		cin >> choice;

		switch(choice) {
			case 1:
				printCSV(transactionList);
				break;
			case 2:
				toOperation = "Login";
				break;
			case 3:
				toOperation = "Deposit";
				break;
			case 4:
				toOperation = "Withdraw";
				break;
			case 5:
				cout << "Exiting...\n";
				break;
			default:
				cout << "Invalid choice! Please enter a number between 1 and 4.\n";
		}


		if (choice >= 1 && choice <= 4) {
			// Call verifyOperation function with the selected operation
			printLogOperation(toOperation, transactionList);

		}
	} while (choice != 5);
}

// Function to calculate total deposit amount
void* searchRunner(void* arg) {
	AmountThreadArgs* args = reinterpret_cast<AmountThreadArgs*>(arg);
	vector<string>& transactionList = *(args->transactionList);
	int startIdx = args->startIdx;
	int length = args->length;
	pthread_t tid = pthread_self();
	for (int i = startIdx; i < startIdx + length; i++) {
		if(transactionList[i] == "Withdraw") {
			// Wait for the withdrawal semaphore
			if (sem_wait(&withdrawalSemaphore) != 0) {
				cerr << "Error: Failed to wait for the withdrawal semaphore." << endl;
				pthread_exit(nullptr);
			}
			
			cout<<endl<<"D> thread : "<< tid<<" Entered WCS added amount: "<< transactionList[i + 1]<<"["<<i+1<<"]";
			
			totalWithdrawalAmount += stod(transactionList[i + 1]);
			// Post the withdrawal semaphore
			if (sem_post(&withdrawalSemaphore) != 0) {
				cerr << "Error: Failed to post the withdrawal semaphore." << endl;
				pthread_exit(nullptr);
			}
			
			cout<<endl<<"D> thread : "<< tid<<" Exited WCS";
			
			
		} else if(transactionList[i] == "Deposit") {
			// Wait for the deposit semaphore
			if (sem_wait(&depositSemaphore) != 0) {
				cerr << "Error: Failed to wait for the deposit semaphore." << endl;
				pthread_exit(nullptr);
			}
			
			cout<<endl<<"D> thread : "<< tid<<" Entered DCS added amount: "<< transactionList[i + 1]<<"["<<i+1<<"]";
			
			totalDepositAmount += stod(transactionList[i + 1]);
			// Post the deposit semaphore
			if (sem_post(&depositSemaphore) != 0) {
				cerr << "Error: Failed to post the deposit semaphore." << endl;
				pthread_exit(nullptr);
			}
			
			cout<<endl<<"D> thread : "<< tid<<" Exited DCS";
			
		}
	}
	pthread_exit(nullptr);
}

void search(vector<string> transactionList) {
	int numThreads = 4;
	int totalUsers = transactionList.size();
	int chunkSize = totalUsers / numThreads;

	pthread_t threads[numThreads];
	AmountThreadArgs threadArgs[numThreads];

	if (sem_init(&depositSemaphore, 0, 1) != 0) {
		cerr << "Error: Failed to initialize the deposit semaphore." << endl;
		return;
	}
	if (sem_init(&withdrawalSemaphore, 0, 1) != 0) {
		cerr << "Error: Failed to initialize the withdrawal semaphore." << endl;
		return;
	}

	// Create threads
	for (int i = 0; i < numThreads; i++) {
		threadArgs[i].transactionList = &transactionList;
		threadArgs[i].startIdx = i * chunkSize;
		threadArgs[i].length = (i == numThreads - 1) ? (totalUsers - i * chunkSize) : chunkSize;

		pthread_create(&threads[i], NULL, searchRunner, reinterpret_cast<void*>(&threadArgs[i]));
		cout<<endl<<"D> ["<<threadArgs[i].startIdx<<"-";
		cout<<threadArgs[i].startIdx+threadArgs[i].length<<"] thread created: "<< threads[i];
	}
	for (int i = 0; i < numThreads; i++) {
		pthread_join(threads[i], NULL);
		cout<<endl<<"D> thread joined: "<< threads[i];
	}

	if (sem_destroy(&depositSemaphore) != 0) {
		cerr << "Error: Failed to destroy the deposit semaphore." << endl;
	}
	if (sem_destroy(&withdrawalSemaphore) != 0) {
		cerr << "Error: Failed to destroy the withdrawal semaphore." << endl;
	}

}

void displayTotalDeposited(vector<string> transactionList) {
	//clearScreen();
	search(transactionList);
	cout << endl<<endl << "Total Deposit Amount: $" << totalDepositAmount << endl<<endl;
	cin.ignore();
	cin.get();
}

void displayTotalWithdrawn(vector<string> transactionList) {
	//clearScreen();
	search(transactionList);
	cout << endl<<endl << "Total Withdrawan Amount: $" << totalWithdrawalAmount << endl<<endl;
	cin.ignore();
	cin.get();
}

/*
card#,operation,amount
card#,operation,amount
card#,operation,amount
card#,operation,amount

kon kon login kiya	DONE
kon kon deposit		DONE
kon kon withdraw	DONE

total withdraw amount    DONE
total deposit amount    DONE
*/
