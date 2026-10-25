#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>
#include <windows.h>

using namespace std;

// Set the console text color to pink.
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void setPink() {
    SetConsoleTextAttribute(hConsole, 0xD0);
}

void resetColor() {
    // Keep the pink background and use black text.
    SetConsoleTextAttribute(hConsole, 0xD0);
}

// Low-stock warning limit.
const int LOW_STOCK_LIMIT = 5;

// Product information
struct Product {
    int id;
    string name;
    double price;
    int quantity;
    int soldQuantity;

    Product() = default;

    Product(int i, string n, double p, int q, int s) {
        id = i;
        name = n;
        price = p;
        quantity = q;
        soldQuantity = s;
    }
};

// Item information in the cart
struct CartItem {
    int id;
    string name;
    double price;
    int quantity;
};

class Market {
private:
    static const int MaxProducts = 100;
    static const int MaxCart = 50;

    Product inventory[MaxProducts];
    CartItem cart[MaxCart];
    int productCount;
    int cartCount;
    int totalInvoices;
    double totalSales;

    // Find a product using its ID
    int findProductIndex(int productId) {
        for (int i = 0; i < productCount; i++) {
            if (inventory[i].id == productId) {
                return i;
            }
        }
        return -1;
    }

    // Find a product by name, ignoring uppercase and lowercase letters
    int findProductByName(string productName) {
        transform(productName.begin(), productName.end(), productName.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });

        for (int i = 0; i < productCount; i++) {
            string currentName = inventory[i].name;
            transform(currentName.begin(), currentName.end(), currentName.begin(),
                      [](unsigned char c) { return static_cast<char>(tolower(c)); });

            if (currentName.find(productName) != string::npos) {
                return i;
            }
        }
        return -1;
    }

    // Show a warning when stock is low
    void showLowStockWarning(int index) {
        if (inventory[index].quantity <= LOW_STOCK_LIMIT) {
            setPink();
            cout << "[LOW STOCK] Only "
                 << inventory[index].quantity << " left for "
                 << inventory[index].name << ".\n";
            resetColor();
        }
    }

    // Clear the cart
    void clearCart() {
        cartCount = 0;
    }

