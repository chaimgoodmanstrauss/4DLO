////////////////////////////////////
//
//   models.cpp
//
// Model implementation with IColorFunction instance storage
//

#include "models.h"
#include "colorfunctions.h"
#include "edgepermutations.h"

////////////////////////////////////
//
// ColorFunctionFactory implementation
//

ColorFunctionFactory* ColorFunctionFactory::instance = nullptr;

// Get or create shared instance from cache (by name only)
std::shared_ptr<StatefulColorFunction> ColorFunctionFactory::getShared(const String& name) {
    // Check if already cached
    auto cacheIt = cache.find(name);
    if (cacheIt != cache.end()) {
        return cacheIt->second;
    }
    
    // Create new instance
    auto creatorIt = creators.find(name);
    if (creatorIt != creators.end()) {
        StatefulColorFunction* rawPtr = creatorIt->second();
        if (rawPtr) {
            std::shared_ptr<StatefulColorFunction> sharedPtr(rawPtr);
            cache[name] = sharedPtr;
            Serial.println("ColorFunctionFactory: Cached new instance of '" + name + "'");
            return sharedPtr;
        }
    }
    
    Serial.println("Warning: Unknown function '" + name + "', returning nullptr");
    return nullptr;
}

// Get or create shared instance with full configuration
std::shared_ptr<StatefulColorFunction> ColorFunctionFactory::getSharedConfigured(
    const String& name,
    const String& paletteName,
    const std::vector<FunctionParameter>& params) {
    
    // Generate cache key: "functionName:paletteName:param1,param2,..."
    String cacheKey = name + ":" + paletteName;
    if(params.size() > 0) {
        cacheKey += ":";
        for(size_t i = 0; i < params.size(); i++) {
            if(i > 0) cacheKey += ",";
            cacheKey += String(params[i].value, 3);  // 3 decimal places
        }
    }
    
    // Check if already cached
    auto cacheIt = cache.find(cacheKey);
    if (cacheIt != cache.end()) {
        return cacheIt->second;
    }
    
    // Create new instance
    auto creatorIt = creators.find(name);
    if (creatorIt != creators.end()) {
        StatefulColorFunction* rawPtr = creatorIt->second();
        if (rawPtr) {
            // Configure the instance
            if(paletteName.length() > 0) {
                rawPtr->setPalette(paletteName);
            }
            if(params.size() > 0) {
                rawPtr->setParameters(params);
            }
            
            // Cache and return
            std::shared_ptr<StatefulColorFunction> sharedPtr(rawPtr);
            cache[cacheKey] = sharedPtr;
            Serial.println("ColorFunctionFactory: Cached new instance '" + cacheKey + "'");
            return sharedPtr;
        }
    }
    
    Serial.println("Warning: Unknown function '" + name + "', returning nullptr");
    return nullptr;
}

// Legacy create method - creates unique instance (not cached)
StatefulColorFunction* ColorFunctionFactory::create(const String& name) {
    auto it = creators.find(name);
    if (it != creators.end()) {
        return it->second();
    }
    Serial.println("Warning: Unknown function '" + name + "', returning nullptr");
    return nullptr;
}

void ColorFunctionFactory::clearCache() {
    cache.clear();
    Serial.println("ColorFunctionFactory: Cache cleared");
}

void ColorFunctionFactory::clearFunction(const String& name) {
    auto it = cache.find(name);
    if (it != cache.end()) {
        cache.erase(it);
        Serial.println("ColorFunctionFactory: Cleared '" + name + "' from cache");
    }
}

void ColorFunctionFactory::listFunctions() {
    Serial.println("Registered color functions:");
    for (auto& pair : creators) {
        Serial.println("  - " + pair.first);
    }
}

void ColorFunctionFactory::printCacheStats() {
    Serial.println("\n=== Color Function Cache Stats ===");
    Serial.print("Total cached instances: ");
    Serial.println(cache.size());
    
    for (auto& pair : cache) {
        Serial.print("  ");
        Serial.print(pair.first);
        Serial.print(": ref_count = ");
        Serial.println(pair.second.use_count());
    }
    Serial.println("==================================\n");
}

