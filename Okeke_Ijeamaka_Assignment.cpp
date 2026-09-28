#include <iostream>
#include <string>
#include <stack>
#include <queue>    
#include <deque>    
#include <limits>    // Required for numeric_limits               //AI was used here




using namespace std;




struct Parcel {
    int parcelID;
    string sender;
    string recipient;
    string address;
    double weight;
    int priority; 

    Parcel* next; // Pointer for the linked list
};

// Undo & Redo 
enum ActionType { ADD, UPDATE, DELETE };
struct Action {
    ActionType type;
    Parcel data; 
};

//Globals for heads of linklists and stacks
Parcel* pendingParcelsHead = nullptr;
Parcel* deliveredParcelsHead = nullptr;
stack<Action> undoStack;
stack<Action> redoStack;

int nextParcelID = 1;



void displayMenu();
void handleUserInput();

// Parcel Management
void registerParcel();
void updateParcel();
void deleteParcel();
Parcel* findParcelByID(int id, Parcel* head);
void displayList(Parcel* head, const string& title);

//Delivery and Load truck
void dispatchParcels();
void trackDeliveredParcel(int parcelID);

//Undo & Redo Functionality
void undoAction();
void redoAction();
void recordActionForUndo(ActionType type, const Parcel& data);
void clearRedoStack();

//Summary & Reports
void generateSummaryReports();
int getListCount(Parcel* head);
void countPendingByPriority();
double calculateAverageWeight();

//Invalid input Error handling
void clearInputBuffer();







int main() {
    handleUserInput();
    return 0;
}

//Filling Empty Function with values
void displayMenu() {
    cout << "\n===== Jumia Logistics Parcel Management System ===\n";
    cout << "1. Register New Parcel\n";
    cout << "2. Update Existing Parcel\n";
    cout << "3. DELETE a Parcel\n";
    cout << "4. Deliver Parcels (Load Truck)\n";
    cout << "5. Undo Last Action\n";
    cout << "6. Redo Last Action\n";
    cout << "7. View Pending Parcels\n";
    cout << "8. View Delivered Parcels\n";
    cout << "9. Generate Summary Reports\n";
    cout << "0. Exit\n";
    cout << "====================================\n";
    cout << "Enter your choice: ";
}

