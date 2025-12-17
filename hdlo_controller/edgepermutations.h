#ifndef EDGEPERMUTATIONS_H
#define EDGEPERMUTATIONS_H

#include <Arduino.h>
#include <array>
#include "globalids.h"

const bool PRINTPERMUTATIONS = false;

class EdgePermutation {
private:
    int permId;
    std::array<int, 120> permutation;
    bool shouldRegister;
    
    static const int MAX_PERMUTATIONS = 70;
    static EdgePermutation* permutationRegistry[MAX_PERMUTATIONS];
    static int permutationIdRegistry[MAX_PERMUTATIONS];
    static int numRegisteredPermutations;

public:
    EdgePermutation();
    EdgePermutation(int inputId, std::array<int, 120> inputPermutation, bool registerPerm = true);
    
    void registerSelf();
    
    int getId() const { return permId; }
    int getPermutation(int index) const { 
        if(index >= 0 && index < 120) {
            return permutation[index];
        }
        return index;
    }
    
    const std::array<int, 120>& getPermutationArray() const { 
        return permutation; 
    }
    
    EdgePermutation compose(const EdgePermutation& other, int newId, bool registerResult = true) const;
    
    static EdgePermutation composeById(const int* permIds, int numPerms, int newId, bool registerResult = true);
    
    static EdgePermutation** getPermutationRegistry() { return permutationRegistry; }
    static int* getPermutationIdRegistry() { return permutationIdRegistry; }
    static int getNumRegisteredPermutations() { return numRegisteredPermutations; }
    static EdgePermutation* findPermutationById(int id);
    static void printRegistry();
};

extern EdgePermutation** edgePermutationArray;
extern int* edgePermutationIds;
extern int numEdgePermutations;

#endif // EDGEPERMUTATIONS_H
