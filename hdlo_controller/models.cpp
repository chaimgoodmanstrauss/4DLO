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

IColorFunction* ColorFunctionFactory::create(const String& name) {
    auto it = creators.find(name);
    if (it != creators.end()) {
        return it->second();
    }
    Serial.println("Warning: Unknown function '" + name + "', using dark");
    return create("dark");
}

void ColorFunctionFactory::listFunctions() {
    Serial.println("Registered color functions:");
    for (auto& pair : creators) {
        Serial.println("  - " + pair.first);
    }
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

// Copy constructor - essential for proper unique_ptr handling
colormodel::colormodel(const colormodel& other)
  : edgemodels(other.edgemodels),
    modelname(other.modelname),
    edgePalettes(other.edgePalettes),
    shouldRegister(other.shouldRegister) {
  
  // Deep copy all IColorFunction instances
  for(int i = 0; i < 120; i++) {
    if(other.edgeFunctions[i]) {
      edgeFunctions[i].reset(other.edgeFunctions[i]->clone());
    } else {
      edgeFunctions[i] = nullptr;
    }
  }
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
  
  // Create new function instance from factory
  IColorFunction* newFunc = ColorFunctionFactory::getInstance().create(functionName);
  
  if(newFunc) {
    // Set palette if provided
    if(paletteName.length() > 0) {
      newFunc->setPalette(paletteName);
    }
    
    // Set parameters if provided
    if(params.size() > 0) {
      newFunc->setParameters(params);
    }
    
    // Store in unique_ptr (automatically deletes old)
    edgeFunctions[edgeindex].reset(newFunc);
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
  
  for(int i = 0; i < 120; i++) {
    int sourceIndex = permArray[i];
    if(sourceIndex >= 0 && sourceIndex < 120) {
      newEdgeModels[i] = edgemodels[sourceIndex];
    } else {
      newEdgeModels[i] = edgemodels[i];
    }
  }
  
  if(newName == "") {
    newName = modelname + "_permuted";
  }
  
  colormodel* result = new colormodel(newEdgeModels, newName, registerModel);
  
  // Copy color functions
  for(int i = 0; i < 120; i++) {
    int sourceIndex = permArray[i];
    if(sourceIndex >= 0 && sourceIndex < 120 && edgeFunctions[sourceIndex]) {
      result->edgeFunctions[i].reset(edgeFunctions[sourceIndex]->clone());
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

colormodel* colormodel::applyEdgePermutationSequence(String modelName, const String* permNames, int numPerms, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model) {
    return model->applyEdgePermutationSequence(permNames, numPerms, newName);
  }
  return nullptr;
}

colormodel* colormodel::mergeModels(const colormodel* model1, const colormodel* model2, String newName) {
  std::array<std::array<int, 6>, 120> mergedEdgeModels;
  
  for(int i = 0; i < 120; i++) {
    if(i < 60) {
      mergedEdgeModels[i] = model1->edgemodels[i];
    } else {
      mergedEdgeModels[i] = model2->edgemodels[i - 60];
    }
  }
  
  if(newName == "") {
    newName = model1->modelname + "_" + model2->modelname;
  }
  
  colormodel* result = new colormodel(mergedEdgeModels, newName, true);
  
  // Copy color functions
  for(int i = 0; i < 120; i++) {
    if(i < 60 && model1->edgeFunctions[i]) {
      result->edgeFunctions[i].reset(model1->edgeFunctions[i]->clone());
      result->edgePalettes[i] = model1->edgePalettes[i];
    } else if(i >= 60 && model2->edgeFunctions[i - 60]) {
      result->edgeFunctions[i].reset(model2->edgeFunctions[i - 60]->clone());
      result->edgePalettes[i] = model2->edgePalettes[i - 60];
    }
  }
  
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
    
    // This function creates variations of base models using edge permutations
    // Example usage (uncomment and modify as needed):
    
    /*
    // Create a rotated version of a model
    EdgePermutation* rot24 = EdgePermutation::findPermutationByName("rot24");
    if(rot24) {
        std::array<int, 120> permArray;
        for(int i = 0; i < 120; i++) {
            permArray[i] = rot24->getPermutation(i);
        }
        colormodel::applyEdgePermutation("test", permArray, "test_rotated");
    }
    
    // Create a sequence of transformations
    String perms[] = {"rot24", "mirror_xy"};
    colormodel::applyEdgePermutationSequence("test", perms, 2, "test_transformed");
    */
    
    Serial.println("Fancy models created (none defined yet)");
    Serial.println("=============================\n");
}

