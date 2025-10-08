#ifndef EDGEPERMUTATIONS_H
#define EDGEPERMUTATIONS_H

#include <Arduino.h>
#include <array>

// Edge permutation class
class EdgePermutation {
private:
    String name;
    std::array<int, 120> permutation;

public:
    EdgePermutation();
    EdgePermutation(String inputName, std::array<int, 120> inputPermutation);
    
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
};

// Registry of edge permutations
extern const int numEdgePermutations;
extern EdgePermutation edgePermutationArray[];
extern String edgePermutationNames[];

#endif // EDGEPERMUTATIONS_H