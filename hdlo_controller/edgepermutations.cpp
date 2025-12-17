#include "edgepermutations.h"

// Static member initialization
EdgePermutation* EdgePermutation::permutationRegistry[MAX_PERMUTATIONS];
int EdgePermutation::permutationIdRegistry[MAX_PERMUTATIONS];
int EdgePermutation::numRegisteredPermutations = 0;

// Legacy extern pointers
EdgePermutation** edgePermutationArray = EdgePermutation::getPermutationRegistry();
int* edgePermutationIds = EdgePermutation::getPermutationIdRegistry();
int numEdgePermutations = 0;

EdgePermutation::EdgePermutation() : permId(0), shouldRegister(false) {
    for(int i = 0; i < 120; i++) {
        permutation[i] = i;
    }
}

EdgePermutation::EdgePermutation(int inputId, std::array<int, 120> inputPermutation, bool registerPerm)
    : permId(inputId), permutation(inputPermutation), shouldRegister(registerPerm) {
    if(shouldRegister) {
        registerSelf();
    }
}

void EdgePermutation::registerSelf() {
    if(numRegisteredPermutations >= MAX_PERMUTATIONS) {
        Serial.println("Error: Permutation registry full");
        return;
    }
    
    for(int i = 0; i < numRegisteredPermutations; i++) {
        if(permutationIdRegistry[i] == permId) {
            if(PRINTPERMUTATIONS) {
                Serial.print("Warning: Permutation ID ");
                Serial.print(permId);
                Serial.println(" already registered");
            }
            return;
        }
    }
    
    permutationRegistry[numRegisteredPermutations] = this;
    permutationIdRegistry[numRegisteredPermutations] = permId;
    numRegisteredPermutations++;
    numEdgePermutations = numRegisteredPermutations;
    
    if(PRINTPERMUTATIONS) {
        Serial.print("Registered permutation ID: ");
        Serial.println(permId);
    }
}

EdgePermutation EdgePermutation::compose(const EdgePermutation& other, int newId, bool registerResult) const {
    std::array<int, 120> result;
    
    for(int i = 0; i < 120; i++) {
        int firstMapping = this->permutation[i];
        
        if(firstMapping < 0) {
            int absIndex = abs(firstMapping);
            if(absIndex < 120) {
                int secondMapping = other.permutation[absIndex];
                result[i] = (secondMapping < 0) ? abs(secondMapping) : -secondMapping;
            } else {
                result[i] = firstMapping;
            }
        } else if(firstMapping < 120) {
            result[i] = other.permutation[firstMapping];
        } else {
            result[i] = firstMapping;
        }
    }
    
    return EdgePermutation(newId, result, registerResult);
}

EdgePermutation EdgePermutation::composeById(const int* permIds, int numPerms, int newId, bool registerResult) {
    if(numPerms <= 0) {
        std::array<int, 120> identity;
        for(int i = 0; i < 120; i++) identity[i] = i;
        return EdgePermutation(newId, identity, registerResult);
    }
    
    EdgePermutation* first = findPermutationById(permIds[0]);
    if(!first) {
        Serial.print("Error: Permutation ID ");
        Serial.print(permIds[0]);
        Serial.println(" not found");
        std::array<int, 120> identity;
        for(int i = 0; i < 120; i++) identity[i] = i;
        return EdgePermutation(newId, identity, registerResult);
    }
    
    EdgePermutation result = *first;
    
    for(int i = 1; i < numPerms; i++) {
        EdgePermutation* next = findPermutationById(permIds[i]);
        if(!next) {
            Serial.print("Error: Permutation ID ");
            Serial.print(permIds[i]);
            Serial.println(" not found");
            continue;
        }
        result = result.compose(*next, 0, false);
    }
    
    return EdgePermutation(newId, result.getPermutationArray(), registerResult);
}

EdgePermutation* EdgePermutation::findPermutationById(int id) {
    for(int i = 0; i < numRegisteredPermutations; i++) {
        if(permutationIdRegistry[i] == id) {
            return permutationRegistry[i];
        }
    }
    return nullptr;
}

void EdgePermutation::printRegistry() {
    Serial.println("\n=== Edge Permutation Registry ===");
    Serial.print("Registered: ");
    Serial.println(numRegisteredPermutations);
    for(int i = 0; i < numRegisteredPermutations; i++) {
        Serial.print("  [");
        Serial.print(i);
        Serial.print("] ID=");
        Serial.println(permutationIdRegistry[i]);
    }
    Serial.println("=================================\n");
}
