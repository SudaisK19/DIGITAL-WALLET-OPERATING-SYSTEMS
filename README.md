# Digital Wallet System

This repository contains a **Digital Wallet System** developed as part of an **Operating Systems** course. The project is implemented in C++ and demonstrates multithreading concepts to ensure efficient and secure transaction processing.

---

## Project Overview

The **Digital Wallet System** simulates the functionalities of an electronic wallet, including:
- Sending and receiving money.
- Real-time transaction logging and database updates.
- Secure card verification using multithreaded search.
- Manager mode for efficient transaction history searches.

The project leverages multithreading using the `pthread` library to optimize performance and ensure concurrency.

---

## Key Features

1. **Core Functionalities**:
   - Send Money: Securely transfer money between accounts.
   - Receive Money: Update the wallet balance upon receiving funds.
   - Real-Time Database Updates: Ensure transactions are logged and reflected in real time.

2. **Multithreaded Search**:
   - Verify Card Number: Implements multithreaded linear search to validate card numbers.
   - Uses the `pthread` library to divide and conquer search operations for optimal efficiency.

3. **Manager Mode**:
   - Allows for efficient multithreaded searching of transaction logs.
   - Designed for secure and fast query handling.

4. **Platform Independence**:
   - Fixed compatibility issues with `system("cls")` and `system("pause")` on Linux systems.
   - Ensured cross-platform functionality.

---

## Implementation Details

### Multithreading
- **verifyCN()**:
  - Splits the list of card numbers into equal sections and spawns threads to search specific ranges.
  - Threads run the `verifyCNrunner()` function, which performs the linear search on the assigned range.

- **verifyCNrunner()**:
  - Accepts arguments as a structured object (to bypass the single argument limit of `pthread`).
  - Ensures accurate and efficient search results.

### Real-Time Database
- Reads and updates a CSV file to reflect the latest wallet transactions.
- Displays the transaction log dynamically.

### Debugging and Improvements
- Improved error handling for CSV reading and writing.
- Fixed cross-platform compatibility issues for terminal commands.

---

## Development Log

- **4th May**:
  - Implemented:
    - Send Money, Receive Money functionalities.
    - Real-time transaction log and database updates.
    - Multithreaded search for Manager mode.

- **3rd May**:
  - Fixed compatibility issues for `system("cls")` and `system("pause")` on Linux.
  - Optimized `ReadCSV()` for better output formatting.
  - Enhanced multithreaded search:
    - Used structures to pass multiple arguments to `pthread` functions.

---

## Course Details

- **Course**: Operating Systems
- **Institution**: [Insert Institution Name]

---

## Contributors

- **[Your Name]**
- **[Collaborator Name 1]**
- **[Collaborator Name 2]**

---

## Future Enhancements

- Add GUI support for user-friendly interactions.
- Include advanced security features like encryption for transactions.
- Implement multi-user support with role-based access controls.
- Extend the system to support multiple wallet types and currencies.

---

Explore the repository to understand the implementation and learn about multithreading in a real-world application!
