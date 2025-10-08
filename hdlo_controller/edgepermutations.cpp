#include "edgepermutations.h"

EdgePermutation::EdgePermutation() 
    : name("identity") {
    // Initialize as identity permutation
    for(int i = 0; i < 120; i++) {
        permutation[i] = i;
    }
}

EdgePermutation::EdgePermutation(String inputName, std::array<int, 120> inputPermutation)
    : name(inputName), permutation(inputPermutation) {}

  /*
 // Example usage in your main code or setup():

#include "edgepermutations.h"
#include "models.h"
#include "hdlo_models.h"

void setupExample() {
  // Initialize your models as usual
  initializemodels();
  
  // Example 1: Create a new model by applying a single permutation
  // Original model is unchanged, returns pointer to new model
  colormodel* rotatedModel = ourcolormodels[0]->applyEdgePermutation(
    edgePermutationArray[1], 
    "flowoctahedron_rotated"
  );
  
  // Example 2: Create a new model with reflection
  String singlePerm[] = {"reflect"};
  colormodel* reflectedModel = ourcolormodels[1]->applyEdgePermutationSequence(
    singlePerm, 
    1, 
    "octachain_reflected"
  );
  
  // Example 3: Create a new model with multiple permutations applied
  String multiplePerms[] = {"rotate30", "reflect", "cyclicshift12"};
  colormodel* complexModel = ourcolormodels[2]->applyEdgePermutationSequence(
    multiplePerms, 
    3, 
    "cycle_complex"
  );
  
  // Example 4: Chain permutations to create variants
  String flowPerms[] = {"invert", "rotate30"};
  colormodel* invertedFlow = ourcolormodels[0]->applyEdgePermutationSequence(
    flowPerms, 
    2, 
    "flowoctahedron_inverted_rotated"
  );
  
  // The original models are still intact!
  Serial.println("Original model: " + ourcolormodels[0]->getModelName());
  Serial.println("New model: " + rotatedModel->getModelName());
  
  // Remember to manage memory - delete models when done
  // delete rotatedModel;
  // delete reflectedModel;
  // etc.
}

// Helper function to create permuted variants of all models
void createPermutedVariants(String permName) {
  colormodel* variants[10];
  
  for(int i = 0; i < nummodels; i++) {
    String perms[] = {permName};
    String newName = ourcolormodels[i]->getModelName() + "_" + permName;
    variants[i] = ourcolormodels[i]->applyEdgePermutationSequence(perms, 1, newName);
    
    Serial.println("Created variant: " + variants[i]->getModelName());
  }
  
  // Now you have variants[] array with permuted versions
  // Original models in ourcolormodels[] are unchanged
}

// Create a library of transformed models
void buildModelLibrary() {
  // Create reflected versions
  colormodel* flowReflected = ourcolormodels[0]->applyEdgePermutation(
    edgePermutationArray[2], 
    "flowoctahedron_reflected"
  );
  
  // Create inverted versions
  colormodel* flowInverted = ourcolormodels[0]->applyEdgePermutation(
    edgePermutationArray[3], 
    "flowoctahedron_inverted"
  );
  
  // Create compound transformations
  String compound[] = {"rotate30", "reflect"};
  colormodel* flowCompound = ourcolormodels[0]->applyEdgePermutationSequence(
    compound, 
    2, 
    "flowoctahedron_compound"
  );
  
  // Store these in an array or use them in sequences
  Serial.println("Model library built with transformed variants");
}

// Note: Each call creates a NEW model
// - Original models remain unchanged
// - You get back a pointer to the new model
// - You're responsible for memory management (delete when done)
// - The optional newName parameter lets you name the new model
*/