public:
    // Set the starting values
    Market() {
        productCount = 0;
        cartCount = 0;
        totalInvoices = 0;
        totalSales = 0;
    }

    // Add sample products
    void loadDefaultProducts() {
        inventory[0] = Product(101, "Milk", 1.50, 20, 0);
        inventory[1] = Product(102, "Bread", 0.80, 15, 0);
        inventory[2] = Product(103, "Cheese", 3.00, 4, 0);
        inventory[3] = Product(104, "Water", 0.50, 50, 0);
        inventory[4] = Product(105, "Juice", 2.20, 12, 0);
        productCount = 5;
    }

    // Show all products
    void displayProduct() {
        setPink();
        cout << "\n" << string(82, '=') << "\n";
        resetColor();

        cout << left << setw(10) << "ID"
             << setw(20) << "Product Name"
             << setw(15) << "Price ($)"
             << setw(20) << "Available Qty"
             << setw(15) << "Sold Qty" << "\n";

        setPink();
        cout << string(82, '-') << "\n";
        resetColor();

        for (int i = 0; i < productCount; i++) {
            cout << left << setw(10) << inventory[i].id
                 << setw(20) << inventory[i].name
                 << setw(15) << fixed << setprecision(2) << inventory[i].price
                 << setw(20) << inventory[i].quantity
                 << setw(15) << inventory[i].soldQuantity;

            if (inventory[i].quantity <= LOW_STOCK_LIMIT) {
                setPink();
                cout << " [LOW STOCK]";
                resetColor();
            }
            cout << "\n";
        }

        setPink();
        cout << string(82, '=') << "\n";
        resetColor();

        // Warn about all products with low stock.
        for (int i = 0; i < productCount; i++) {
            showLowStockWarning(i);
        }
    }

    // Search for a product by ID or name
    void searchProduct() {
        int searchChoice;

        setPink();
        cout << "\n------- PRODUCT SEARCH ---------\n";
        resetColor();

        cout << "1. Search by ID\n";
        cout << "2. Search by name\n";
        cout << "Enter choice: ";

        if (!(cin >> searchChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice\n";
            return;
        }

        int index = -1;

        if (searchChoice == 1) {
            int id;
            cout << "Enter Product ID: ";

            if (!(cin >> id)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a number.\n";
                return;
            }

            index = findProductIndex(id);

        } else if (searchChoice == 2) {
            string name;
            cout << "Enter product name: ";
            cin >> ws;
            getline(cin, name);
            index = findProductByName(name);

        } else {
            cout << "Invalid option \n";
            return;
        }

        if (index == -1) {
            cout << "Product not found :( \n";
        } else {
            cout << "\nID: " << inventory[index].id << "\n";
            cout << "Name: " << inventory[index].name << "\n";
            cout << "Price: $" << fixed << setprecision(2)
                 << inventory[index].price << "\n";
            cout << "Available quantity: " << inventory[index].quantity << "\n";
            cout << "Sold quantity today: " << inventory[index].soldQuantity << "\n";
            showLowStockWarning(index);
        }
    }

    // Add products to the cart
    void addToCart() {
        if (productCount == 0) {
            cout << "No products are available \n";
            return;
        }

        char answer = 'y';

        do {
            int productId;
            int requestedQty;

            displayProduct();
            cout << "Enter Product ID: ";

            if (!(cin >> productId)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a number \n";
                return;
            }

            int index = findProductIndex(productId);

            if (index == -1) {
                cout << "Product not found \n";
            } else {
                cout << "Enter quantity: ";

                if (!(cin >> requestedQty)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Please enter a whole number \n";
                    return;
                }

                if (requestedQty <= 0) {
                    cout << "Quantity must be greater than zero \n";
                } else if (requestedQty > inventory[index].quantity) {
                    cout << "Not enough items in stock \n";
                } else if (cartCount == MaxCart) {
                    cout << "The cart is full \n";
                    return;
                } else {
                    cart[cartCount].id = inventory[index].id;
                    cart[cartCount].name = inventory[index].name;
                    cart[cartCount].price = inventory[index].price;
                    cart[cartCount].quantity = requestedQty;
                    cartCount++;

                    inventory[index].quantity -= requestedQty;

                    cout << "Product added to cart \n";
                    showLowStockWarning(index);
                }
            }

            cout << "Add another product? (y/n): ";
            cin >> answer;

        } while (answer == 'y' || answer == 'Y');
    }

    // Calculate the invoice
    void calculateInvoice(double &grossTotal, double &netAmount,
                          double &discountAmount) {
        grossTotal = 0;

        for (int i = 0; i < cartCount; i++) {
            grossTotal = grossTotal + cart[i].price * cart[i].quantity;
        }

        if (grossTotal >= 20) {
            discountAmount = grossTotal * 0.10;
        } else {
            discountAmount = 0;
        }

        netAmount = grossTotal - discountAmount;
    }

    // Print the invoice and receive payment
    void checkout() {
        if (cartCount == 0) {
            cout << "The cart is empty. Add products first.\n";
            return;
        }

        double grossTotal = 0;
        double netAmount = 0;
        double discountAmount = 0;

        calculateInvoice(grossTotal, netAmount, discountAmount);

        setPink();
        cout << "\n-------------SHOPPING RECEIPT -------------\n";
        resetColor();

        cout << left << setw(20) << "Product Name"
             << setw(10) << "Qty"
             << setw(15) << "Price ($)"
             << setw(15) << "Total ($)" << "\n";

        setPink();
        cout << string(60, '-') << "\n";
        resetColor();

        for (int i = 0; i < cartCount; i++) {
            double itemTotal = cart[i].price * cart[i].quantity;

            cout << left << setw(20) << cart[i].name
                 << setw(10) << cart[i].quantity
                 << setw(15) << fixed << setprecision(2) << cart[i].price
                 << setw(15) << itemTotal << "\n";
        }

        setPink();
        cout << string(60, '-') << "\n";
        resetColor();

        cout << "Total before discount: " << grossTotal << "\n";
        cout << "Discount:              " << discountAmount << "\n";
        cout << "Amount to pay:          " << netAmount << "\n";

        double paidAmount;
        cout << "Enter amount paid:      ";

        if (!(cin >> paidAmount)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid payment amount \n";
            return;
        }

        if (paidAmount < netAmount) {
            cout << "Payment is not enough. Try again \n";
            return;
        }

        // Count sold items only after payment succeeds.
        for (int i = 0; i < cartCount; i++) {
            int index = findProductIndex(cart[i].id);

            if (index != -1) {
                inventory[index].soldQuantity += cart[i].quantity;
            }
        }

        cout << "Change:          " << paidAmount - netAmount << "\n";
        cout << "Thank you for shopping! \n";

        totalInvoices++;
        totalSales = totalSales + netAmount;
        clearCart();
    }

    // Add, edit, or search for products
    void manageInventory() {
        int choice;

        cout << "\n------- INVENTORY CONTROL --------\n";
        cout << "1. Add new product\n";
        cout << "2. Edit product\n";
        cout << "3. Search product\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice \n";
            return;
        }

        if (choice == 1) {
            if (productCount >= MaxProducts) {
                cout << "Inventory is full \n";
                return;
            }

            int id, quantity;
            string name;
            double price;

            cout << "Enter product ID: ";

            if (!(cin >> id)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid ID \n";
                return;
            }

            if (findProductIndex(id) != -1) {
                cout << "This ID already exists \n";
                return;
            }

            cout << "Enter product name (one word): ";
            cin >> name;

            cout << "Enter price: ";

            if (!(cin >> price)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid price \n";
                return;
            }

            cout << "Enter quantity: ";

            if (!(cin >> quantity)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid quantity \n";
                return;
            }

            if (price < 0 || quantity < 0) {
                cout << "Price and quantity cannot be negative \n";
                return;
            }

            inventory[productCount] = Product(id, name, price, quantity, 0);
            productCount++;

            cout << "Product added successfully \n";
            showLowStockWarning(productCount - 1);

        } else if (choice == 2) {
            int id;

            cout << "Enter product ID to edit: ";

            if (!(cin >> id)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid ID \n";
                return;
            }

            int index = findProductIndex(id);

            if (index == -1) {
                cout << "Product not found :( \n";
                return;
            }

            double newPrice;
            int newQuantity;

            cout << "Enter new price: ";

            if (!(cin >> newPrice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid price.\n";
                return;
            }

            cout << "Enter new quantity: ";

            if (!(cin >> newQuantity)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid quantity \n";
                return;
            }

            if (newPrice < 0 || newQuantity < 0) {
                cout << "Price and quantity cannot be negative \n";
                return;
            }

            inventory[index].price = newPrice;
            inventory[index].quantity = newQuantity;

            cout << "Product updated successfully \n";
            showLowStockWarning(index);

        } else if (choice == 3) {
            // Search by either ID or product name.
            searchProduct();

        } else {
            cout << "Invalid option \n";
        }
    }

    // Show sales and stock information
    void displayReports() {
        int totalUnitsSold = 0;
        double inventoryValue = 0;
        int bestIndex = -1;

        for (int i = 0; i < productCount; i++) {
            totalUnitsSold += inventory[i].soldQuantity;
            inventoryValue += inventory[i].price * inventory[i].quantity;

            if (bestIndex == -1 ||
                inventory[i].soldQuantity > inventory[bestIndex].soldQuantity) {
                bestIndex = i;
            }
        }

        setPink();
        cout << "\n------------SALES REPORT -----------\n";
        resetColor();

        cout << "Completed invoices: " << totalInvoices << "\n";
        cout << "Total sales: " << fixed << setprecision(2) << totalSales << "\n";
        cout << "Total units sold today: " << totalUnitsSold << "\n";
        cout << "Inventory value: " << inventoryValue << "\n";

        if (bestIndex != -1 && totalUnitsSold > 0) {
            cout << "Most sold product today: " << inventory[bestIndex].name
                 << " (" << inventory[bestIndex].soldQuantity << " units)\n";
        } else {
            cout << "Most sold product today: No sales yet \n";
        }

        setPink();
        cout << "-------------------\n";
        resetColor();
    }
};

