#include <iostream>
#include <string>
#include <vector>
#include <fstream> 
#include <sstream> 
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;


// Base class for all types of items managed by GTN Manager
class Item {
public:
    string title;
    string description;

    // Constructor initializes title and description
    Item(const string& title, const string& description) : title(title), description(description) {}

    // Pure virtual functions to be implemented by derived classes
    virtual void display() const = 0;
    virtual string getDetails() const = 0;
  
    // Virtual destructor for proper cleanup of derived classes
    virtual ~Item() {} 
};


// Task class derived from Item for managing tasks
class Task : public Item {
public:
    string deadline;
    int priority;

    // Constructor initializes Task attributes along with inherited attributes
    Task(const string& title, const string& description, const string& deadline, int priority) :
        Item(title, description), deadline(deadline), priority(priority) {}

    // Displays task information
    void display() const override {
        cout << "Task: " << title << ", Deadline: " << deadline << ", Priority: " << priority << endl;
    }

    // Returns task details as a formatted string
    string getDetails() const override {
        return "Title: " + title + "\nDescription: " + description + "\nDeadline: " + deadline + "\nPriority: " + to_string(priority);
    }
};


// Specialized Task class for recurring tasks
class RecurringTask : public Task {
public:
    string recurrenceInterval;

    RecurringTask(const string& title, const string& description, const string& deadline, int priority, const string& interval) :
        Task(title, description, deadline, priority), recurrenceInterval(interval) {}

    void display() const override {
        cout << "Recurring Task: " << title << ", Deadline: " << deadline << ", Priority: " << priority << ", Interval: " << recurrenceInterval << endl;
    }

    string getDetails() const override {
        return Task::getDetails() + "\nRecurrence Interval: " + recurrenceInterval;
    }
};


// Specialized Task class for one-time tasks
class OneTimeTask : public Task {
public:
    OneTimeTask(const string& title, const string& description, const string& deadline, int priority) :
        Task(title, description, deadline, priority) {}

    void display() const override {
        cout << "One-Time Task: " << title << ", Deadline: " << deadline << ", Priority: " << priority << endl;
    }

    string getDetails() const override {
        return Task::getDetails();
    }
};


// Note class derived from Item for managing notes
class Note : public Item {
public:
    vector<string> tags;// Tags associated with the note

    Note(const string& title, const string& description, const vector<string>& tags) :
        Item(title, description), tags(tags) {}

    // Display basic note information along with tags
    void display() const override {
        cout << "Note: " << title << " [Tags: ";
        for (const auto& tag : tags) {
            cout << tag << " ";
        }
        cout << "]" << endl;
    }

    // Returns detailed information about the note including all tags
    string getDetails() const override {
        string details = "Title: " + title + "\nDescription: " + description + "\nTags: ";
        for (const auto& tag : tags) {
            details += tag + ", ";
        }
        // Remove trailing comma and space from tags list
        details.pop_back(); // Remove the last comma
        details.pop_back(); // Remove the last space
        return details;
    }
};

// ProtectedNote class for notes that require a password to access
class ProtectedNote : public Note {
public:
    string password; // Password for accessing the note

    ProtectedNote(const string& title, const string& description, const vector<string>& tags, const string& password) :
        Note(title, description, tags), password(password) {}

    void display() const override {
        cout << "Protected Note: " << title << " [Protected]" << endl;
    }

    string getDetails() const override {
        return Note::getDetails() + "\nPassword Protected";
    }
};

// PublicNote class for notes that are publicly accessible
class PublicNote : public Note {
public:
    PublicNote(const string& title, const string& description, const vector<string>& tags) :
        Note(title, description, tags) {}

    // Display public note information
    void display() const override {
        cout << "Public Note: " << title << " [Tags: ";
        for (const auto& tag : tags) {
            cout << tag << " ";
        }
        cout << "]" << endl;
    }

    // Returns details of the public note
    string getDetails() const override {
        return Note::getDetails();
    }
};


// Goal base class derived from Item, for managing goals with a progress attribute
class Goal : public Item {
protected:
    double progress;// Progress percentage of the goal

public:
    Goal(const string& title, const string& description, double progress) : Item(title, description), progress(progress) {}