////////////////////////////////////
//
// colormodel implementation
//

// Initialize static members
colormodel* colormodel::modelRegistry[MAX_MODELS] = {nullptr};
String colormodel::modelNameRegistry[MAX_MODELS] = {""};
int colormodel::numRegisteredModels = 0;

colormodel::colormodel(std::array<std::array<int, 6>, 120> importedgemodels, 
                       String importname, bool registerModel)
  : edgemodels(importedgemodels), modelname(importname), shouldRegister(registerModel) {
  
  // Initialize all edge functions to dark
  for(int i = 0; i < 120; i++) {
    edgeFunctions[i] = nullptr;
    edgePalettes[i] = "";
  }
  
  if(shouldRegister) {
    registerSelf();
  }
}

// Copy constructor - shared_ptr handles reference counting automatically
colormodel::colormodel(const colormodel& other)
  : edgemodels(other.edgemodels),
    modelname(other.modelname),
    edgeFunctions(other.edgeFunctions),  // Shallow copy - shares instances
    edgePalettes(other.edgePalettes),
    shouldRegister(other.shouldRegister) {
  // No deep copy needed - shared_ptr automatically manages references
}

void colormodel::registerSelf() {
  if(numRegisteredModels >= MAX_MODELS) {
    Serial.println("ERROR: Model registry full! Cannot register: " + modelname);
    return;
  }
  
  // Check for duplicates
  for(int i = 0; i < numRegisteredModels; i++) {
    if(modelNameRegistry[i].equals(modelname)) {
      Serial.println("Warning: Model name '" + modelname + "' already registered. Skipping duplicate.");
      return;
    }
  }
  
  modelRegistry[numRegisteredModels] = this;
  modelNameRegistry[numRegisteredModels] = modelname;
  numRegisteredModels++;
  
  Serial.print("Registered model: ");
  Serial.print(modelname);
  Serial.print(" (index ");
  Serial.print(numRegisteredModels - 1);
  Serial.println(")");
}

CRGB colormodel::getcolorfunction(int edgeindex, float position) {
  if(edgeindex < 0 || edgeindex >= 120) {
    return CRGB::Black;
  }

  // Do not remove this code!
  // Apply direction reversal if needed
  int direction = edgemodels[edgeindex][1];
  if(direction < 0) {
    position = 1.0 - position;
  }

  if(direction==0){
    position = 2.0*abs(.5-position);
  }
  
  
  // Get the base position transformation from edge data
  float startPos = edgemodels[edgeindex][2] / 10000.0;
  float scale = edgemodels[edgeindex][3] / 10000.0;
  float transformedPosition = startPos + scale * position;
  
  // Call the edge's color function
  if(edgeFunctions[edgeindex]) {
    edgeFunctions[edgeindex]->updateIfNeeded(millis());
    return edgeFunctions[edgeindex]->getColor(transformedPosition);
  }
  
  return CRGB::Black;
}

void colormodel::setColorFunction(int edgeindex, const String& functionName, 
                                   const String& paletteName,
                                   const std::vector<FunctionParameter>& params) {
  if(edgeindex < 0 || edgeindex >= 120) {
    return;
  }
  
  // Get shared instance from factory cache with full configuration
  std::shared_ptr<StatefulColorFunction> sharedFunc = 
      ColorFunctionFactory::getInstance().getSharedConfigured(functionName, paletteName, params);
  
  if(sharedFunc) {
    // Store shared_ptr (automatically manages reference counting)
    edgeFunctions[edgeindex] = sharedFunc;
    edgePalettes[edgeindex] = paletteName;
  }
}

colormodel* colormodel::applyEdgePermutation(const EdgePermutation& perm, String newName, bool registerModel) const {
  std::array<int, 120> permArray;
  for(int i = 0; i < 120; i++) {
    permArray[i] = perm.getPermutation(i);
  }
  return applyEdgePermutation(permArray, newName, registerModel);
}

