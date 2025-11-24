#ifndef EDGEPERMUTATIONS_H
#define EDGEPERMUTATIONS_H

#include <Arduino.h>
#include <array>

const bool PRINTPERMUTATIONS = false;

// Edge permutation class
class EdgePermutation {
private:
    String name;
    std::array<int, 120> permutation;
    bool shouldRegister;
    
    // Global permutation registry
    static const int MAX_PERMUTATIONS = 70;
    static EdgePermutation* permutationRegistry[MAX_PERMUTATIONS];
    static String permutationNameRegistry[MAX_PERMUTATIONS];
    static int numRegisteredPermutations;

public:
    EdgePermutation();
    EdgePermutation(String inputName, std::array<int, 120> inputPermutation, bool registerPerm = true);
    
    // Helper for registration
    void registerSelf();
    
    String getName() const { return name; }
    int getPermutation(int index) const { 
        if(index >= 0 && index < 120) {
            return permutation[index];
        }
        return index; // Return identity if out of bounds
    }
    
    const std::array<int, 120>& getPermutationArray() const { 
        return permutation; 
    }
    
    // Compose this permutation with another
    // Result = apply this permutation, then apply other
    EdgePermutation compose(const EdgePermutation& other, String newName, bool registerResult = true) const;
    
    // Static method to compose multiple permutations by name
    // Applies permutations left-to-right: name1, then name2, then name3, etc.
    // FIXED: Returns by value to avoid memory leak
    static EdgePermutation composeByName(const String* permNames, int numPerms, String newName, bool registerResult = true);
    
    // Static methods to access the global registry
    static EdgePermutation** getPermutationRegistry() { return permutationRegistry; }
    static String* getPermutationNameRegistry() { return permutationNameRegistry; }
    static int getNumRegisteredPermutations() { return numRegisteredPermutations; }
    static EdgePermutation* findPermutationByName(String name);
    static void printRegistry();
};

// Legacy arrays for backward compatibility - now point to registry
extern EdgePermutation** edgePermutationArray;
extern String* edgePermutationNames;
extern int numEdgePermutations;

#endif // EDGEPERMUTATIONS_H
