/*
 * Community Food Bank Inventory and Distribution System
 * Part 1 - Program Entry and Shared Design
 *
 * This file contains the entire program for Part 1, which includes the
 * program entry point, user registration and login,
 * the administrator and customer menus, and the shared declarations
 * (constants, structures and function prototypes) used by Parts 2-4.
 *
 * Part 3 (Customer Requests, by Matthew-Earle Ellington "2004981") has been
 * integrated into this file. Its constants, structure, variables and
 * functions are kept separate from Part 1 and are labelled "PART 3".
 *
 * Functions marked "PLACEHOLDER" belong to other group members. They only
 * print a message so the program compiles and the menus can be tested.
 * Replace them with the real implementations when they are available.
 *
 * Completed by Anthony Granston "2602727"
 * Compile: gcc -Wall -Wextra -std=c11 main.c -o main
 * Run:     ./main
 */

// 1. Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 2. Constants

// Array capacities 
// Note For Self Confirm the Contants values with group members 
#define MAX_USERS      20   // Maximum number of registered accounts
#define MAX_ITEMS      50   // Maximum number of food item records
#define NUM_LOCATIONS  3    // Number of food bank locations

#define MAX_NAME_LENGTH      30   // Username and password length
#define MAX_ITEM_NAME_LENGTH 50   // Food item name length
#define MAX_INPUT_LENGTH     100  // Size of the buffer used to read input

// Role identifiers
#define ROLE_ADMIN    1
#define ROLE_CUSTOMER 2

// Welcome menu options
#define WELCOME_REGISTER 1
#define WELCOME_LOGIN    2
#define WELCOME_EXIT     3

// Administrator menu options
#define ADMIN_LIST_ITEMS      1
#define ADMIN_RECORD_DONATION 2
#define ADMIN_LOW_STOCK       3
#define ADMIN_SORT_BY_STOCK   4
#define ADMIN_SAVE_EXIT       5

// Customer menu options
#define CUSTOMER_MAKE_REQUEST 1
#define CUSTOMER_LIST_ITEMS   2
#define CUSTOMER_SAVE_EXIT    3

// Value returned by loginUser() when login fails
#define LOGIN_FAILED -1

// PART 3 - Customer Request constants
#define MAX_REQUESTS   100  // Maximum number of customer requests stored
#define ITEM_NOT_FOUND -1   // Returned by findItemIndexById() when no item matches

// ==========================================
// 3. Structure Declarations
// ==========================================

// One registered account
typedef struct {
    char username[MAX_NAME_LENGTH];
    char password[MAX_NAME_LENGTH];
    int role;                       // ROLE_ADMIN or ROLE_CUSTOMER
} User;

// One food item record (PROVISIONAL - fields must be agreed with Parts 2-4)
// The stock array holds the quantity at each location.
// The array index is the location identifier (0 to NUM_LOCATIONS - 1).
typedef struct {
    int itemId;
    char name[MAX_ITEM_NAME_LENGTH];
    int stock[NUM_LOCATIONS];
} FoodItem;

// PART 3 - One customer request for a food item
typedef struct {
    int requestId;   // Request number, starting at 1
    int itemId;      // ID of the FoodItem that was requested
    int quantity;    // How many units the customer asked for
} Request;

// ==========================================
// 4. Function Prototypes
// ==========================================

// Input helpers
int readInteger(void);
void readString(char text[], int size);

// Welcome menu, registration and login
void displayWelcomeMenu(void);
void registerUser(User users[], int *userCount);
int findUser(const User users[], int userCount, const char username[]);
int loginUser(const User users[], int userCount);

// Role menus
void displayAdminMenu(void);
void runAdminMenu(FoodItem items[], int *itemCount);
void displayCustomerMenu(void);
void runCustomerMenu(FoodItem items[], int *itemCount,
                     Request requests[], int *requestCount);

// Interfaces for Parts 2 and 4 (PLACEHOLDERS in this file)
void listFoodItems(const FoodItem items[], int itemCount);
void recordDonation(FoodItem items[], int *itemCount);
void displayLowStockItems(const FoodItem items[], int itemCount);
void sortFoodItemsByStock(FoodItem items[], int itemCount);
void saveData(const FoodItem items[], int itemCount);

