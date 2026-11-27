/*
 * Community Food Bank Inventory and Distribution System
 * Part 1 - Program Entry and Shared Design
 *
 * This file contains the program entry point, user registration and login,
 * the administrator and customer menus, and the shared declarations
 * (constants, structures and function prototypes) used by Parts 2-4.
 *
 * Functions marked "PLACEHOLDER" belong to other group members. They only
 * print a message so the program compiles and the menus can be tested.
 * Replace them with the real implementations when they are available.
 *
 * Compile: gcc -Wall -Wextra -std=c11 main.c -o program
 * Run:     ./program
 */

// ==========================================
// 1. Header Files
// ==========================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>   // INT_MAX - used by Part 2 to stop stock overflowing

// ==========================================
// 2. Constants
// ==========================================

// Array capacities (PROVISIONAL - confirm with group)
#define MAX_USERS      20   // Maximum number of registered accounts
#define MAX_ITEMS      50   // Maximum number of food item records
#define NUM_LOCATIONS  3    // Number of food bank locations

// String lengths (include room for the '\0' terminator)
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

// PART 2 - Inventory and Donation constants
#define LOW_STOCK_THRESHOLD 5   // Stock below this at a location is "low"
#define NEW_ITEM_OPTION     0   // Entered in recordDonation() to add a new item

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
void runCustomerMenu(FoodItem items[], int *itemCount);

// PART 2 - Inventory and Donations
long long totalStock(const FoodItem *item);
void listFoodItems(const FoodItem items[], int itemCount);
void updateStock(int *quantity, int amount);
void recordDonation(FoodItem items[], int *itemCount);
void displayLowStockItems(const FoodItem items[], int itemCount);