colormodel* colormodel::applyEdgePermutation(const std::array<int, 120>& permArray, String newName, bool registerModel) const {
  std::array<std::array<int, 6>, 120> newEdgeModels;
  
  Serial.println("Applying permutation to model: " + modelname);
  
  for(int i = 0; i < 120; i++) {
    int sourceIndex = permArray[i];
    bool flip = (sourceIndex < 0);
    int absIndex = abs(sourceIndex);
    
    if(absIndex >= 0 && absIndex < 120) {
      newEdgeModels[i] = edgemodels[absIndex];
      
      if(flip) {
        newEdgeModels[i][1]=- edgemodels[absIndex][1];        // Reverse direction
      }
      
      // Debug first 7 edges
      if(i < 7 ) {
        Serial.print("  Edge ");
        Serial.print(i);
        Serial.print(" from Edge ");
        Serial.print(absIndex);
        if(flip) Serial.print(" (flipped)");
        Serial.print(" ");
        Serial.print(newEdgeModels[i][0]);
        Serial.print(" ");
        Serial.print(newEdgeModels[i][1]);
        Serial.print(" ");
        Serial.print(newEdgeModels[i][2]);
        Serial.print(" ");
        Serial.print(newEdgeModels[i][3]);
        Serial.println(" ");
      }
    } else {
      newEdgeModels[i] = edgemodels[i];
    }
  }
  
  if(newName == "") {
    newName = modelname + "_permuted";
  }
  
  colormodel* result = new colormodel(newEdgeModels, newName, registerModel);
  
  // Copy shared pointers (no cloning needed - instances are shared)
  for(int i = 0; i < 120; i++) {
    int sourceIndex = permArray[i];
    if(sourceIndex >= 0 && sourceIndex < 120 && edgeFunctions[sourceIndex]) {
      result->edgeFunctions[i] = edgeFunctions[sourceIndex];  // Share the instance
      result->edgePalettes[i] = edgePalettes[sourceIndex];
    }
  }
  
  return result;
}

colormodel* colormodel::applyEdgePermutationSequence(const String* permNames, int numPerms, String newName) const {
  colormodel* current = const_cast<colormodel*>(this);
  colormodel* temp = nullptr;
  
  for(int i = 0; i < numPerms; i++) {
    EdgePermutation* perm = EdgePermutation::findPermutationByName(permNames[i]);
    if(perm) {
      temp = current->applyEdgePermutation(*perm, "", false);
      if(i > 0 && current != this) {
        delete current;
      }
      current = temp;
    }
  }
  
  if(newName == "") {
    newName = modelname + "_sequence";
  }
  
  if(current != this) {
    current->modelname = newName;
    current->registerSelf();
  }
  
  return current;
}

colormodel* colormodel::applyEdgePermutationSequence(std::initializer_list<String> permNames, String newName, bool mergeWithOriginal) const {
  // Convert initializer_list to vector for easier iteration
  std::vector<String> permVector(permNames.begin(), permNames.end());
  int numPerms = permVector.size();
  
  colormodel* current = const_cast<colormodel*>(this);
  colormodel* temp = nullptr;
  
  for(int i = 0; i < numPerms; i++) {
    EdgePermutation* perm = EdgePermutation::findPermutationByName(permVector[i]);
    if(perm) {
      temp = current->applyEdgePermutation(*perm, "", false);
      if(i > 0 && current != this) {
        delete current;
      }
      current = temp;
    }
  }
  
  if(newName == "") {
    newName = modelname + "_sequence";
  }
  
  // Merge with original if requested
  if(mergeWithOriginal && current != this) {
    colormodel* merged = mergeModels(current, this, newName);
    if(current != this) {
      delete current;
    }
    return merged;
  }
  
  if(current != this) {
    current->modelname = newName;
    current->registerSelf();
  }
  
  return current;
}

colormodel* colormodel::applyEdgePermutation(String modelName, const EdgePermutation& perm, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model) {
    return model->applyEdgePermutation(perm, newName);
  }
  return nullptr;
}