// PART 3 - Customer Requests
int findItemIndexById(const FoodItem items[], int itemCount, int searchItemId);
int itemTotalStock(const FoodItem *item);
void makeFoodRequest(const FoodItem items[], int itemCount,
                     Request requests[], int *requestCount);

// ==========================================
// 5. Main Function
// ==========================================
int main(void)
{
    // Shared data is declared here and passed to functions (no globals)
    User users[MAX_USERS];
    int userCount = 0;

    FoodItem items[MAX_ITEMS];
    int itemCount = 0;

    // PART 3 - customer requests made during this run of the program
    Request requests[MAX_REQUESTS];
    int requestCount = 0;

    int choice;
    int userIndex;
    int running = 1;

    while (running) {
        displayWelcomeMenu();
        choice = readInteger();

        switch (choice) {
        case WELCOME_REGISTER:
            registerUser(users, &userCount);
            break;

        case WELCOME_LOGIN:
            userIndex = loginUser(users, userCount);
            if (userIndex != LOGIN_FAILED) {
                // Send the user to the menu that matches their role
                if (users[userIndex].role == ROLE_ADMIN) {
                    runAdminMenu(items, &itemCount);
                } else {
                    runCustomerMenu(items, &itemCount, requests, &requestCount);
                }
            }
            break;

        case WELCOME_EXIT:
            printf("Goodbye!\n");
            running = 0;
            break;

        default:
            printf("Invalid choice. Please enter 1, 2 or 3.\n");
            break;
        }
    }

    return 0;
}

// ==========================================
// 6. Input Helper Functions
// ==========================================

// Reads one line and converts it to an integer.
// Returns -1 if the line is not a whole number (so menus treat it as invalid).
// Ends the program if input has ended (EOF) to avoid an infinite loop.
int readInteger(void)
{
    char buffer[MAX_INPUT_LENGTH];
    int value;
    char extra;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("\nInput ended. Exiting program.\n");
        exit(0);
    }

    // Accept the line only if it holds exactly one integer
    if (sscanf(buffer, "%d %c", &value, &extra) != 1) {
        return -1;
    }
    return value;
}

// Reads one line of text into the given array and removes the newline.
// Ends the program if input has ended (EOF).
void readString(char text[], int size)
{
    char buffer[MAX_INPUT_LENGTH];

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("\nInput ended. Exiting program.\n");
        exit(0);
    }

    buffer[strcspn(buffer, "\n")] = '\0';   // Remove the newline
    strncpy(text, buffer, size - 1);
    text[size - 1] = '\0';                  // Make sure the text is terminated
}

// ==========================================
// 7. Registration and Login Functions
// ==========================================

