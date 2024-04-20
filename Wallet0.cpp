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
struct ThreadArgs {
    vector<string>* userList;
    int startIdx;
    int length;
    string targetCN;
};

int main() {
    vector<string> userList;
    readCSV("Users.csv", userList); // fill the userList

    // Print the contents of the list (for verification)
    // printCSV(userList);

    string cardNumber;

    // Loop to prompt user for card number input
    int index;
    do {
        system("cls");
        cout << "Enter card number (or 'quit' to exit): ";
        cin >> cardNumber;

        if (cardNumber == "quit") {
            cout << "Exiting program." << endl;
            exit(0);
        }

        index = verifyCN(cardNumber, userList);
        if (index >= 0) {
            cout << "Card number verified. User found at index " << index << "." << endl;
            break;
        } else {
            cout << "Card number not found. Please try again." << endl;
            system("pause");
        }
    } while (true);

    return 0;
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

void printCSV(vector<string> userList) {
    for (const auto& user : userList) {
        cout << user << endl;
    }
}

int verifyCN(const string cardNumber, vector<string> userList) {
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
    }

    // Join threads
    for (int i = 0; i < numThreads; ++i) {
        int* returnedIndex;
        pthread_join(threads[i], reinterpret_cast<void**>(&returnedIndex));
        if (*returnedIndex != -1) {
            resultIndex = *returnedIndex;
            break;
        }
    }

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

    for (int i = startIdx; i < startIdx + length; i++) {
        if (userList[i] == targetCN) {
            *resultIndex = i;
            break;
        }
    }
    pthread_exit(resultIndex);
}