int main() {
    Market myMarket;

    // Use a pink background with black text.
    system("color D0");
    system("cls");
    setPink();

    myMarket.loadDefaultProducts();

    int choice = 0;

    do {
        setPink();
        cout << "\n----------------------------------------\n";
        cout << "          MINI MARKET POS SYSTEM         \n";
        cout << "-----------------------------------------\n";
        resetColor();

        cout << "1. Display Products\n";
        cout << "2. Add Products to Cart\n";
        cout << "3. Print Invoice and Pay\n";
        cout << "4. Inventory Control\n";
        cout << "5. Sales Report\n";
        cout << "6. Exit\n";

        setPink();
        cout << "-----------------------------------------\n";
        resetColor();

        cout << "Enter your choice (1-6): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number \n";
            choice = 0;
            continue;
        }

        // Run the selected menu option.
        switch (choice) {
            case 1:
                myMarket.displayProduct();
                break;
            case 2:
                myMarket.addToCart();
                break;
            case 3:
                myMarket.checkout();
                break;
            case 4:
                myMarket.manageInventory();
                break;
            case 5:
                myMarket.displayReports();
                break;
            case 6:
                setPink();
                cout << "Thank you for using the system :) \n";
                resetColor();
                break;
            default:
                cout << "Please choose a number from 1 to 6 \n";
        }

    } while (choice != 6);

    resetColor();
    return 0;
}