void displayWelcomeMenu(void)
{
    printf("\n===== Community Food Bank =====\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

// Adds a new account to the users array.
// PROVISIONAL: the user chooses their role here until the group decides
// how roles and administrator accounts should be created.
void registerUser(User users[], int *userCount)
{
    User newUser;

    if (*userCount >= MAX_USERS) {
        printf("Registration failed: the maximum number of users has been reached.\n");
        return;
    }

    printf("\n--- Register ---\n");
    printf("Enter username: ");
    readString(newUser.username, MAX_NAME_LENGTH);
    if (strlen(newUser.username) == 0) {
        printf("Registration failed: username cannot be empty.\n");
        return;
    }
    if (findUser(users, *userCount, newUser.username) != LOGIN_FAILED) {
        printf("Registration failed: that username is already taken.\n");
        return;
    }

    printf("Enter password: ");
    readString(newUser.password, MAX_NAME_LENGTH);
    if (strlen(newUser.password) == 0) {
        printf("Registration failed: password cannot be empty.\n");
        return;
    }

    printf("Select role (1 = Administrator, 2 = Customer): ");
    newUser.role = readInteger();
    if (newUser.role != ROLE_ADMIN && newUser.role != ROLE_CUSTOMER) {
        printf("Registration failed: invalid role.\n");
        return;
    }

    users[*userCount] = newUser;
    (*userCount)++;
    printf("Registration successful. You can now log in.\n");
}

// Searches for a username. Returns its index, or LOGIN_FAILED if not found.
int findUser(const User users[], int userCount, const char username[])
{
    int i;

    for (i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return LOGIN_FAILED;
}

// Asks for a username and password and checks them against the users array.
// Returns the index of the logged-in user, or LOGIN_FAILED.
int loginUser(const User users[], int userCount)
{
    char username[MAX_NAME_LENGTH];
    char password[MAX_NAME_LENGTH];
    int index;

    if (userCount == 0) {
        printf("No accounts are registered. Please register first.\n");
        return LOGIN_FAILED;
    }

    printf("\n--- Login ---\n");
    printf("Enter username: ");
    readString(username, MAX_NAME_LENGTH);
    printf("Enter password: ");
    readString(password, MAX_NAME_LENGTH);

    index = findUser(users, userCount, username);
    if (index == LOGIN_FAILED || strcmp(users[index].password, password) != 0) {
        printf("Login failed: incorrect username or password.\n");
        return LOGIN_FAILED;
    }

    printf("Login successful. Welcome, %s!\n", users[index].username);
    return index;
}

// ==========================================
// 8. Administrator Menu Functions
// ==========================================

void displayAdminMenu(void)
{
    printf("\n===== Administrator Menu =====\n");
    printf("1. List food items and stock by location\n");
    printf("2. Record donation\n");
    printf("3. Display low-stock items\n");
    printf("4. Sort and display food items by total stock\n");
    printf("5. Save and exit\n");
    printf("Enter your choice: ");
}

// Shows the administrator menu until "Save and exit" is chosen.
void runAdminMenu(FoodItem items[], int *itemCount)
{
    int choice;
    int inMenu = 1;

    while (inMenu) {
        displayAdminMenu();
        choice = readInteger();

        switch (choice) {
        case ADMIN_LIST_ITEMS:
            listFoodItems(items, *itemCount);
            break;
        case ADMIN_RECORD_DONATION:
            recordDonation(items, itemCount);
            break;
        case ADMIN_LOW_STOCK:
            displayLowStockItems(items, *itemCount);
            break;
        case ADMIN_SORT_BY_STOCK:
            sortFoodItemsByStock(items, *itemCount);
            break;
        case ADMIN_SAVE_EXIT:
            saveData(items, *itemCount);
            inMenu = 0;   // Return to the welcome menu
            break;
        default:
            printf("Invalid choice. Please enter a number from 1 to 5.\n");
            break;
        }
    }
}

// ==========================================
// 9. Customer Menu Functions
// ==========================================

void displayCustomerMenu(void)
{
    printf("\n===== Customer Menu =====\n");
    printf("1. Make a food-item request\n");
    printf("2. List food items and stock\n");
    printf("3. Save and exit\n");
    printf("Enter your choice: ");
}

// Shows the customer menu until "Save and exit" is chosen.
void runCustomerMenu(FoodItem items[], int *itemCount,
                     Request requests[], int *requestCount)
{
    int choice;
    int inMenu = 1;

    while (inMenu) {
        displayCustomerMenu();
        choice = readInteger();

        switch (choice) {
        case CUSTOMER_MAKE_REQUEST:
            makeFoodRequest(items, *itemCount, requests, requestCount);
            break;
        case CUSTOMER_LIST_ITEMS:
            listFoodItems(items, *itemCount);
            break;
        case CUSTOMER_SAVE_EXIT:
            saveData(items, *itemCount);
            inMenu = 0;   // Return to the welcome menu
            break;
        default:
            printf("Invalid choice. Please enter a number from 1 to 3.\n");
            break;
        }
    }
}

// ==========================================
// 10. Integration Functions for Parts 2 and 4
// ==========================================
// PLACEHOLDERS ONLY. Each function prints a message so the menus can be
// tested. Delete each placeholder when the group member's real function
// is added to the project.

// PLACEHOLDER - Part 2: list every food item with its stock per location
void listFoodItems(const FoodItem items[], int itemCount)
{
    (void)items;
    printf("[PLACEHOLDER] listFoodItems() not yet implemented (%d items stored).\n", itemCount);
}

// PLACEHOLDER - Part 2: add donated stock (may add a new item, so the count is a pointer)
void recordDonation(FoodItem items[], int *itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] recordDonation() not yet implemented.\n");
}

// PLACEHOLDER - Part 2: show items whose stock is below the agreed threshold
void displayLowStockItems(const FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] displayLowStockItems() not yet implemented.\n");
}

// PLACEHOLDER - Part 2: sort items by total stock and display them
void sortFoodItemsByStock(FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] sortFoodItemsByStock() not yet implemented.\n");
}

