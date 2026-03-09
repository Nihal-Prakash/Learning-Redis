#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

using namespace std;

class FixedSizeIntrusiveHashTable {
private:
    struct HashtableNode {
        HashtableNode* nextNodeInChain = nullptr;
        uint64_t hashCode = 0;
    };

    struct KeyValueEntry {
        HashtableNode hashTableNode;
        string keyString;
        string valueString;

        KeyValueEntry(string key, string value) {
            keyString = key;
            valueString = value;
            hashTableNode.hashCode = computeStringHash(key);
        }
    };

    vector<HashtableNode*> slotArray;
    size_t slotIndexMask = 0;
    size_t numberOfStoredItems = 0;

private:
    static uint64_t computeStringHash(const string& text) {
        uint64_t hashValue = 1469598103934665603ULL;

        for (unsigned char character : text) {
            hashValue ^= character;
            hashValue *= 1099511628211ULL;
        }

        return hashValue;
    }

    static KeyValueEntry* getEntryFromNode(HashtableNode* pointerToNode) {
        return (KeyValueEntry*)((char*)pointerToNode - offsetof(KeyValueEntry, hashTableNode));
    }

    size_t getSlotIndexForHash(uint64_t hashCode) {
        return hashCode & slotIndexMask;
    }

    HashtableNode** findPointerThatPointsToMatchingNode(const string& keyString) {
        uint64_t targetHashCode = computeStringHash(keyString);
        size_t slotIndex = getSlotIndexForHash(targetHashCode);

        HashtableNode** pointerThatPointsToCurrentNode = &slotArray[slotIndex];

        while (*pointerThatPointsToCurrentNode != nullptr) {
            HashtableNode* currentNode = *pointerThatPointsToCurrentNode;
            KeyValueEntry* currentEntry = getEntryFromNode(currentNode);

            if (currentNode->hashCode == targetHashCode &&
                currentEntry->keyString == keyString) {
                return pointerThatPointsToCurrentNode;
            }

            pointerThatPointsToCurrentNode = &currentNode->nextNodeInChain;
        }

        return nullptr;
    }

    void insertNodeIntoFrontOfChain(HashtableNode* nodeToInsert) {
        size_t slotIndex = getSlotIndexForHash(nodeToInsert->hashCode);
        nodeToInsert->nextNodeInChain = slotArray[slotIndex];
        slotArray[slotIndex] = nodeToInsert;
        numberOfStoredItems++;
    }

public:
    FixedSizeIntrusiveHashTable(size_t numberOfSlots = 8) {
        slotArray.assign(numberOfSlots, nullptr);
        slotIndexMask = numberOfSlots - 1;
    }

    void setKeyValue(const string& keyString, const string& valueString) {
        HashtableNode** pointerThatPointsToExistingNode =
            findPointerThatPointsToMatchingNode(keyString);

        if (pointerThatPointsToExistingNode != nullptr) {
            KeyValueEntry* existingEntry =
                getEntryFromNode(*pointerThatPointsToExistingNode);
            existingEntry->valueString = valueString;
            return;
        }

        KeyValueEntry* newEntry = new KeyValueEntry(keyString, valueString);
        insertNodeIntoFrontOfChain(&newEntry->hashTableNode);
    }

    string* getValueForKey(const string& keyString) {
        HashtableNode** pointerThatPointsToMatchingNode =
            findPointerThatPointsToMatchingNode(keyString);

        if (pointerThatPointsToMatchingNode == nullptr) {
            return nullptr;
        }

        KeyValueEntry* matchingEntry =
            getEntryFromNode(*pointerThatPointsToMatchingNode);

        return &matchingEntry->valueString;
    }

    bool containsKey(const string& keyString) {
        return findPointerThatPointsToMatchingNode(keyString) != nullptr;
    }

    bool deleteKeyValue(const string& keyString) {
        HashtableNode** pointerThatPointsToMatchingNode =
            findPointerThatPointsToMatchingNode(keyString);

        if (pointerThatPointsToMatchingNode == nullptr) {
            return false;
        }

        HashtableNode* targetNode = *pointerThatPointsToMatchingNode;
        *pointerThatPointsToMatchingNode = targetNode->nextNodeInChain;

        delete getEntryFromNode(targetNode);
        numberOfStoredItems--;

        return true;
    }

    size_t size() {
        return numberOfStoredItems;
    }

    void clear() {
        for (size_t slotIndex = 0; slotIndex < slotArray.size(); slotIndex++) {
            HashtableNode* currentNode = slotArray[slotIndex];

            while (currentNode != nullptr) {
                HashtableNode* nextNode = currentNode->nextNodeInChain;
                delete getEntryFromNode(currentNode);
                currentNode = nextNode;
            }

            slotArray[slotIndex] = nullptr;
        }

        numberOfStoredItems = 0;
    }

    void printInternalStructure() {
        cout << "\n--- Hashtable Internal Structure ---\n";

        for (size_t slotIndex = 0; slotIndex < slotArray.size(); slotIndex++) {
            cout << "slot[" << slotIndex << "]: ";

            HashtableNode* currentNode = slotArray[slotIndex];

            while (currentNode != nullptr) {
                KeyValueEntry* currentEntry = getEntryFromNode(currentNode);
                cout << "(" << currentEntry->keyString
                     << " -> "
                     << currentEntry->valueString
                     << ") -> ";
                currentNode = currentNode->nextNodeInChain;
            }

            cout << "NULL\n";
        }
    }
};

int main() {
    FixedSizeIntrusiveHashTable studentDatabaseTable(8);

    studentDatabaseTable.setKeyValue("student_name", "Rahul");
    studentDatabaseTable.setKeyValue("branch_name", "CSE");
    studentDatabaseTable.setKeyValue("college_name", "IIIT");

    string* valueForStudentName =
        studentDatabaseTable.getValueForKey("student_name");
    if (valueForStudentName != nullptr) {
        cout << "student_name = " << *valueForStudentName << '\n';
    }

    string* valueForBranchName =
        studentDatabaseTable.getValueForKey("branch_name");
    if (valueForBranchName != nullptr) {
        cout << "branch_name = " << *valueForBranchName << '\n';
    }

    studentDatabaseTable.setKeyValue("branch_name", "Computer Science");

    string* updatedValueForBranchName =
        studentDatabaseTable.getValueForKey("branch_name");
    if (updatedValueForBranchName != nullptr) {
        cout << "branch_name after update = "
             << *updatedValueForBranchName << '\n';
    }

    cout << "contains college_name? "
         << (studentDatabaseTable.containsKey("college_name") ? "yes" : "no")
         << '\n';

    studentDatabaseTable.deleteKeyValue("college_name");

    cout << "contains college_name after delete? "
         << (studentDatabaseTable.containsKey("college_name") ? "yes" : "no")
         << '\n';

    cout << "current size = " << studentDatabaseTable.size() << '\n';

    studentDatabaseTable.printInternalStructure();
    studentDatabaseTable.clear();

    return 0;
}
