# Flight Management System (BST)

C++ Flight Management System using a Binary Search Tree (BST), built for the Data Structures and Algorithms course (Module 1).

**Group members:** Sajal Khan, Zamman Ashfaq

## What it does
Flight records are stored in a BST ordered by unique Flight ID. Duplicate IDs are rejected.

- Add, search, delete (leaf, one child, two children) and display flights (ascending Flight ID, inorder traversal)
- Calculate ETA = Departure Time + Duration and mark each flight On-Time, Early or Delayed
- Surprise feature: search all flights of an airline
- Save records to `flights.txt` and load them automatically at start-up

## File format
One flight per line, comma-separated:

`101,Lahore,Karachi,PIA,09:30,11:15,1:45/1.75,18000,Scheduled`

Order: ID, source, destination, airline, departure, arrival, duration (H:MM/hours), price (PKR), status. Do not use commas inside fields.

## How to run
1. Keep `main.cpp` and `flights.txt` in the same folder.
2. Compile: `g++ main.cpp -o flights` (or open `main.cpp` in Visual Studio and run).
3. Run `flights` and use the menu.
