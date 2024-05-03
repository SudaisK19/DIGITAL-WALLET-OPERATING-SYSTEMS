#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

void readCSV(const string& filename, vector<string>& userList);
void printCSV(vector<string> userList);
int verifyCN(const string cardNumber, vector<string> userList);
void* verifyCNrunner(void* args);
void balanceInquiry(int index, const vector<string>& userList);
void clearScreen();

struct ThreadArgs {
    vector<string>* userList;
    int startIdx;
    int length;
    string targetCN;
};

volatile bool found = false; // Shared memory variable to indicate if target is found

int main() {
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

	
	//When verified the loop breaks to the menu below: result-> index is now aqcuired	
		//menu
	int choice;
    	do {
    	clearScreen();
        // Display menu
        cout << "E-Wallet Menu\n";
        cout << "1. Send Money\n";
        cout << "2. Receive Money\n";
        cout << "3. Balance Inquiry\n";
        cout << "4. Log Out\n";
        cout << "Choose an option (1-4): ";

        // Read user choice
        cin >> choice;

        // Switch structure for menu actions
        switch (choice) {
            //case 1:
                //sendMoney(); // Implement your send money logic here
                //break;

            //case 2:
              //  receiveMoney(); // Implement your receive money logic here
                //break;

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

void clearScreen(){
	cout << "\033[2J\033[H"; // safe replacement for system("clear");
	};

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

void printCSV(vector<string> userList) {
    /*
        This function prints the Vector which 
        represents the CSV file
    */
    int nl=3;
    for (const auto& user : userList) {
    	if(nl==0 || nl==3){
    	nl = 3;
    	cout << endl << user;
    	nl--;
    	}
    	else{
        cout <<", " << user;
        nl--;
        }
    }
}

int verifyCN(const string cardNumber, vector<string> userList) {
    /*
        This function creates n number of threads
        and implements fair load for linear search
        among multiple threads [data parallelism]
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
    	if(found){
    		cout<<"D> Found! while "<<tid<<" was searching ["<<i<<"], Exiting..."<<endl;
    		pthread_exit(resultIndex);        
        }
        if (userList[i] == targetCN) {
      	    cout<<"D> Found by Thread "<<tid<<" Exiting..."<<endl;
    	
    	    *resultIndex = i;	
            found = true; //set flag true if found
    	    pthread_exit(resultIndex);        
      
        }
    }
    cout<<"D> Thread "<<tid<<" Finished Search Exiting..."<<endl;
    pthread_exit(resultIndex);
}

//Shaheer Edit


void balanceInquiry(int index, const vector<string>& userList){
// Prints the current users details
    clearScreen();
    cout << "\tBalance inquiry\n\nCard Number: " << userList[index] << endl;

    cout << "Full Name: " <<  userList[index+1] << endl;

    cout << "Current Balance: $" <<  userList[index+2] << endl;
	
    cin.ignore();
    cin.get();
}
