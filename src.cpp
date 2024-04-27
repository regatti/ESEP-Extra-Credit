#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>

class InMemoryDB {
private:
    std::unordered_map<std::string, int> mainState;
    std::unordered_map<std::string, int> tranState; // var for transaction state
    bool tranInProgress;

public:
    InMemoryDB() : tranInProgress(false) {} // transaction in progress

    int get(const std::string& key) {
        if (!tranInProgress) {
            return mainState[key]; // gets from mainstate if no transaction is happening
        } else {
            // checks if the key exists in tranState, if not return null
            for (const auto& entry : tranState) {
                if (entry.first == key) {
                    return entry.second;
                }
            }
            return 0; // null represented by 0
        }
    }

    void put(const std::string& key, int value) {
        if (!tranInProgress) {
            throw std::runtime_error("error: no active transaction. start a transaction before performing any operations.");
        } else {
            tranState[key] = value;
        }
    }

    void begin_transaction() {
        if (tranInProgress) {
            throw std::runtime_error("error: transaction already in progress. commit or rollback the current transaction before starting a new one.");
        }
        tranInProgress = true;

        // copy mainstate to tranState using loop
        for (const auto& entry : mainState) {
            tranState[entry.first] = entry.second;
        }
    }

    void commit() {
        if (!tranInProgress) {
            throw std::runtime_error("error: no active transaction. cannot commit without an active transaction.");
        }

        // copy tranState to mainState using loop
        for (const auto& entry : tranState) {
            mainState[entry.first] = entry.second;
        }

        tranInProgress = false;
    }

    void rollback() {
        if (!tranInProgress) {
            throw std::runtime_error("error: no active transaction. cannot rollback without an active transaction.");
        }
        tranInProgress = false;

        // clear tranState using loop
        tranState.clear();
    }
};

int main() {
    InMemoryDB inmemoryDB;

    // should return 0, because A doesn’t exist yet
    std::cout << inmemoryDB.get("A") << std::endl;

    // should throw an error because a transaction is not in progress
    // inmemoryDB.put("A", 5);

    // starts a new transaction
    inmemoryDB.begin_transaction();

    // sets value of A to 5, but its not committed yet
    inmemoryDB.put("A", 5);

    // should return 0 because updates to A are not committed yet
    std::cout << inmemoryDB.get("A") << std::endl;

    // update A’s value to 6 in the transaction
    inmemoryDB.put("A", 6);

    // commits the open transaction
    inmemoryDB.commit();

    // should return 6 since it was the last value of A to be committed
    std::cout << inmemoryDB.get("A") << std::endl;

    // throws an error, because there's no open transaction
    // inmemoryDB.commit();

    // throws an error because there's no ongoing transaction
    // inmemoryDB.rollback();

    // should return 0 because B does not exist in the database
    std::cout << inmemoryDB.get("B") << std::endl;

    // starts a new transaction
    inmemoryDB.begin_transaction();

    // set key B’s value to 10 within the transaction
    inmemoryDB.put("B", 10);

    // rollback the transaction and revert changes made to B
    inmemoryDB.rollback();

    // should return 0 because changes to B were rolled back
    std::cout << inmemoryDB.get("B") << std::endl;

    return 0;
}
