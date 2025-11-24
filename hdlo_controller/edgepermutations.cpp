#include "edgepermutations.h"

// Initialize static members
EdgePermutation* EdgePermutation::permutationRegistry[MAX_PERMUTATIONS] = {nullptr};
String EdgePermutation::permutationNameRegistry[MAX_PERMUTATIONS] = {""};
int EdgePermutation::numRegisteredPermutations = 0;

// Legacy arrays for backward compatibility - point to registry
EdgePermutation** edgePermutationArray = EdgePermutation::getPermutationRegistry();
String* edgePermutationNames = EdgePermutation::getPermutationNameRegistry();
int numEdgePermutations = 0; // Updated as permutations register

EdgePermutation::EdgePermutation() 
    : name("identity"), shouldRegister(false) {
    // Initialize as identity permutation
    for(int i = 0; i < 120; i++) {
        permutation[i] = i;
    }
}

EdgePermutation::EdgePermutation(String inputName, std::array<int, 120> inputPermutation, bool registerPerm)
    : name(inputName), permutation(inputPermutation), shouldRegister(registerPerm) {
    if(shouldRegister) {
        registerSelf();
    }
}

void EdgePermutation::registerSelf() {
    if(numRegisteredPermutations >= MAX_PERMUTATIONS) {
        Serial.println("ERROR: Permutation registry full! Cannot register: " + name);
        return;
    }
    
    // Check if name already exists (avoid duplicates)
    for(int i = 0; i < numRegisteredPermutations; i++) {
        if(permutationNameRegistry[i].equals(name)) {
            Serial.println("Warning: Permutation name '" + name + "' already registered. Skipping duplicate.");
            return;
        }
    }
    
    permutationRegistry[numRegisteredPermutations] = this;
    permutationNameRegistry[numRegisteredPermutations] = name;
    numRegisteredPermutations++;
    numEdgePermutations = numRegisteredPermutations; // Update legacy variable
    
    Serial.println("Registered permutation: " + name + " (total: " + String(numRegisteredPermutations) + ")");
}

EdgePermutation* EdgePermutation::findPermutationByName(String name) {
    for(int i = 0; i < numRegisteredPermutations; i++) {
        if(permutationNameRegistry[i].equalsIgnoreCase(name)) {
            return permutationRegistry[i];
        }
    }
    return nullptr;
}

void EdgePermutation::printRegistry() {
    Serial.println("=== Edge Permutation Registry ===");
    Serial.println("Total permutations: " + String(numRegisteredPermutations));
    for(int i = 0; i < numRegisteredPermutations; i++) {
        Serial.println("  [" + String(i) + "] " + permutationNameRegistry[i]);
    }
    Serial.println("=================================");
}

EdgePermutation EdgePermutation::compose(const EdgePermutation& other, String newName, bool registerResult) const {
    std::array<int, 120> composedPerm;
    
    // Composition: first apply this permutation, then apply other
    // result[i] = this[other[abs(i)]] with sign handling
    for(int i = 0; i < 120; i++) {
        int otherValue = other.permutation[i];
        int otherIndex = abs(otherValue);
        bool otherNegative = (otherValue < 0);
        
        int thisValue = this->permutation[otherIndex];
        int thisIndex = abs(thisValue);
        bool thisNegative = (thisValue < 0);
        
        // If either is negative (but not both), result is negative
        bool resultNegative = (otherNegative != thisNegative);
        
        composedPerm[i] = resultNegative ? -thisIndex : thisIndex;
    }
    
    return EdgePermutation(newName, composedPerm, registerResult);
}

// FIXED: Compose by name now returns by value to avoid memory leak
EdgePermutation EdgePermutation::composeByName(const String* permNames, int numPerms, String newName, bool registerResult) {
    if(numPerms <= 0) {
        Serial.println("Error: No permutations to compose");
        // Return identity permutation as error case
        std::array<int, 120> identity;
        for(int i = 0; i < 120; i++) identity[i] = i;
        return EdgePermutation("error", identity, false);
    }
    
    // Find first permutation
    EdgePermutation* firstPerm = findPermutationByName(permNames[0]);
    if(firstPerm == nullptr) {
        Serial.println("Error: Permutation '" + permNames[0] + "' not found");
        std::array<int, 120> identity;
        for(int i = 0; i < 120; i++) identity[i] = i;
        return EdgePermutation("error", identity, false);
    }
    
    Serial.println("Starting composition with: " + permNames[0]);
    
    // Start with a copy of the first permutation (by value, no allocation)
    EdgePermutation result(firstPerm->name, firstPerm->permutation, false);
    
    // Compose with remaining permutations
    for(int p = 1; p < numPerms; p++) {
        EdgePermutation* nextPerm = findPermutationByName(permNames[p]);
        
        if(nextPerm == nullptr) {
            Serial.println("Warning: Permutation '" + permNames[p] + "' not found, skipping");
            continue;
        }
        
        Serial.println("Composing with: " + permNames[p]);
        
        // Compose (returns by value, no allocation)
        String tempName = result.getName() + "_" + permNames[p];
        result = result.compose(*nextPerm, tempName, false);
    }
    
    // Set final name and register if requested
    result.name = newName;
    if(registerResult) {
        result.shouldRegister = true;
        result.registerSelf();
    }
    
    return result;
}
