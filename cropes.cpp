#include<iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <map>
#include <limits>
#include <cctype>

using namespace std;
const vector<string> validSeasons = {"Spring", "Summer", "Autumn", "Winter"};
struct FarmingSeason {
    string seasonName;
    string cropName;
    int year;
    float expenses = 0;
    float income = 0;

    float profit() const {
        return income - expenses;
    }

    string profitOrLoss() const {
        float p = profit();
        if (p > 0) return "Profit";
        else if (p < 0) return "Loss";
        else return "Break-even";
    }
};
struct Node {
    FarmingSeason season;
    Node* next;
    Node(const FarmingSeason& s) : season(s), next(nullptr) {}
};

Node* head = nullptr;

int getCurrentYear() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    return now->tm_year + 1900;
}

bool isValidSeason(const string& season) {
    return find(validSeasons.begin(), validSeasons.end(), season) != validSeasons.end();
}

void displayFarmingSeason(const FarmingSeason& s) {
    cout << "Season: " << s.seasonName << " " << s.year
         << " | Crop: " << s.cropName
         << " | Expenses: $" << s.expenses
         << " | Income: $" << s.income
         << " | Profit: $" << s.profit()
         << " (" << s.profitOrLoss() << ")\n";
}

void insert(const FarmingSeason& s) {
    Node* newNode = new Node(s);
    if (!head) {
        head = newNode;
    } else {
        Node* curr = head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

void saveAllToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << " for saving.\n";
        return;
    }
    Node* curr = head;
    while (curr) {
        file << curr->season.seasonName << "," << curr->season.cropName << "," << curr->season.year << ","
             << curr->season.expenses << "," << curr->season.income << "\n";
        curr = curr->next;
    }
    file.close();
    cout << "All data saved to " << filename << ".\n";
}

void saveToFile(const FarmingSeason& s, const string& filename) {
    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << " for appending.\n";
        return;
    }
    file << s.seasonName << "," << s.cropName << "," << s.year << ","
         << s.expenses << "," << s.income << "\n";
    file.close();
}

void loadFromFile(const string& filename) {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No existing crop data file found. Starting with an empty list.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        size_t p3 = line.find(',', p2 + 1);
        size_t p4 = line.find(',', p3 + 1);
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos || p4 == string::npos) continue;

        FarmingSeason fs;
        fs.seasonName = line.substr(0, p1);
        fs.cropName = line.substr(p1 + 1, p2 - p1 - 1);
        fs.year = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        fs.expenses = stof(line.substr(p3 + 1, p4 - p3 - 1));
        fs.income = stof(line.substr(p4 + 1));
        insert(fs);
    }

    file.close();
    cout << "Crop data loaded from " << filename << ".\n";
}
void deleteBySeason(const string& season) {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }

    Node* current = head;
    Node* prev = nullptr;
    bool deletedAny = false;
    while (current != nullptr && current->season.seasonName == season) {
        head = current->next;
        delete current;
        current = head;
        deletedAny = true;
    }
    while (current != nullptr) {
        if (current->season.seasonName == season) {
            prev->next = current->next;
            delete current;
            current = prev->next;
            deletedAny = true;
        } else {
            prev = current;
            current = current->next;
        }
    }

    if (deletedAny) {
        saveAllToFile("crop.txt");
        cout << "All entries for season '" << season << "' deleted.\n";
    } else {
        cout << "No entries found for season '" << season << "'.\n";
    }
}

void deleteByCropName(const string& cropName) {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }

    Node* current = head;
    Node* prev = nullptr;
    bool deletedAny = false;

    while (current != nullptr && current->season.cropName == cropName) {
        head = current->next;
        delete current;
        current = head;
        deletedAny = true;
    }
    while (current != nullptr) {
        if (current->season.cropName == cropName) {
            prev->next = current->next;
            delete current;
            current = prev->next;
            deletedAny = true;
        } else {
            prev = current;
            current = current->next;
        }
    }

    if (deletedAny) {
        saveAllToFile("crop.txt");
        cout << "All entries for crop '" << cropName << "' deleted.\n";
    } else {
        cout << "No entries found for crop '" << cropName << "'.\n";
    }
}