    // Display goal information including progress percentage
    virtual void display() const override {
        cout << "Goal: " << title << ", Progress: " << fixed << setprecision(0) << progress * 100 << "%" << endl;
    }
    // Returns details of the goal including formatted progress
    virtual string getDetails() const override {
        return "Title: " + title + "\nDescription: " + description + "\nProgress: " + to_string(static_cast<int>(progress * 100)) + "%";
    }
    // Returns the current progress of the goal
    virtual double getProgress() const {
        return progress;
    }
};

// QuantifiableGoal class derived from Goal for goals that have quantifiable progress
class QuantifiableGoal : public Goal {
public:
    QuantifiableGoal(const string& title, const string& description, double progress) : Goal(title, description, progress) {}

    void display() const override {
        cout << "Quantifiable Goal: " << title << ", Progress: " << fixed << setprecision(0) << progress * 100 << "%" << endl;
    }

    string getDetails() const override {
        return Goal::getDetails();
    }

    double getProgress() const override {
        return progress;
    }
};

// NonQuantifiableGoal class for goals where progress is not quantified
class NonQuantifiableGoal : public Goal {
public:
    NonQuantifiableGoal(const string& title, const string& description, double progress) : Goal(title, description, progress) {}

    void display() const override {
        cout << "Non-Quantifiable Goal: " << title << " - Progress not quantified." << endl;
    }

    string getDetails() const override {
        return Goal::getDetails() + "\nNon-quantifiable progress";
    }

    double getProgress() const override {
        // Return a special value indicating non-quantifiability
        return -1.0;
    }
};

// Utility function to split strings based on a delimiter
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to load data from a file into the system
void loadDataFromFile(const string& filename, vector<Item*>& items) {
    ifstream file(filename);  // Open the file for reading
    string line, type, title, description, deadline, tags, password, interval;
    int priority;
    double progress;

    // Read each line from the file
    while (getline(file, line)) {
        stringstream ss(line);  // Use stringstream for parsing the line
        getline(ss, type, ',');  // Get the type of the item

        if (type == "Task" || type == "RecurringTask" || type == "OneTimeTask") {
            getline(ss, title, ',');
            getline(ss, description, ',');
            getline(ss, deadline, ',');
            ss >> priority;
            ss.ignore(1, ','); // Ignore the comma after reading priority
            if (type == "RecurringTask") {
                getline(ss, interval); // Read the interval
                items.push_back(new RecurringTask(title, description, deadline, priority, interval));
            }
            else if (type == "OneTimeTask") {
                items.push_back(new OneTimeTask(title, description, deadline, priority));
            }
            else {
                items.push_back(new Task(title, description, deadline, priority));
            }
        }
        else if (type == "Note" || type == "ProtectedNote" || type == "PublicNote") {
            getline(ss, title, ',');
            getline(ss, description, ',');
            getline(ss, tags, ',');
            if (type == "ProtectedNote") {
                getline(ss, password); // Read the password
                vector<string> tagList = split(tags, ',');
                items.push_back(new ProtectedNote(title, description, tagList, password));
            }
            else if (type == "PublicNote") {
                vector<string> tagList = split(tags, ',');
                items.push_back(new PublicNote(title, description, tagList));
            }
            else {
                vector<string> tagList = split(tags, ',');
                items.push_back(new Note(title, description, tagList));
            }
        }
        else if (type == "Goal" || type == "QuantifiableGoal" || type == "NonQuantifiableGoal") {
            getline(ss, title, ',');
            getline(ss, description, ',');
            ss >> progress;
            ss.ignore(); // Skip newline at the end
            if (type == "QuantifiableGoal") {
                items.push_back(new QuantifiableGoal(title, description, progress));
            }
            else if (type == "NonQuantifiableGoal") {
                items.push_back(new NonQuantifiableGoal(title, description, progress));
            }
            else {
                items.push_back(new Goal(title, description, progress));
            }
        }
    }

    file.close();  // Close the file after reading
}



