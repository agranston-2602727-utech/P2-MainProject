// PART 3 - Customer Requests- Matthew-Earle Ellington 2004981


// Finds an item by its ID. Returns its index, or -1 if not found.
int findItemIndexById(const FoodItem items[], int itemCount, int itemId)
{
    int i;

    for (i = 0; i < itemCount; i++) {
        if (items[i].itemId == itemId) {
            return i;
        }
    }
    return -1;
}

// Adds up the stock of one item across all locations.

int itemTotalStock(const FoodItem *item)
{
    int loc;
    int total = 0;

    for (loc = 0; loc < NUM_LOCATIONS; loc++) {
        total += item->stock[loc];
    }
    return total;
}

// Lets a customer request a quantity of one food item and records the request.

void makeFoodRequest(const FoodItem items[], int itemCount,
                     Request requests[], int *requestCount)
{
    int itemId;
    int quantity;
    int index;
    int available;

     if (itemCount == 0) {
        printf("There are no food items available to request.\n");
        return;
    }
      if (*requestCount >= MAX_REQUESTS) {
        printf("Request failed: the maximum number of requests has been reached.\n");
        return;
    }

    printf("\n--- Make a Food Request ---\n");
       listFoodItems(items, itemCount);

     printf("Enter the item ID to request: ");
     itemId = readInteger();
     index = findItemIndexById(items, itemCount, itemId);
    if (index == -1) {
        printf("Request failed: that item ID does not exist.\n");
        return;
    }

     printf("Enter the quantity needed: ");
     quantity = readInteger();
     if (quantity <= 0) {
        printf("Request failed: quantity must be a whole number greater than 0.\n");
        return;
    }

    available = itemTotalStock(&items[index]);
     if (quantity > available) {
        printf("Request failed: only %d of %s available.\n",
               available, items[index].name);
        return;
    }

    // Save the request
     requests[*requestCount].requestId = *requestCount + 1;
     requests[*requestCount].itemId = itemId;
    requests[*requestCount].quantity = quantity;
    (*requestCount)++;

    printf("Request #%d recorded: %d x %s.\n",
           requests[*requestCount - 1].requestId, quantity, items[index].name);
}
}