// PLACEHOLDER - Part 4 (Member 4): save the food item records
void saveData(const FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] saveData() not yet implemented. Nothing was saved.\n");
}

// ==========================================
// 11. PART 3 - Customer Requests
// ==========================================
// Completed by Matthew-Earle Ellington "2004981"
//
// A customer chooses a food item by its ID and the quantity they need.
// The request is checked against the total stock of that item (all
// locations added together) and, if valid, stored in the requests array.
//
// Part 3 uses its own data (the Request structure, the requests array and
// requestCount, declared in main) and its own local variable names, so it
// does not share or overwrite any Part 1 variables.

// Searches the items array for the item with the given ID.
// Returns the array index of that item, or ITEM_NOT_FOUND if no item matches.
int findItemIndexById(const FoodItem items[], int itemCount, int searchItemId)
{
    int itemPosition;   // Current position being checked in the items array

    for (itemPosition = 0; itemPosition < itemCount; itemPosition++) {
        if (items[itemPosition].itemId == searchItemId) {
            return itemPosition;   // Match found
        }
    }
    return ITEM_NOT_FOUND;         // Checked every item, no match
}

// Adds up the stock of one item across all food bank locations.
// Returns the total number of units available.
int itemTotalStock(const FoodItem *item)
{
    int locationIndex;    // Location being added (0 to NUM_LOCATIONS - 1)
    int stockTotal = 0;   // Running total of stock

    for (locationIndex = 0; locationIndex < NUM_LOCATIONS; locationIndex++) {
        stockTotal += item->stock[locationIndex];
    }
    return stockTotal;
}

// Lets a customer request a quantity of one food item and records the request.
// The request is rejected (nothing is stored) if:
//   - there are no food items,
//   - the requests array is full,
//   - the item ID does not exist,
//   - the quantity is not a whole number above 0, or
//   - the quantity is more than the total stock available.
// Note: this only records the request; it does not remove stock.
void makeFoodRequest(const FoodItem items[], int itemCount,
                     Request requests[], int *requestCount)
{
    int requestedItemId;     // Item ID typed by the customer
    int requestedQuantity;   // Quantity typed by the customer
    int requestedItemIndex;  // Position of the requested item in the items array
    int availableStock;      // Total stock of the requested item
    Request newRequest;      // The request being built before it is stored

    // Step 1: make sure a request is possible at all
    if (itemCount == 0) {
        printf("There are no food items available to request.\n");
        return;
    }
    if (*requestCount >= MAX_REQUESTS) {
        printf("Request failed: the maximum number of requests has been reached.\n");
        return;
    }

    // Step 2: show the items so the customer can choose an ID
    printf("\n--- Make a Food Request ---\n");
    listFoodItems(items, itemCount);

    // Step 3: read and check the item ID
    printf("Enter the item ID to request: ");
    requestedItemId = readInteger();
    requestedItemIndex = findItemIndexById(items, itemCount, requestedItemId);
    if (requestedItemIndex == ITEM_NOT_FOUND) {
        printf("Request failed: that item ID does not exist.\n");
        return;
    }

    // Step 4: read and check the quantity
    // (readInteger() returns -1 for invalid input, so it is rejected here too)
    printf("Enter the quantity needed: ");
    requestedQuantity = readInteger();
    if (requestedQuantity <= 0) {
        printf("Request failed: quantity must be a whole number greater than 0.\n");
        return;
    }

    // Step 5: make sure enough stock exists across all locations
    availableStock = itemTotalStock(&items[requestedItemIndex]);
    if (requestedQuantity > availableStock) {
        printf("Request failed: only %d of %s available.\n",
               availableStock, items[requestedItemIndex].name);
        return;
    }

    // Step 6: build the request and store it at the next free position
    newRequest.requestId = *requestCount + 1;   // Request numbers start at 1
    newRequest.itemId    = requestedItemId;
    newRequest.quantity  = requestedQuantity;

    requests[*requestCount] = newRequest;
    (*requestCount)++;

    printf("Request #%d recorded: %d x %s.\n",
           newRequest.requestId, requestedQuantity, items[requestedItemIndex].name);
}
