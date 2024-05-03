3rd May:
-System("cls") and System("pause") do not work on linux. FIXED and MADE SAFER
-Potential issue: CSV file is not read properly. NO ISSUE
-Changed ReadCSV() for better looking output
JOINING ALL CODE:

[On Upload]:
Code will read CSV file and create a vector in main()
user will input card number
verifyCN() will run
verifyCN() divides the list into equal sections and creates four threads. The four threads run a function verifyCNrunner();
verifyCNrunner() performs a linear search on the specified part of vector.

Problem:
The verifyCNrunner() needs parameters vector, starting index, length to search, target. Since we are using pthreads library only 1 argument can be passed.
Solved:
used struct to pass arguments

To Do:
Before implementing anyother functionality polish the current version thoroughly and document errors.
