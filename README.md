# OOP Judge

A simple object-oriented online judge system built in C++. This project includes a console and SFML UI for managing contests, users, problems, submissions, and results.

## Features

- User registration and login
- Problem bank and contest management
- Submission handling and result evaluation
- Score calculation and contest ranking
- Optional SFML-based user interface

## Project Structure

- `OOP-Online-Judge/` – main application source and headers
- `OOP-Online-Judge/src/` – implementation files
- `OOP-Online-Judge/include/` – class headers
- `OOP-Online-Judge/sfml_ui/` – SFML UI implementation
- `VFS/` – virtual file system demo code
- `Docs/` – project notes and instructions

## Requirements

- C++17 or later
- SFML (if running the SFML UI version)
- A C++ compiler such as `g++`, `clang`, or MSVC

## Build and Run

1. Open a terminal in the project folder.
2. Compile the project using your preferred build system.
   Example with `g++`:
   ```bash
   g++ -std=c++17 -I OOP-Online-Judge/include OOP-Online-Judge/src/*.cpp -o OOP_Judge
   3. Run the executable:
   ```bash
   ./OOP_Judge