void handleUserInput() {
    int choice;
    do {
        displayMenu();
        cin >> choice;

        // Input validation
        if (cin.fail()) {
            clearInputBuffer();
            choice = -1; 
        }

        switch (choice) {
            case 1: registerParcel(); break;
            case 2: updateParcel(); break;
            case 3: deleteParcel(); break;
            case 4: dispatchParcels(); break;
            case 5: undoAction(); break;
            case 6: redoAction(); break;
            case 7: displayList(pendingParcelsHead, "Pending Parcels"); break;
            case 8: displayList(deliveredParcelsHead, "Delivered Parcels History"); break;
            case 9: generateSummaryReports(); break;
            case 0: cout << "Exiting application. Goodbye! \n Thank you for Using Jumia Parcel Delivery Service! \n"; break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    } while (choice != 0);
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

//Parcel Management
void registerParcel() {
    Parcel* newParcel = new Parcel();
    newParcel->parcelID = nextParcelID++;

    cout << "Enter Sender Name: ";
    cin.ignore();
    getline(cin, newParcel->sender);

    cout << "Enter Recipient Name: ";
    getline(cin, newParcel->recipient);

    cout << "Enter Delivery Address: ";
    getline(cin, newParcel->address);

    cout << "Enter Parcel Weight (kg): ";
    cin >> newParcel->weight;

    cout << "Enter Priority (1-High, 2-Medium, 3-Low): ";
    cin >> newParcel->priority;

    
    newParcel->next = pendingParcelsHead;
    pendingParcelsHead = newParcel;

    cout << "Parcel registered successfully with ID: " << newParcel->parcelID << "\n";
    
    
    recordActionForUndo(ADD, *newParcel);             
}

void updateParcel() {
    cout << "Enter Parcel ID to update: ";
    int id;
    cin >> id;

    Parcel* parcelToUpdate = findParcelByID(id, pendingParcelsHead);
    if (!parcelToUpdate) {
        cout << "Parcel ID not found in pending list.\n";
        return;
    }

   
    recordActionForUndo(UPDATE, *parcelToUpdate);

    cout << "Enter new Priority (1-High, 2-Medium, 3-Low): ";
    cin >> parcelToUpdate->priority;
    cout << "Enter new Weight (kg): ";
    cin >> parcelToUpdate->weight;

    cout << "Parcel " << id << " updated successfully.\n";
}

void deleteParcel() {
    cout << "Enter Parcel ID to delete: ";
    int id;
    cin >> id;

    // Finding the parcel and its previous node
    Parcel* temp = pendingParcelsHead;
    Parcel* prev = nullptr;
    while (temp != nullptr && temp->parcelID != id) {
        prev = temp;
        temp = temp->next;                                               //AI was used here
    }

    if (temp == nullptr) {
        cout << "Parcel ID not found in pending list.\n";
        return;
    }
    
   
    recordActionForUndo(DELETE, *temp);

    // Unlinking from the pending list
    if (prev == nullptr) { // It's the head node
        pendingParcelsHead = temp->next;
    } else {
        prev->next = temp->next;
    }

    delete temp;

    cout << "Parcel " << id << " has been deleted.\n";                //AI was used here
}

Parcel* findParcelByID(int id, Parcel* head) {
    Parcel* current = head;
    while (current != nullptr) {
        if (current->parcelID == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr; 
}

void displayList(Parcel* head, const string& title) {
    cout << "\n--- " << title << " ---\n";
    if (!head) {
        cout << "The list is empty.\n";
        return;
    }
    Parcel* current = head;
    while (current != nullptr) {
        cout << "ID: " << current->parcelID
                  << ", To: " << current->recipient
                  << ", Address: " << current->address
                  << ", Weight: " << current->weight
                  << "kg, Priority: " << current->priority << "\n";
        current = current->next;
    }
    cout << "------------------------------------\n";
}


//Delivery & Loading truck


struct CompareParcel {
    bool operator()(const Parcel* a, const Parcel* b) {
    	
        
        return a->priority > b->priority;
    }
};

void dispatchParcels() {
    if (!pendingParcelsHead) {
        cout << "No parcels pending for dispatch.\n";
        return;
    }
    
    
    
    

    
    priority_queue<Parcel*, deque<Parcel*>, CompareParcel> truckQueue;
    
    Parcel* current = pendingParcelsHead;
    while(current) {
        truckQueue.push(current);
        current = current->next;
    }                                                                     //AI was used here

    cout << "How many parcels to load onto the truck? (Max: " << truckQueue.size() << "): ";
    size_t count;
    cin >> count;

    if (count > truckQueue.size()) {
        count = truckQueue.size();
    }

    cout << "\n--- Loading Truck in Priority Order ---\n";
    for (size_t i = 0; i < count; ++i) {
        Parcel* toDispatch = truckQueue.top();
        truckQueue.pop();

        cout << "Loading Parcel ID: " << toDispatch->parcelID 
                  << " (Priority: " << toDispatch->priority << ")\n";

        
		
		
		// Moving the parcel from pending to delivered list
        trackDeliveredParcel(toDispatch->parcelID);
    }
    cout << "--- Truck Loaded Successfully ---\n";
}

void trackDeliveredParcel(int parcelID) {

    Parcel* temp = pendingParcelsHead;
    Parcel* prev = nullptr;
    while (temp != nullptr && temp->parcelID != parcelID) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr) return; 

    if (prev == nullptr) {
        pendingParcelsHead = temp->next;
    } else {
        prev->next = temp->next;
    }

    
    temp->next = deliveredParcelsHead;
    deliveredParcelsHead = temp;
}


//Undo & Redo Functionality
void recordActionForUndo(ActionType type, const Parcel& data) {
    Action action;
    action.type = type;
    action.data = data; 
    action.data.next = nullptr;
    undoStack.push(action);
    clearRedoStack(); 
}

void clearRedoStack() {
    while (!redoStack.empty()) {
        redoStack.pop();
    }
}

void undoAction() {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    Action lastAction = undoStack.top();
    undoStack.pop();

    switch (lastAction.type) {
        case ADD: {
            
            Parcel* toDelete = pendingParcelsHead;
            pendingParcelsHead = pendingParcelsHead->next;
            delete toDelete;
            cout << "Undo: Removed recently added parcel " << lastAction.data.parcelID << ".\n";
            break;
        }
        case UPDATE: {
            
            Parcel* toRestore = findParcelByID(lastAction.data.parcelID, pendingParcelsHead);
            if (toRestore) {
                
                Parcel tempData = *toRestore;
                *toRestore = lastAction.data;
                lastAction.data = tempData;
                cout << "Undo: Reverted updates for parcel " << toRestore->parcelID << ".\n";
            }
            break;
        }
        case DELETE: {
            
            Parcel* restoredParcel = new Parcel(lastAction.data);
            restoredParcel->next = pendingParcelsHead;
            pendingParcelsHead = restoredParcel;
            cout << "Undo: Restored DELETEed parcel " << restoredParcel->parcelID << ".\n";
            break;
        }
    }
    redoStack.push(lastAction);
}

void redoAction() {
    if (redoStack.empty()) {
        cout << "Nothing to redo.\n";
        return;
    }

    Action actionToRedo = redoStack.top();
    redoStack.pop();

    switch (actionToRedo.type) {
        case ADD: {
            
            Parcel* restoredParcel = new Parcel(actionToRedo.data);
            restoredParcel->next = pendingParcelsHead;
            pendingParcelsHead = restoredParcel;
            cout << "Redo: Re-registered parcel " << restoredParcel->parcelID << ".\n";                     //AI was used here
            break;
        }
        case UPDATE: {
            
            Parcel* toUpdate = findParcelByID(actionToRedo.data.parcelID, pendingParcelsHead);
             if (toUpdate) {
               
                Parcel tempData = *toUpdate;
                *toUpdate = actionToRedo.data;
                actionToRedo.data = tempData;
                cout << "Redo: Re-applied updates for parcel " << toUpdate->parcelID << ".\n";                //AI was used here
            }
            break;
        }
        case DELETE: {
           
            Parcel* temp = pendingParcelsHead;
            Parcel* prev = nullptr;
            while (temp != nullptr && temp->parcelID != actionToRedo.data.parcelID) {
                prev = temp;
                temp = temp->next;
            }
            if (temp != nullptr) {
                if (prev == nullptr) pendingParcelsHead = temp->next;
                else prev->next = temp->next;
                delete temp;
                cout << "Redo: DELETEed parcel " << actionToRedo.data.parcelID << " again.\n";
            }
            break;
        }
    }
    undoStack.push(actionToRedo);
}


//Summary & Reports
void generateSummaryReports() {
    int pendingCount = getListCount(pendingParcelsHead);
    int deliveredCount = getListCount(deliveredParcelsHead);
    int totalRegistered = pendingCount + deliveredCount;

    cout << "\n========== SUMMARY REPORTS ==========\n";
    cout << "Total Parcels Delivered: " << deliveredCount << "\n";
    cout << "Total Parcels Pending: " << pendingCount << "\n";
    cout << "Total Parcels Registered in System: " << totalRegistered << "\n";
    
    if (pendingCount > 0) {
       countPendingByPriority();
    }
    
    if (totalRegistered > 0) {
        cout << "Average Parcel Weight: " << calculateAverageWeight() << " kg\n";
    }
    
    displayList(deliveredParcelsHead, "Delivery History Report");
    cout << "===============================\n";
}

int getListCount(Parcel* head) {
    int count = 0;
    Parcel* current = head;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

void countPendingByPriority() {
    int high = 0, medium = 0, low = 0;
    Parcel* current = pendingParcelsHead;
    while (current != nullptr) {
        if (current->priority == 1) high++;
        else if (current->priority == 2) medium++;
        else low++;
        current = current->next;
    }
    cout << "\n--- Pending Parcels by Priority ---\n";
    cout << "High Priority (1): " << high << "\n";
    cout << "Medium Priority (2): " << medium << "\n";
    cout << "Low Priority (3): " << low << "\n";
}

double calculateAverageWeight() {
    double totalWeight = 0;
    int totalCount = 0;
    
    Parcel* current = pendingParcelsHead;
    while (current) {
        totalWeight += current->weight;
        totalCount++;
        current = current->next;
    }

    current = deliveredParcelsHead;
    while (current) {
        totalWeight += current->weight;
        totalCount++;
        current = current->next;
    }

    return (totalCount == 0) ? 0 : totalWeight / totalCount;
}