colormodel* colormodel::applyEdgePermutation(String modelName, const std::array<int, 120>& permArray, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model) {
    return model->applyEdgePermutation(permArray, newName);
  }
  return nullptr;
}

colormodel* colormodel::applyEdgePermutation(String modelName, String permName, String newName) {
  colormodel* model = findModelByName(modelName);
  if(!model) {
    Serial.println("Error: Model '" + modelName + "' not found");
    return nullptr;
  }
  
  EdgePermutation* perm = EdgePermutation::findPermutationByName(permName);
  if(!perm) {
    Serial.println("Error: Permutation '" + permName + "' not found");
    return nullptr;
  }
  
  return model->applyEdgePermutation(*perm, newName);
}

colormodel* colormodel::applyEdgePermutationSequence(String modelName, const String* permNames, int numPerms, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model) {
    return model->applyEdgePermutationSequence(permNames, numPerms, newName);
  }
  return nullptr;
}

colormodel* colormodel::applyEdgePermutationSequence(String modelName, std::initializer_list<String> permNames, String newName, bool mergeWithOriginal) {
  colormodel* model = findModelByName(modelName);
  if(model) {
    return model->applyEdgePermutationSequence(permNames, newName, mergeWithOriginal);
  }
  return nullptr;
}

colormodel* colormodel::mergeModels(const colormodel* model1, const colormodel* model2, String newName) {
  std::array<std::array<int, 6>, 120> mergedEdgeModels;
  
  Serial.println("Merging models: " + model1->modelname + " + " + model2->modelname);
  
  for(int i = 0; i < 120; i++) {
    // Use model1's edge unless it's dark (function index 0), then use model2's
    if(model1->edgemodels[i][0] != 0) {
      mergedEdgeModels[i] = model1->edgemodels[i];
    } else {
      mergedEdgeModels[i] = model2->edgemodels[i];
    }
  }
  
  if(newName == "") {
    newName = model1->modelname + "_" + model2->modelname;
  }
  
  colormodel* result = new colormodel(mergedEdgeModels, newName, true);
  
  // Copy shared pointers - model1 takes priority, model2 fills in blanks
  for(int i = 0; i < 120; i++) {
    if(model1->edgemodels[i][0] != 0 && model1->edgeFunctions[i]) {
      result->edgeFunctions[i] = model1->edgeFunctions[i];  // Share the instance
      result->edgePalettes[i] = model1->edgePalettes[i];
    } else if(model2->edgeFunctions[i]) {
      result->edgeFunctions[i] = model2->edgeFunctions[i];  // Share the instance
      result->edgePalettes[i] = model2->edgePalettes[i];
    }
  }
  
  Serial.println("Merge complete: " + result->modelname);
  
  return result;
}

colormodel* colormodel::mergeModels(String model1Name, String model2Name, String newName) {
  colormodel* model1 = findModelByName(model1Name);
  colormodel* model2 = findModelByName(model2Name);
  
  if(model1 && model2) {
    return mergeModels(model1, model2, newName);
  }
  return nullptr;
}

colormodel* colormodel::findModelByName(String name) {
  for(int i = 0; i < numRegisteredModels; i++) {
    if(modelNameRegistry[i].equals(name)) {
      return modelRegistry[i];
    }
  }
  return nullptr;
}

void colormodel::printRegistry() {
  Serial.println("\n=== Model Registry ===");
  Serial.print("Total registered models: ");
  Serial.println(numRegisteredModels);
  
  for(int i = 0; i < numRegisteredModels; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(modelNameRegistry[i]);
  }
  Serial.println("======================\n");
}

////////////////////////////////////
//
// Initialize fancy model variations
//

void initializefancymodels() {
    Serial.println("\n=== Creating Fancy Models ===");
    /*
    // Create test_permed model
    colormodel* result = colormodel::applyEdgePermutation("test", "simpletest", "test_permed");
    if(result) {
        Serial.print("Successfully created model: ");
        Serial.println(result->getModelName());
    } else {
        Serial.println("ERROR: Failed to create test_permed");
    }*/
   
}

