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

// Interfaces for Parts 2-4 (PLACEHOLDERS in this file)
void listFoodItems(const FoodItem items[], int itemCount);
void recordDonation(FoodItem items[], int *itemCount);
void displayLowStockItems(const FoodItem items[], int itemCount);
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

// ==========================================
// 10. Integration Functions for Parts 2-4
// ==========================================
// PLACEHOLDERS ONLY. Each function prints a message so the menus can be
// tested. Delete each placeholder when the group member's real function
// is added to the project.

// PLACEHOLDER - Part 2/3: list every food item with its stock per location
void listFoodItems(const FoodItem items[], int itemCount)
{
    (void)items;
    printf("[PLACEHOLDER] listFoodItems() not yet implemented (%d items stored).\n", itemCount);
}

// PLACEHOLDER - Part 2/3: add donated stock (may add a new item, so the count is a pointer)
void recordDonation(FoodItem items[], int *itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] recordDonation() not yet implemented.\n");
}

// PLACEHOLDER - Part 2/3: show items whose stock is below the agreed threshold
void displayLowStockItems(const FoodItem items[], int itemCount)
{
    (void)items;
    (void)itemCount;
    printf("[PLACEHOLDER] displayLowStockItems() not yet implemented.\n");
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
