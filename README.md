# Community Food Bank Inventory and Distribution System

`Group Project` · CMP1025 Programming 2

A menu-driven C console program for a community food bank. Administrators
record donations and track stock across several food bank locations.
Customers browse the inventory and request food items. The inventory is
saved to a text file, so it is kept between runs.

## Group Members

`Anthony Granston`
`Rhian Sang`
`Aldane Stennett`
`Matthew Earle`

## Contents

- [Features](#features)
- [Requirements](#requirements)
- [Build](#build)
- [Run](#run)
- [How to Use](#how-to-use)
- [Data File](#data-file)
- [Program Limits](#program-limits)
- [Code Structure](#code-structure)
- [Known Limitations](#known-limitations)

## Features

The program is split into four parts, all in `main.c`:

| Part | Feature | What it does |
| --- | --- | --- |
| 1 | Program Entry and Shared Design | Welcome menu, registration, login, the administrator and customer menus, input validation, and the shared constants and structures |
| 2 | Inventory and Donations | Lists items with stock per location, records donations (adding new items when needed), and shows low-stock items |
| 3 | Customer Requests | Lets a customer request a quantity of an item, checked against the total stock available |
| 4 | File Handling and Sorted Stock Report | Saves and loads the inventory, and shows items sorted by total stock (bubble sort) |

**Administrator** can:

1. List food items and stock by location
2. Record a donation
3. Display low-stock items
4. Sort and display food items by total stock (ascending or descending)
5. Save and exit

**Customer** can:

1. Make a food-item request
2. List food items and stock
3. Save and exit

## Requirements

- A C compiler that supports C11 (for example, GCC 9 or later)
- A terminal (Linux, macOS, or Windows with MinGW-w64 / MSYS2)

No external libraries are needed. The program only uses the standard
headers `<stdio.h>`, `<stdlib.h>`, `<string.h>` and `<limits.h>`.

## Build

From the project folder:

```bash
gcc -Wall -Wextra -std=c11 main.c -o main
```

On Windows (MinGW-w64):

```bash
gcc -Wall -Wextra -std=c11 main.c -o main.exe
```

The program should compile with no warnings.

## Run

Linux / macOS:

```bash
./main
```

Windows:

```bash
main.exe
```

The data file `foodbank_data.txt` is read from and saved to the folder you
run the program from. On the first run there is no file, so the program
prints `No data file found. Starting with no food items.` This is normal.

## How to Use

Every menu is navigated by typing the option number and pressing Enter.

### First run walkthrough

1. **Register an administrator.** At the welcome menu choose `1. Register`,
   enter a username and password, then choose role `1` (Administrator).
2. **Log in.** Choose `2. Login` and enter the same username and password.
   You are taken to the Administrator Menu.
3. **Add stock.** Choose `2. Record donation`.
   - Enter `0` to add a new item, then type its name (for example `Rice`).
   - Enter the location (`1` to `3`) and the donation amount.
   - To add more stock to an existing item, enter its item number instead of `0`.
4. **Check the inventory.** Use options `1`, `3` and `4` to list items, see
   low stock and view the sorted stock report.
5. **Save.** Choose `5. Save and exit`. The inventory is written to
   `foodbank_data.txt` and you return to the welcome menu.
6. **Make a request as a customer.** Register a second account with role
   `2` (Customer), log in, and choose `1. Make a food-item request`. Enter
   the item ID and the quantity needed.
7. **Exit.** Choose `3. Exit` at the welcome menu.

The next time the program starts, it loads the saved inventory
automatically (`Loaded N food items from foodbank_data.txt.`).

### Input rules

| Input | Rule |
| --- | --- |
| Menu choices | Must be a whole number shown in the menu |
| Username / password | Cannot be empty; username must be unique |
| Item name | Cannot be empty; up to 49 characters |
| Location | `1` to `3` |
| Donation amount / requested quantity | Whole number greater than 0 |
| Requested quantity | Cannot be more than the item's total stock across all locations |

Invalid input is rejected with a message and nothing is changed.

### Low stock

An item is reported as low stock at a location when its stock there is
below **5** (`LOW_STOCK_THRESHOLD`). Each location is checked on its own,
so one item can appear more than once.

## Data File

The inventory is saved as plain text in `foodbank_data.txt` (sequential
access). Every value is on its own line:

```
3          <- number of food items
1          <- item ID
Rice       <- item name (may contain spaces)
8          <- stock at Location 1
0          <- stock at Location 2
0          <- stock at Location 3
2
Beans
0
20
0
...
```

Item IDs must be `1, 2, 3 ...` in order. If the file is damaged (missing
lines, non-numbers, negative stock, IDs out of order, or too many items),
nothing is loaded and the program starts with an empty inventory instead
of using half-loaded data.

The data file is listed in `.gitignore`, so it is not committed.

## Program Limits

These values are set as constants at the top of `main.c`:

| Constant | Value | Meaning |
| --- | --- | --- |
| `MAX_USERS` | 20 | Registered accounts |
| `MAX_ITEMS` | 50 | Food item records |
| `NUM_LOCATIONS` | 3 | Food bank locations |
| `MAX_REQUESTS` | 100 | Customer requests per run |
| `MAX_NAME_LENGTH` | 30 | Username / password buffer (29 characters) |
| `MAX_ITEM_NAME_LENGTH` | 50 | Item name buffer (49 characters) |
| `LOW_STOCK_THRESHOLD` | 5 | Stock below this is "low" |

## Code Structure

```
P2-MainProject/
├── main.c        The whole program (Parts 1-4)
├── README.md     This file
└── .gitignore
```

`main.c` is divided into numbered, commented sections:

| Section | Contents |
| --- | --- |
| 1. Header Files | Standard library includes |
| 2. Constants | Limits, menu options, and constants for each part |
| 3. Structure Declarations | `User`, `FoodItem`, `Request` |
| 4. Function Prototypes | Grouped by part |
| 5. Main Function | Program loop; loads saved data at start-up |
| 6. Input Helper Functions | `readInteger()`, `readString()` |
| 7. Registration and Login Functions | Welcome menu, `registerUser()`, `loginUser()` |
| 8. Administrator Menu Functions | `runAdminMenu()` |
| 9. Customer Menu Functions | `runCustomerMenu()` |
| 10. PART 2 - Inventory and Donations | `listFoodItems()`, `recordDonation()`, `displayLowStockItems()`, `totalStock()` |
| 11. PART 3 - Customer Requests | `makeFoodRequest()`, `findItemIndexById()` |
| 12. PART 4 - File Handling and Sorted Stock Report | `loadData()`, `saveData()`, `sortFoodItemsByStock()` |

Design notes:

- **No global variables.** All data is declared in `main()` and passed to
  functions as parameters (pointers are used when a function changes a count).
- **Safe input.** All input is read a whole line at a time with `fgets()`,
  so invalid input never leaves the program stuck in a loop.
- **Shared stock total.** `totalStock()` (Part 2) is used by Parts 2, 3 and
  4. It returns a `long long` so the sum of several locations cannot overflow.
- **The report does not reorder the inventory.** The sorted stock report sorts a
  copy of the items, so item numbers stay the same.

## Known Limitations

- **Accounts are not saved.** Users exist only while the program runs, so
  you register again after restarting.
- **Requests are not saved and do not reduce stock.** A request is checked
  against available stock and recorded, but stock is not deducted.
- **Users choose their own role** when registering, including Administrator.
- **Passwords are stored as plain text** in memory.
- **Fixed capacity.** Arrays have fixed sizes (see [Program Limits](#program-limits)).