// Function to merge two halves of a vector sorted by task priority
void merge(vector<Task*>& tasks, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
   
    // Create temp arrays
    vector<Task*> L(n1), R(n2);

    // Copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++)
        L[i] = tasks[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = tasks[mid + 1 + j];

    // Merge the temp arrays back into tasks[left..right]
    int i = 0; // Initial index of first subarray
    int j = 0; // Initial index of second subarray
    int k = left; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i]->priority <= R[j]->priority) {
            tasks[k] = L[i];
            i++;
        }
        else {
            tasks[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        tasks[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        tasks[k] = R[j];
        j++;
        k++;
    }
}


// Merge sort function for sorting tasks
void mergeSort(vector<Task*>& tasks, int left, int right) {
    if (left >= right) {
        return; // Recursively return if there is only one element
    }
    int mid = left + (right - left) / 2; // Calculate the middle point
    mergeSort(tasks, left, mid); // Recursively sort the left half
    mergeSort(tasks, mid + 1, right); // Recursively sort the right half
    merge(tasks, left, mid, right); // Merge the two halves
}

// Function to merge two halves of a vector sorted by task deadline
void mergeByDeadline(vector<Task*>& tasks, int left, int mid, int right) {
    int n1 = mid - left + 1; // Number of elements in the first half
    int n2 = right - mid; // Number of elements in the second half

    vector<Task*> L(n1), R(n2); // Temporary arrays for left and right halves

    // Copy data to temporary arrays L[] and R[]
    for (int i = 0; i < n1; i++)
        L[i] = tasks[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = tasks[mid + 1 + j];

    int i = 0; // Initial index of first subarray
    int j = 0; // Initial index of second subarray
    int k = left; // Initial index of merged subarray
    // Merge the temp arrays back into tasks[left..right]
    while (i < n1 && j < n2) {
        if (L[i]->deadline <= R[j]->deadline) {
            tasks[k] = L[i++];
        }
        else {
            tasks[k] = R[j++];
        }
        k++;
    }

    // Copy the remaining elements of L[], if any
    while (i < n1) {
        tasks[k++] = L[i++];
    }
    // Copy the remaining elements of R[], if any
    while (j < n2) {
        tasks[k++] = R[j++];
    }
}

// Function to sort tasks by their deadlines using merge sort
void mergeSortByDeadline(vector<Task*>& tasks, int left, int right) {
    if (left >= right) return; // Base case for recursion
    int mid = left + (right - left) / 2;
    mergeSortByDeadline(tasks, left, mid); // Sort the first half
    mergeSortByDeadline(tasks, mid + 1, right); // Sort the second half
    mergeByDeadline(tasks, left, mid, right); // Merge the two halves
}

// Function to display all items in the inventory
void displayAllItems(const vector<Item*>& items) {
    for (auto& item : items) {
        item->display(); // Call the display function polymorphically
        cout << endl;
    }
}

// Function to handle tasks submenu
void handleTasks(vector<Item*>& items) {
    int taskChoice;
    do {
        cout << "-----------------------------------------\n\n";
        cout << "\tTasks Menu\n\n";
        cout << "1. Display All Tasks\n";
        cout << "2. View Generic Tasks Details\n";
        cout << "3. View Recurring Tasks Details\n";
        cout << "4. View One-Time Tasks Details\n";
        cout << "5. Sort tasks by priority\n";
        cout << "6. Sort Tasks by deadline\n";
        cout << "7. Go Back\n\n";
        cout << "-----------------------------------------\n";

        if (!(cin >> taskChoice)) {
            cin.clear(); // Clears error state of the stream
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignores incorrect input up to the maximum limit
            cout << "Invalid input. Please enter a number.\n";
            continue; // Continue to the next iteration of the loop
        }

        vector<Task*> tasks;
        for (auto& item : items) {
            if (Task* task = dynamic_cast<Task*>(item)) {
                tasks.push_back(task);
            }
        }

        switch (taskChoice) {
        case 1:
            cout << "All Tasks:\n\n";
            for (auto& task : tasks) {// Displays all tasks 
                task->display();
                cout << endl;
            }
            break;
        case 2:
            cout << "\tGeneric Tasks Details:\n\n";
            for (auto& task : tasks) { //Goes through and shows all generic task details
                if (task != nullptr && dynamic_cast<RecurringTask*>(task) == nullptr && dynamic_cast<OneTimeTask*>(task) == nullptr) {
                    cout << task->getDetails() << endl << endl;
                }
                else if (task == nullptr) {
                    cout << "Error: Encountered a null task pointer." << endl;
                }
            }
            break;
        case 3:
            cout << "\tAll Recurring Tasks Details:\n" << endl;
            for (auto& task : tasks) {//Goes through and shows all Recurring task details
                if (RecurringTask* recurringTask = dynamic_cast<RecurringTask*>(task)) {
                    cout << recurringTask->getDetails() << endl << endl;
                }
            }
            break;
        case 4:
            cout << "\tAll One-Time Tasks Details:\n" << endl;
            for (auto& task : tasks) {//Goes through and shows all Non-Recurring task details
                if (OneTimeTask* oneTimeTask = dynamic_cast<OneTimeTask*>(task)) {
                    cout << oneTimeTask->getDetails() << endl << endl;
                }
            }
            break;
        case 5:
            mergeSort(tasks, 0, tasks.size() - 1);
            cout << "\tTasks sorted by priority:\n" << endl;
            for (auto& task : tasks) {
                task->display();
                cout << endl;
            }
            break;
        case 6:
            mergeSortByDeadline(tasks, 0, tasks.size() - 1);
            cout << "\tTasks sorted by deadline:\n" << endl;
            for (auto& task : tasks) {
                task->display();
                cout << endl;
            }
            break;
        case 7:
            return; // Exit the task menu
        default:
            cout << "Invalid choice, please choose again." << endl;
            break;
        }
        // Clears any extraneous input.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (taskChoice != 7);
}

// Heapify function for HeapSort adjusted for goals
void heapify(vector<Goal*>& goals, int n, int i) {
    int largest = i;  // Initialize largest as root
    int left = 2 * i + 1;  // Left child index
    int right = 2 * i + 2; // Right child index

    // Check if left child exists and is greater than root
    if (left < n && goals[left]->getProgress() > goals[largest]->getProgress() && goals[left]->getProgress() != -1)
        largest = left;

    // Check if right child exists and is greater than the current largest
    if (right < n && goals[right]->getProgress() > goals[largest]->getProgress() && goals[right]->getProgress() != -1)
        largest = right;

    // If largest is not root, swap and continue heapifying
    if (largest != i) {
        swap(goals[i], goals[largest]);
        heapify(goals, n, largest);
    }
}

// HeapSort function to sort goals by progress
void heapSort(vector<Goal*>& goals) {
    int n = goals.size();

    // Build heap from the array
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(goals, n, i);

    // Extract elements one by one from the heap
    for (int i = n - 1; i >= 0; i--) {
        // Move current root to end
        swap(goals[0], goals[i]);

        // Call heapify on the reduced heap
        heapify(goals, i, 0);
    }
}

void handleGoals(vector<Item*>& items) {
    int goalChoice;
    do {
        cout << "-----------------------------------------\n";
        cout << "\tGoals Menu\n\n";
        cout << "1. Display All Goals\n";
        cout << "2. View Generic Goals Details\n";
        cout << "3. View Quantifiable Goals Details\n";
        cout << "4. View Non-Quantifiable Goals Details\n";
        cout << "5. Sort goals by progress\n";
        cout << "6. Go Back\n";
        cout << "-----------------------------------------\n";

        if (!(cin >> goalChoice)) {
            cin.clear(); // Clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore incorrect input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        vector<Goal*> allGoals;
        for (auto& item : items) {
            if (Goal* goal = dynamic_cast<Goal*>(item)) {
                allGoals.push_back(goal);
            }
        }

        switch (goalChoice) {
        case 1:
            cout << "\tAll Goals:\n" << endl;
            for (auto& goal : allGoals) {
                goal->display();
                cout << endl;
            }
            break;
        case 2:
            cout << "\tGeneric Goals Details:\n\n";
            for (auto& goal : allGoals) {
                if (goal != nullptr) {
                    QuantifiableGoal* quantGoal = dynamic_cast<QuantifiableGoal*>(goal);
                    NonQuantifiableGoal* nonQuantGoal = dynamic_cast<NonQuantifiableGoal*>(goal);
                    if (!quantGoal && !nonQuantGoal) {
                        cout << goal->getDetails() << endl << endl;
                    }
                }
                else {
                    cout << "Error: Encountered a null goal pointer." << endl;
                }
            }
            break;
        case 3:
            cout << "\tQuantifiable Goals Details:\n" << endl;
            for (auto& item : items) {
                QuantifiableGoal* quantGoal = dynamic_cast<QuantifiableGoal*>(item);
                if (quantGoal) {
                    cout << quantGoal->getDetails() << endl << endl;
                }
            }
            break;
        case 4:
            cout << "\tNon-Quantifiable Goals Details:\n" << endl;
            for (auto& goal : allGoals) {
                if (goal->getProgress() == -1) {
                    cout << goal->getDetails() << endl << endl;
                }
            }
            break;
        case 5:
            heapSort(allGoals);
            cout << "\tGoals sorted by progress:\n" << endl;
            for (auto& goal : allGoals) {
                goal->display();
                cout << endl;
            }
            break;
        case 6:
            return;  // Exit the loop
        default:
            cout << "Invalid choice, please choose again." << endl;
        }
        // Clear the buffer to handle any extraneous input.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (goalChoice != 6);
}


// Helper function to search for notes by a specific tag.
void searchNotesByTag(const vector<Note*>& notes, const string& tag) {
    bool found = false;
    for (auto note : notes) {
        // Check if the tag exists in the note's tags vector.
        if (find(note->tags.begin(), note->tags.end(), tag) != note->tags.end()) {
            note->display(); // Display the note if the tag is found.
            cout << endl;
            found = true;
        }
    }
    // If no note with the tag is found, print a message indicating so.
    if (!found) {
        cout << "No notes found with that tag." << endl;
    }
}

// Helper function to compute the partial match table 
vector<int> computeKMPTable(const string& pattern) {
    int m = pattern.size(); // The length of the pattern
    vector<int> lps(m, 0); // Longest prefix which is also suffix
    int len = 0; // length of the previous longest prefix suffix
    int i = 1; // index for which we are computing the lps value

    // Loop to fill lps array
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        }
        else if (len) {
            len = lps[len - 1];
        }
        else {
            lps[i++] = 0;
        }
    }
    return lps;
}

// Function to search a text for a given pattern using the KMP searching algorithm.
bool KMPSearch(const string& text, const string& pattern) {
    if (pattern.empty()) return false; // Handle the edge case where pattern is an empty string.

    int n = text.size(); // The length of the text to search within.
    int m = pattern.size(); // The length of the pattern to search for.
    int i = 0; // Index for text.
    int j = 0; // Index for pattern.

    vector<int> lps = computeKMPTable(pattern); // Preprocess the pattern to create the lps array.

    // Loop through the text to find the pattern.
    while (i < n) {
        if (text[i] == pattern[j]) {
            if (++j == m) {
                return true; // Match found when we reach the end of the pattern.
            }
            i++;
        }
        else if (j) {
            j = lps[j - 1]; // Use lps array to avoid unnecessary comparisons.
        }
        else {
            i++;
        }
    }
    return false; // Return false if no match is found.
}


// Utility to convert string to lower case
string toLowerCase(const string& str) {
    string lowerCaseStr;
    transform(str.begin(), str.end(), back_inserter(lowerCaseStr),
        [](unsigned char c) { return tolower(c); });
    return lowerCaseStr;
}

// Full text search across all note fields
void searchNotesFullText(const vector<Note*>& notes, const string& searchText) {
    bool found = false;
    cout << "\nSearching all note fields for: " << searchText << "\n\n " << endl;
    for (const auto& note : notes) {
        string fullText = note->title + " " + note->description + " ";
        for (const auto& tag : note->tags) {
            fullText += tag + " ";
        }

        // Convert fullText and searchText to lowercase before searching
        if (KMPSearch(toLowerCase(fullText), toLowerCase(searchText))) {
            note->display();
            cout << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No matching notes found." << endl;
    }
}

void handleNotes(vector<Item*>& items) {
    int noteChoice;
    do {
        cout << "-----------------------------------------\n";
        cout << "\tNotes Menu\n\n";
        cout << "1. Display All Notes\n";
        cout << "2. View Generic Notes Details\n";
        cout << "3. View Protected Notes Details\n";
        cout << "4. View Unprotected Notes Details\n";
        cout << "5. Search for a note (full text)\n";
        cout << "6. Search note by tags\n";
        cout << "7. Go Back\n";
        cout << "-----------------------------------------\n";

        if (!(cin >> noteChoice)) {
            cin.clear(); // Clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        vector<Note*> notes;
        for (auto& item : items) {
            if (Note* note = dynamic_cast<Note*>(item)) {
                notes.push_back(note);
            }
        }

        switch (noteChoice) {
        case 1:
            cout << "All Notes:\n\n";
            for (auto& note : notes) {
                note->display();
                cout << endl;
            }
            break;
        case 2:  // Now this will only display details for generic notes
            cout << "\tGeneric Notes Details:\n\n";
            for (auto& note : notes) {
                if (note != nullptr && dynamic_cast<ProtectedNote*>(note) == nullptr && dynamic_cast<PublicNote*>(note) == nullptr) {
                    cout << note->getDetails() << endl << endl;
                }
                else if (note == nullptr) {
                    cout << "Note is null and cannot be displayed.\n";
                }
            }
            break;
        case 3: {
            cout << "\tProtected Notes Details:\n\n";
            bool accessGranted = false;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer just in case

            for (auto& note : notes) {
                ProtectedNote* protectedNote = dynamic_cast<ProtectedNote*>(note);
                if (protectedNote) {
                    cout << "\nEnter password to view " << protectedNote->title << "(HINT: password123 for Personal Diary, if you want to access other protected notes you've created please press ENTER): ";
                    string passwordInput;
                    getline(cin, passwordInput);

                    if (passwordInput == protectedNote->password) {
                        cout << "\nAccess granted to: " << protectedNote->title << "\n";
                        cout << protectedNote->getDetails() << endl << endl;
                        accessGranted = true;
                        break;
                    }
                    else {
                        cout << "Incorrect password. Try again for this note.\n";
                    }
                }
            }

            if (!accessGranted) {
                cout << "\nNo access granted to any protected notes with given passwords.\n";
            }
            break;
        }
        case 4:
            cout << "\tUnprotected Notes Details:\n\n";
            for (auto& note : notes) {
                if (PublicNote* publicNote = dynamic_cast<PublicNote*>(note)) {
                    cout << publicNote->getDetails() << endl << endl;
                }
            }
            break;
        case 5:
        {
            string searchText;
            cout << "\nEnter search text: ";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            getline(cin, searchText);
            searchNotesFullText(notes, searchText);
        }
        break;
        case 6:
        {
            string tag;
            cout << "\nEnter tag to search: ";
            cin >> tag;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear any excess input
            searchNotesByTag(notes, tag);
        }
        break;
        case 7:
            return; // Exit the loop and return to the main menu
        default:
            cout << "Invalid choice, please choose again.\n";
        }
    } while (noteChoice != 7); // Keep looping until 'Go Back' is selected
}

void addTask(vector<Item*>& items) {
    string title, description, deadline, interval;
    int priority, type;
    cout << "Enter task type (1 for One-Time, 2 for Recurring, 3 for Generic): ";
    if (!(cin >> type)) {
        cin.clear(); // Clear error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore incorrect input
        cout << "Invalid input. Please enter a valid number. Press ENTER to continue." << endl;
        return; // Exit the function on invalid input
    }
    cin.ignore(); // Clear the newline left by cin >>

    cout << "Enter task title: ";
    getline(cin, title);
    cout << "Enter description: ";
    getline(cin, description);
    cout << "Enter deadline (YYYY-MM-DD or No Deadline): ";
    getline(cin, deadline);
    cout << "Enter priority (1-10): ";
    if (!(cin >> priority)) {
        cin.clear(); // Clear error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore incorrect input
        cout << "Invalid input. Please enter a valid number. Press ENTER to continue." << endl;
        return; // Exit the function on invalid input
    }
    cin.ignore(); // Clear the newline left by cin >>

    if (type == 2) { // Recurring Task
        cout << "Enter recurrence interval (e.g., weekly, monthly): ";
        getline(cin, interval);
        RecurringTask* newTask = new RecurringTask(title, description, deadline, priority, interval);
        items.push_back(newTask);
        cout << "Recurring Task added successfully! Press ENTER to continue!\n";
    }
    else if (type == 1) { // One-Time Task
        OneTimeTask* newTask = new OneTimeTask(title, description, deadline, priority);
        items.push_back(newTask);
        cout << "One-Time Task added successfully! Press ENTER to continue!\n";
    }
    else { // Generic Task
        Task* newTask = new Task(title, description, deadline, priority);
        items.push_back(newTask);
        cout << "Generic Task added successfully! Press ENTER to continue!\n";
    }
}





void addGoal(vector<Item*>& items) {
    string title, description;
    double progress = 0.0; // Initialize progress with a default value
    int type;
    cout << "Enter goal type (1 for Quantifiable, 2 for Non-Quantifiable, 3 for Generic): ";
    cin >> type;
    cin.ignore(); // Consume the remaining newline character

    cout << "Enter goal title: ";
    getline(cin, title);
    cout << "Enter description: ";
    getline(cin, description);

    if (type == 1) { // Quantifiable Goal
        cout << "Enter progress (0.0 - 1.0): ";
        cin >> progress;
        QuantifiableGoal* newGoal = new QuantifiableGoal(title, description, progress);
        items.push_back(newGoal);
        cout << "Quantifiable Goal added successfully!" << endl;
    }
    else if (type == 2) { // Non-Quantifiable Goal
        NonQuantifiableGoal* newGoal = new NonQuantifiableGoal(title, description, progress); // Default progress as 0
        items.push_back(newGoal);
        cout << "Non-Quantifiable Goal added successfully! Press ENTER to continue!" << endl;
    }
    else { // Generic Goal
        cout << "Enter progress (0.0 - 1.0, enter 0 if progress does not apply): ";
        cin >> progress;
        Goal* newGoal = new Goal(title, description, progress);
        items.push_back(newGoal);
        cout << "Generic Goal added successfully!" << endl;
    }
}




void addNote(vector<Item*>& items) {
    string title, description, tagsInput, password;
    vector<string> tags;
    int type;
    cout << "Enter note type (1 for Public, 2 for Protected, 3 for Generic): ";
    cin >> type;
    cin.ignore(); // Important to consume the newline character after cin

    cout << "Enter note title: ";
    getline(cin, title);
    cout << "Enter description: ";
    getline(cin, description);
    cout << "Enter tags (comma-separated): ";
    getline(cin, tagsInput);

    stringstream ss(tagsInput);
    string tag;
    while (getline(ss, tag, ',')) {
        tag = tag.empty() ? "generic" : tag; // Ensure no empty tags; use 'generic' as a placeholder if empty
        tags.push_back(tag);
    }
    if (type == 2) { // Protected Note
        cout << "Enter password for protected note: ";
        getline(cin, password);
        ProtectedNote* newNote = new ProtectedNote(title, description, tags, password);
        items.push_back(newNote);
        cout << "Protected Note added successfully! Press ENTER to continue!" << endl;
    }
    else if (type == 1) { // Public Note
        PublicNote* newNote = new PublicNote(title, description, tags);
        items.push_back(newNote);
        cout << "Public Note added successfully! Press ENTER to continue!" << endl;
    }
    else if (type == 3) { // Generic Note
        Note* newNote = new Note(title, description, tags); // Generic notes can use the base class Note
        items.push_back(newNote);
        cout << "Generic Note added successfully! Press ENTER to continue!" << endl;
    }
}



// Main function
int main() {
    vector<Item*> items;
    loadDataFromFile("data.txt", items); // Load existing data

    int choice;
    do {
        cout << "-----------------------------------------\n";
        cout << "\tWelcome to GTN Manager!\n\n";
        cout << "1. Display All Items\n";
        cout << "2. Tasks\n";
        cout << "3. Goals\n";
        cout << "4. Notes\n";
        cout << "5. Add New Task\n";
        cout << "6. Add New Goal\n";
        cout << "7. Add New Note\n";
        cout << "8. Exit\n";
        cout << "-----------------------------------------\n";

        if (!(cin >> choice)) {
            cin.clear(); // Clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the input
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1:
            displayAllItems(items);
            break;
        case 2:
            handleTasks(items);
            break;
        case 3:
            handleGoals(items);
            break;
        case 4:
            handleNotes(items);
            break;
        case 5:
            addTask(items);
            break;
        case 6:
            addGoal(items);
            break;
        case 7:
            addNote(items);
            break;
        case 8:
            cout << "Exiting program..." << endl;
            break;
        default:
            cout << "Invalid choice, please choose again." << endl;
        }

        // This clears any extra input that the user might have entered beyond the first number.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

    } while (choice != 8);

    // Cleanup memory and save data
    for (auto& item : items) {
        delete item;
    }

    return 0;
}