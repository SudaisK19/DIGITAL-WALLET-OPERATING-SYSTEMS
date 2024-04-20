So far:
Code will read CSV file and create a vector in main()
user will input card number
verifyCN() will run
verifyCN() divides the list into equal sections and creates four threads. The four threads run a function verifyCNrunner();
verifyCNrunner() performs a linear search on the specified part of vector.

Problem:
The verifyCNrunner() needs parameters vector, starting index, length to search, target. Since we are using pthreads library only 1 argument can be passed.
