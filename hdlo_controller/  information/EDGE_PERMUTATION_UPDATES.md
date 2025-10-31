# Edge Permutation System Updates

## Summary of Changes

I've updated your edge permutation system to include:

1. **Automatic registration** - Edge permutations now register themselves when created
2. **Permutation composition** - New methods to combine multiple permutations
3. **Registry-based lookup** - Find permutations by name using a centralized registry

---

## Key Features Added

### 1. Automatic Registration System

Edge permutations now automatically register themselves in a global registry, similar to your colormodel system.

```cpp
// Old way (manual arrays)
EdgePermutation edgePermutationArray[5] = { ... };
String edgePermutationNames[5] = { ... };

// New way (automatic registration)
EdgePermutation myPerm("custom_transform", permArray, true);  // Automatically registers!

// Look up by name
EdgePermutation* found = EdgePermutation::findPermutationByName("custom_transform");
```

### 2. Permutation Composition

Combine multiple permutations into a single new permutation:

```cpp
// Compose permutations by name
String perms[] = {"rotate30", "reflect", "invert"};
EdgePermutation* composed = EdgePermutation::composeByName(
    perms, 
    3, 
    "my_complex_transform",
    true  // Register the result
);

// Now available by name for all models
EdgePermutation* found = EdgePermutation::findPermutationByName("my_complex_transform");
```

### 3. Registry Management

Access and manage the permutation registry:

```cpp
// Print all available permutations
EdgePermutation::printRegistry();

// Get registry info
int numPerms = EdgePermutation::getNumRegisteredPermutations();
EdgePermutation** allPerms = EdgePermutation::getPermutationRegistry();
String* allNames = EdgePermutation::getPermutationNameRegistry();
```

---

## API Reference

### EdgePermutation Class

#### Constructors
```cpp
EdgePermutation();  // Creates identity permutation (unregistered)
EdgePermutation(String name, std::array<int, 120> perm, bool registerPerm = true);
```

#### Methods
```cpp
// Registration
void registerSelf();

// Lookup
static EdgePermutation* findPermutationByName(String name);
static void printRegistry();

// Composition
EdgePermutation compose(const EdgePermutation& other, String newName, bool registerResult = true) const;
static EdgePermutation* composeByName(const String* permNames, int numPerms, String newName, bool registerResult = true);

// Accessors
String getName() const;
int getPermutation(int index) const;
const std::array<int, 120>& getPermutationArray() const;
```

---

## Usage Examples

### Example 1: Creating and Registering Custom Permutations

```cpp
// Define your permutation array
std::array<int, 120> myPerm = {{ 
    // ... your 120 values ...
}};

// Create and automatically register
EdgePermutation customPerm("my_custom", myPerm, true);

// Now available everywhere by name
String perms[] = {"my_custom"};
colormodel* transformed = ourcolormodels[0]->applyEdgePermutationSequence(
    perms, 1, "flow_custom"
);
```

### Example 2: Composing Permutations

```cpp
// Build a library of composed permutations
void buildPermutationLibrary() {
    // Rotate then reflect
    String comp1[] = {"rotate30", "reflect"};
    EdgePermutation::composeByName(comp1, 2, "rot_reflect", true);
    
    // Reflect then invert
    String comp2[] = {"reflect", "invert"};
    EdgePermutation::composeByName(comp2, 2, "reflect_invert", true);
    
    // Complex triple composition
    String comp3[] = {"cyclicshift12", "rotate30", "reflect"};
    EdgePermutation::composeByName(comp3, 3, "triple_transform", true);
    
    // All registered and available!
    EdgePermutation::printRegistry();
}
```

### Example 3: Using Composed Permutations

```cpp
// Create a complex transformation
String myTransform[] = {"rotate30", "reflect", "invert"};
EdgePermutation* perm = EdgePermutation::composeByName(
    myTransform, 3, "special_transform", true
);

// Apply to multiple models by name
for(int i = 0; i < nummodels; i++) {
    String perms[] = {"special_transform"};
    String newName = ourcolormodels[i]->getModelName() + "_special";
    colormodel* variant = ourcolormodels[i]->applyEdgePermutationSequence(
        perms, 1, newName
    );
}
```

### Example 4: On-the-Fly Composition (without registration)

```cpp
// Compose without registering (for temporary use)
String temp[] = {"rotate30", "reflect"};
EdgePermutation* tempPerm = EdgePermutation::composeByName(
    temp, 2, "temp_transform", 
    false  // Don't register
);

// Use it once
colormodel* tempModel = ourcolormodels[0]->applyEdgePermutation(
    *tempPerm, "temp_model"
);

// Clean up
delete tempPerm;
```

---

## Backward Compatibility

The legacy arrays are still available for existing code:

```cpp
// These still work (they point to the registry)
extern EdgePermutation** edgePermutationArray;
extern String* edgePermutationNames;
extern int numEdgePermutations;

// Old code continues to work
colormodel* model = ourcolormodels[0]->applyEdgePermutation(
    *edgePermutationArray[1],  // Still works!
    "rotated"
);
```

---

## Updated Files

1. **edgepermutations.h** - Added registry system and composition methods
2. **edgepermutations.cpp** - Implemented registration and composition logic
3. **namedpermutations.cpp** - Updated to use automatic registration
4. **models.cpp** - Updated to use new registry lookup system

---

## Migration Guide

### If you have existing manual permutation arrays:

**Before:**
```cpp
// Manual array definition
EdgePermutation myPerms[3] = {
    EdgePermutation("perm1", data1),
    EdgePermutation("perm2", data2),
    EdgePermutation("perm3", data3)
};
String myPermNames[3] = {"perm1", "perm2", "perm3"};
```

**After:**
```cpp
// Just create them - they auto-register
EdgePermutation perm1("perm1", data1, true);
EdgePermutation perm2("perm2", data2, true);
EdgePermutation perm3("perm3", data3, true);

// Access via registry
EdgePermutation* found = EdgePermutation::findPermutationByName("perm1");
```

---

## Benefits

1. **No manual array management** - Permutations register themselves
2. **Dynamic composition** - Create new permutations at runtime
3. **Name-based access** - Look up any permutation by name
4. **Consistent with colormodel** - Same registration pattern
5. **Backward compatible** - Existing code still works
6. **Debugging friendly** - printRegistry() shows all available permutations

---

## Testing

To verify the system is working:

```cpp
void testPermutations() {
    // Print what's registered
    Serial.println("=== Testing Permutation System ===");
    EdgePermutation::printRegistry();
    
    // Test composition
    String test[] = {"rotate30", "reflect"};
    EdgePermutation* composed = EdgePermutation::composeByName(
        test, 2, "test_composed", true
    );
    
    if(composed != nullptr) {
        Serial.println("Composition successful!");
    }
    
    // Verify it's findable
    EdgePermutation* found = EdgePermutation::findPermutationByName("test_composed");
    if(found != nullptr) {
        Serial.println("Registry lookup successful!");
    }
    
    EdgePermutation::printRegistry();
}
```

---

## Notes

- Intermediate compositions (during `composeByName`) are not registered unless `registerResult=true`
- You can create unregistered permutations for temporary use by passing `false` as the third constructor parameter
- The registry has a maximum of 50 permutations (configurable via `MAX_PERMUTATIONS`)
- Duplicate names are detected and prevented