void updateExpenses() {
    string season;
    cout << "Enter season name to update expense for: ";
    getline(cin, season);
    if (!isValidSeason(season)) {
        cout << "Invalid season.\n";
        return;
    }

    Node* found = nullptr;
    for (Node* curr = head; curr; curr = curr->next) {
        if (curr->season.seasonName == season) {
            found = curr;
            break;
        }
    }

    if (!found) {
        cout << "No entry found for season " << season << ".\n";
        return;
    }

    string reason;
    float amount;
    cout << "Enter reason for expense: ";
    getline(cin, reason);
    cout << "Enter expense amount for " << reason << ": ";
    while (!(cin >> amount)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    found->season.expenses += amount;
    saveAllToFile("crop.txt");
    cout << "Expense updated successfully.\n";
}

void updateIncome() {
    string season;
    cout << "Enter season name to update income for: ";
    getline(cin, season);
    if (!isValidSeason(season)) {
        cout << "Invalid season.\n";
        return;
    }

    Node* found = nullptr;
    for (Node* curr = head; curr; curr = curr->next) {
        if (curr->season.seasonName == season) {
            found = curr;
            break;
        }
    }

    if (!found) {
        cout << "No entry found for season " << season << ".\n";
        return;
    }

    float income;
    cout << "Enter new income: ";
    while (!(cin >> income)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    found->season.income = income;
    saveAllToFile("crop.txt");
    cout << "Income updated successfully.\n";
}
void displayAllSortedBySeason() {
    vector<FarmingSeason> entries;
    for (Node* curr = head; curr != nullptr; curr = curr->next)
        entries.push_back(curr->season);

    auto comparator = [](const FarmingSeason& a, const FarmingSeason& b) {
        auto idx = [](const string& s) {
            for (int i = 0; i < validSeasons.size(); i++) {
                if (validSeasons[i] == s) return i;
            }
            return 4;
        };
        return idx(a.seasonName) < idx(b.seasonName);
    };

    sort(entries.begin(), entries.end(), comparator);

    cout << "\n=== All Crop Entries ===\n";
    if (entries.empty()) {
        cout << "No crop records to display.\n";
        return;
    }
    for (const auto& e : entries) {
        displayFarmingSeason(e);
    }
}

void searchBySeason(const string& season) {
    bool found = false;
    cout << "\n=== Search Results for Season: " << season << " ===\n";
    for (Node* curr = head; curr != nullptr; curr = curr->next) {
        if (curr->season.seasonName == season) {
            displayFarmingSeason(curr->season);
            found = true;
        }
    }
    if (!found) cout << "No entries found for season " << season << ".\n";
}




















int main(){
loadFromFile("crop.txt");

    int choice;
    do {
        cout << "\n--- Crops Financial Tracker ---\n";
        cout << "1. Add Crop Entry\n";
        cout << "2. Display All Entries\n";
        cout << "3. Search by Season\n";
        cout << "4. Calculate Seasonal Profit/Loss\n";
        cout << "5. Calculate Annual Profit\n";
        cout << "6. Delete Entry\n";
        cout << "7. Add Expense by Season\n";
        cout << "8. Add Income by Season\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                FarmingSeason entry;
                string season_name_input;
                string crop_name_input;
                float expense;
                int year = getCurrentYear();

                int season_choice;
                cout << "Select Season Name:\n";
                cout << "1. Spring\n";
                cout << "2. Summer\n";
                cout << "3. Autumn\n";
                cout << "4. Winter\n";
                cout << "Enter your choice (1-4): ";
                while (!(cin >> season_choice) || season_choice < 1 || season_choice > 4) {
                    cout << "Invalid choice. Please enter a number between 1 and 4: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (season_choice) {
                    case 1: season_name_input = "Spring"; break;
                    case 2: season_name_input = "Summer"; break;
                    case 3: season_name_input = "Autumn"; break;
                    case 4: season_name_input = "Winter"; break;
                }

                while (true) {
                    cout << "Enter crop name: ";
                    getline(cin, crop_name_input);                   
                    if (crop_name_input.length() > 15) {
                        cout << "Crop name too long. Maximum 15 characters allowed.\n";
                        continue;
                    }
                    bool all_letters = true;
                    for (char c : crop_name_input) {
                        if (!isalpha(c) && c != ' ') { 
                            all_letters = false;
                            break;
                        }
                    }
                    if (all_letters && !crop_name_input.empty()) {
                        break;
                    } else {
                        cout << "Invalid crop name. Please use letters only.\n";
                    }
                }

                cout << "Enter expense : ";
                while (!(cin >> expense) || expense < 0) { 
                    cout << "Invalid input. Please enter a non-negative number: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                entry.seasonName = season_name_input;
                entry.cropName = crop_name_input;
                entry.year = year;
                entry.expenses = expense;
                entry.income = 0;

                insert(entry);
                saveToFile(entry, "crop.txt");
                cout << "Crop entry saved. Income is initially $0. Use Option 8 to update it later.\n";
                break;
            }

            case 2: displayAllSortedBySeason(); break;
            case 3: {
                string season;
                cout << "Enter season name to search: ";
                getline(cin, season);
                searchBySeason(season);
                break;
            }
            case 4: {
                string season;
                cout << "Enter season name to summarize: ";
                getline(cin, season);
                calculateSeasonalProfit(season);
                break;
            }
            case 5: calculateAnnualProfit(); break;
            case 6: {
                int deleteOption;
                cout << "\n--- Delete Entry Options ---\n";
                cout << "1. Delete by Season Name\n";
                cout << "2. Delete by Crop Name\n";
                cout << "Enter your choice (1 or 2): ";
                while (!(cin >> deleteOption) || (deleteOption != 1 && deleteOption != 2)) {
                    cout << "Invalid choice. Please enter 1 or 2: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (deleteOption == 1) {
                    string seasonToDelete;
                    cout << "Enter the *season name*: ";
                    getline(cin, seasonToDelete);
                    deleteBySeason(seasonToDelete);
                } else {
                    string cropNameToDelete;
                    cout << "Enter the *crop name*: ";
                    getline(cin, cropNameToDelete);
                    deleteByCropName(cropNameToDelete);
                }
                break;
            }
            case 7: updateExpenses(); break;
            case 8: updateIncome(); break;
            case 0: cout << "Goodbye.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    cleanupList();








    return 0;
}