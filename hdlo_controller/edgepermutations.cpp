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
    
    if(PRINTPERMUTATIONS){
    Serial.println("Registered permutation: " + name + " (total: " + String(numRegisteredPermutations) + ")");
    }
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

EdgePermutation* EdgePermutation::composeByName(const String* permNames, int numPerms, String newName, bool registerResult) {
    if(numPerms <= 0) {
        Serial.println("Error: No permutations to compose");
        return nullptr;
    }
    
    // Find first permutation
    EdgePermutation* firstPerm = findPermutationByName(permNames[0]);
    if(firstPerm == nullptr) {
        Serial.println("Error: Permutation '" + permNames[0] + "' not found");
        return nullptr;
    }
    
    Serial.println("Starting composition with: " + permNames[0]);
    
    // Start with a copy of the first permutation (don't register intermediate)
    EdgePermutation* result = new EdgePermutation(firstPerm->name, firstPerm->permutation, false);
    
    // Compose with remaining permutations
    for(int p = 1; p < numPerms; p++) {
        EdgePermutation* nextPerm = findPermutationByName(permNames[p]);
        
        if(nextPerm == nullptr) {
            Serial.println("Warning: Permutation '" + permNames[p] + "' not found, skipping");
            continue;
        }
        
        Serial.println("Composing with: " + permNames[p]);
        
        // Compose (don't register intermediate results)
        String tempName = result->getName() + "_" + permNames[p];
        EdgePermutation tempResult = result->compose(*nextPerm, tempName, false);
        
        // Update result
        *result = tempResult;
    }
    
    // Set final name and register if requested
    result->name = newName;
    if(registerResult) {
        result->shouldRegister = true;
        result->registerSelf();
    }
    
    return result;
}

  /*
 // Example usage in your main code or setup():

#include "edgepermutations.h"
#include "models.h"
#include "hdlo_models.h"

void setupExample() {
  // Initialize your models as usual
  initializemodels();
  
  // Print available permutations (automatically registered)
  EdgePermutation::printRegistry();
  
  // Example 1: Find and use a permutation by name
  EdgePermutation* rotatePerm = EdgePermutation::findPermutationByName("rotate30");
  if(rotatePerm != nullptr) {
    colormodel* rotatedModel = ourcolormodels[0]->applyEdgePermutation(
      *rotatePerm, 
      "flowoctahedron_rotated"
    );
  }
  
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
  
  // NEW Example 5: Compose permutations to create a new named permutation (automatically registered)
  String permsToCompose[] = {"rotate30", "reflect"};
  EdgePermutation* rotateAndReflect = EdgePermutation::composeByName(
    permsToCompose,
    2,
    "rotate30_then_reflect",
    true  // Register the result
  );
  
  // The composed permutation is now available by name!
  EdgePermutation* foundComposed = EdgePermutation::findPermutationByName("rotate30_then_reflect");
  if(foundComposed != nullptr) {
    colormodel* composedModel = ourcolormodels[0]->applyEdgePermutation(
      *foundComposed,
      "flowoctahedron_composed"
    );
  }
  
  // NEW Example 6: Create custom permutation and register it
  std::array<int, 120> myCustomPerm;
  // ... fill in your custom permutation values ...
  
  // This will automatically register
  EdgePermutation myPerm("my_custom_transform", myCustomPerm, true);
  
  // Now it's available by name
  EdgePermutation* found = EdgePermutation::findPermutationByName("my_custom_transform");
  
  // NEW Example 7: Compose multiple permutations for complex transformations
  String complexComposition[] = {"rotate30", "reflect", "cyclicshift12", "invert"};
  EdgePermutation* superPerm = EdgePermutation::composeByName(
    complexComposition,
    4,
    "super_transformation",
    true  // Register it
  );
  
  // Apply to any model using the name
  String superPermArray[] = {"super_transformation"};
  colormodel* transformedCycle = ourcolormodels[2]->applyEdgePermutationSequence(
    superPermArray,
    1,
    "cycle_super_transformed"
  );
  
  // The original models are still intact!
  Serial.println("Original model: " + ourcolormodels[0]->getModelName());
  
  // Check the registry at any time
  EdgePermutation::printRegistry();
}

// Helper function to create a library of composed permutations
void buildPermutationLibrary() {
  // Compose and register new permutations
  String comp1[] = {"rotate30", "reflect"};
  EdgePermutation::composeByName(comp1, 2, "rot_reflect", true);
  
  String comp2[] = {"reflect", "invert"};
  EdgePermutation::composeByName(comp2, 2, "reflect_invert", true);
  
  String comp3[] = {"cyclicshift12", "rotate30", "reflect"};
  EdgePermutation::composeByName(comp3, 3, "triple_transform", true);
  
  // All of these are now available by name!
  EdgePermutation::printRegistry();
  
  // Use any of them
  EdgePermutation* myPerm = EdgePermutation::findPermutationByName("triple_transform");
  if(myPerm != nullptr) {
    colormodel* transformed = ourcolormodels[0]->applyEdgePermutation(
      *myPerm,
      "flow_triple_transformed"
    );
  }
}

// Create permuted variants of all models
void createPermutedVariants(String permName) {
  EdgePermutation* perm = EdgePermutation::findPermutationByName(permName);
  if(perm == nullptr) {
    Serial.println("Error: Permutation '" + permName + "' not found");
    return;
  }
  
  colormodel* variants[10];
  
  for(int i = 0; i < nummodels; i++) {
    String newName = ourcolormodels[i]->getModelName() + "_" + permName;
    variants[i] = ourcolormodels[i]->applyEdgePermutation(*perm, newName);
    
    Serial.println("Created variant: " + variants[i]->getModelName());
  }
}

// Note: 
// - Permutations automatically register when created with registerPerm=true (default)
// - Use EdgePermutation::findPermutationByName() to look up by name
// - Composed permutations can be registered with the registerResult parameter
// - EdgePermutation::printRegistry() shows all available permutations
// - The legacy edgePermutationArray and edgePermutationNames point to the registry
*/