// Interfaces for other parts (PLACEHOLDERS in this file)
void sortFoodItemsByStock(FoodItem items[], int itemCount);
void makeFoodRequest(FoodItem items[], int itemCount);
void saveData(const FoodItem items[], int itemCount);

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
                    runCustomerMenu(items, &itemCount);
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
void runCustomerMenu(FoodItem items[], int *itemCount)
{
    int choice;
    int inMenu = 1;

    while (inMenu) {
        displayCustomerMenu();
        choice = readInteger();

        switch (choice) {
        case CUSTOMER_MAKE_REQUEST:
            makeFoodRequest(items, *itemCount);
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

// ------------------------------------------
// PART 2 - Inventory and Donations
// ------------------------------------------
// Lets the administrator view the inventory, record donations (adding new
// food items when needed) and see which items are running low.
//
// Item numbering: items are numbered 1, 2, 3 ... in the order they are
// added, so an item's number is always (its array index + 1). The functions
// below rely on this to turn the number a user types into an array index.

// Adds up the stock of one item across every location.
// A long long is returned because the sum of several int values can be
// larger than the biggest value an int can hold.
long long totalStock(const FoodItem *item)
{
    long long stockTotal = 0;   // Running total of stock
    int locationIndex;          // Location being added (0 to NUM_LOCATIONS - 1)

    for (locationIndex = 0; locationIndex < NUM_LOCATIONS; locationIndex++) {
        stockTotal += item->stock[locationIndex];
    }

    return stockTotal;
}

// Prints every food item, its stock at each location and its total stock.
void listFoodItems(const FoodItem items[], int itemCount)
{
    int itemIndex;       // Item being printed
    int locationIndex;   // Location being printed for that item

    printf("\n--- FOOD INVENTORY ---\n");

    if (itemCount == 0) {
        printf("No food items available.\n");
        return;
    }

    for (itemIndex = 0; itemIndex < itemCount; itemIndex++) {
        // Item heading, e.g. "1. Rice"
        printf("\n%d. %s\n", items[itemIndex].itemId, items[itemIndex].name);

        // Stock at each location (shown to the user as Location 1, 2, 3)
        for (locationIndex = 0; locationIndex < NUM_LOCATIONS; locationIndex++) {
            printf("Location %d: %d\n",
                   locationIndex + 1, items[itemIndex].stock[locationIndex]);
        }

        printf("Total stock: %lld\n", totalStock(&items[itemIndex]));
    }
}

// Adds an amount to a stock value. The stock value is passed by pointer
// so the change is made to the original variable, not a copy.
void updateStock(int *quantity, int amount)
{
    *quantity += amount;
}

// Records a donation of one food item at one location.
// The administrator may pick an existing item or enter NEW_ITEM_OPTION (0)
// to add a new item. All input is checked before anything is changed, so
// a mistake never leaves a half-created item in the inventory.
void recordDonation(FoodItem items[], int *itemCount)
{
    int itemNumber;          // Item number typed by the user (0 = new item)
    int itemIndex;           // Array position of the item receiving the donation
    int locationNumber;      // Location typed by the user (1 to NUM_LOCATIONS)
    int locationIndex;       // Array position of that location
    int donationAmount;      // Number of units donated
    char newItemName[MAX_ITEM_NAME_LENGTH];   // Name, only used for a new item

    printf("\n--- RECORD DONATION ---\n");

    // Step 1: show the items so the user can choose one
    listFoodItems(items, *itemCount);

    // Step 2: read and check the item number
    printf("\nEnter item number or %d to add a new item: ", NEW_ITEM_OPTION);
    itemNumber = readInteger();

    if (itemNumber < NEW_ITEM_OPTION || itemNumber > *itemCount) {
        printf("Invalid item number.\n");
        return;
    }

    // Step 3: for a new item, check there is room and read its name
    if (itemNumber == NEW_ITEM_OPTION) {
        if (*itemCount >= MAX_ITEMS) {
            printf("Inventory is full.\n");
            return;
        }

        printf("Enter food name: ");
        readString(newItemName, MAX_ITEM_NAME_LENGTH);

        if (strlen(newItemName) == 0) {
            printf("Invalid food name.\n");
            return;
        }
    }

    // Step 4: read and check the location
    printf("Enter location (1-%d): ", NUM_LOCATIONS);
    locationNumber = readInteger();

    if (locationNumber < 1 || locationNumber > NUM_LOCATIONS) {
        printf("Invalid location.\n");
        return;
    }

    // Step 5: read and check the donation amount
    // (readInteger() returns -1 for invalid input, so it is rejected here too)
    printf("Enter donation amount: ");
    donationAmount = readInteger();

    if (donationAmount <= 0) {
        printf("Invalid donation amount.\n");
        return;
    }

    // Step 6: all input is valid - create the new item if one was requested
    if (itemNumber == NEW_ITEM_OPTION) {
        itemNumber = *itemCount + 1;   // Next item number in sequence

        items[*itemCount].itemId = itemNumber;
        strcpy(items[*itemCount].name, newItemName);

        // A new item starts with no stock at any location
        for (locationIndex = 0; locationIndex < NUM_LOCATIONS; locationIndex++) {
            items[*itemCount].stock[locationIndex] = 0;
        }

        (*itemCount)++;
    }

    // Convert the numbers the user typed (starting at 1) into array indexes
    itemIndex = itemNumber - 1;
    locationIndex = locationNumber - 1;

    // Step 7: stop the stock going past the largest value an int can hold
    if ((long long)items[itemIndex].stock[locationIndex] + donationAmount > INT_MAX) {
        printf("Donation amount is too large.\n");
        return;
    }

    // Step 8: add the donation and confirm it to the user
    updateStock(&items[itemIndex].stock[locationIndex], donationAmount);

    printf("\nDonation recorded successfully!\n");
    printf("Food item: %s\n", items[itemIndex].name);
    printf("New stock: %d\n", items[itemIndex].stock[locationIndex]);
}

// Lists every item and location where stock is below LOW_STOCK_THRESHOLD.
// Each location is checked on its own, so one item can appear more than once.
void displayLowStockItems(const FoodItem items[], int itemCount)
{
    int itemIndex;          // Item being checked
    int locationIndex;      // Location being checked for that item
    int lowStockFound = 0;  // Becomes 1 once any low-stock entry is printed

    printf("\n--- LOW STOCK ITEMS ---\n");

    for (itemIndex = 0; itemIndex < itemCount; itemIndex++) {
        for (locationIndex = 0; locationIndex < NUM_LOCATIONS; locationIndex++) {
            if (items[itemIndex].stock[locationIndex] < LOW_STOCK_THRESHOLD) {
                printf("%s - Location %d: %d left\n",
                       items[itemIndex].name, locationIndex + 1,
                       items[itemIndex].stock[locationIndex]);

                lowStockFound = 1;
            }
        }
    }

    if (lowStockFound == 0) {
        printf("No low stock items found.\n");
    }
}

// PLACEHOLDER - Part 2/3: sort items by total stock and display them
void sortFoodItemsByStock(FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] sortFoodItemsByStock() not yet implemented.\n");
}

// PLACEHOLDER - Part 2/3: let a customer request a food item
void makeFoodRequest(FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] makeFoodRequest() not yet implemented.\n");
}

// PLACEHOLDER - Part 4 (Member 4): save the food item records
void saveData(const FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] saveData() not yet implemented. Nothing was saved.\n");
}
