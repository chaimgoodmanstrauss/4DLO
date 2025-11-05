////////////////////////////////////
//
// colorfunctionclass instances do one thing:
// return its colorfunction(position) 
// colorfunction may incorporate other information such as time
// (in some other manner than already linearly encoded in position)
// sound, or other data.
//
#include <FastLED.h>
#include "models.h"
#include "edgepermutations.h"
#include "colorfunctions.h"
//#include "edgesetup.h" // just for positionresolution=10000

////////////////////////////////////
//
// colorfunctionclass implementation
//
colorfunctionclass::colorfunctionclass()
  : name(""), colorfunction(nullptr) {}

colorfunctionclass::colorfunctionclass(String inputname, ColorFunction inputcolorfunction) {
  name = inputname;
  colorfunction = inputcolorfunction;
}

CRGB colorfunctionclass::returncolor(float position) {
  if (colorfunction != nullptr) {
    return colorfunction(position);
  }
  return CRGB::Black;  // Default if no function set
}

////////////////////////////////////
//
// colormodel implementation
//

// Initialize static members
colormodel* colormodel::modelRegistry[MAX_MODELS] = {nullptr};
String colormodel::modelNameRegistry[MAX_MODELS] = {""};
int colormodel::numRegisteredModels = 0;

colormodel::colormodel(std::array<std::array<int, 6>, 120> importedgemodels, String importname, bool registerModel)
  : edgemodels(importedgemodels), modelname(importname), shouldRegister(registerModel) {
  // Only register if requested
  if(shouldRegister) {
    registerSelf();
  }
}

void colormodel::registerSelf() {
  if(numRegisteredModels >= MAX_MODELS) {
    Serial.println("ERROR: Model registry full! Cannot register: " + modelname);
    return;
  }
  
  // Check if name already exists (avoid duplicates)
  for(int i = 0; i < numRegisteredModels; i++) {
    if(modelNameRegistry[i].equals(modelname)) {
      Serial.println("Warning: Model name '" + modelname + "' already registered. Skipping duplicate.");
      return;
    }
  }
  
  modelRegistry[numRegisteredModels] = this;
  modelNameRegistry[numRegisteredModels] = modelname;
  numRegisteredModels++;
  
  Serial.println("Registered model: " + modelname + " (total: " + String(numRegisteredModels) + ")");
}

colormodel* colormodel::findModelByName(String name) {
  for(int i = 0; i < numRegisteredModels; i++) {
    if(modelNameRegistry[i].equalsIgnoreCase(name)) {
      return modelRegistry[i];
    }
  }
  return nullptr;
}

void colormodel::printRegistry() {
  Serial.println("=== Model Registry ===");
  Serial.println("Total models: " + String(numRegisteredModels));
  for(int i = 0; i < numRegisteredModels; i++) {
    Serial.println("  [" + String(i) + "] " + modelNameRegistry[i]);
  }
  Serial.println("=====================");
}

// Track edge-specific palettes
static String edgePalettes[120];
static String edgeFunctions[120];
static bool edgePalettesInitialized = false;

// Function to set edge-specific palettes
void setEdgePalette(int edgeIndex, String functionName, String paletteName) {
  if(!edgePalettesInitialized) {
    for(int i = 0; i < 120; i++) {
      edgePalettes[i] = "";
      edgeFunctions[i] = "";
    }
    edgePalettesInitialized = true;
  }
  
  if(edgeIndex >= 0 && edgeIndex < 120) {
    edgeFunctions[edgeIndex] = functionName;
    edgePalettes[edgeIndex] = paletteName;
  }
}

CRGB colormodel::getcolorfunction(int edgeindex, float position) {
  // Initialize edge palette arrays if needed
  if(!edgePalettesInitialized) {
    for(int i = 0; i < 120; i++) {
      edgePalettes[i] = "";
      edgeFunctions[i] = "";
    }
    edgePalettesInitialized = true;
  }
  
  // Check if this edge has a specific palette set
  if(edgeFunctions[edgeindex] != "" && edgePalettes[edgeindex] != "") {
    // Always apply the edge-specific palette (no caching)
    switchPalette(edgeFunctions[edgeindex], edgePalettes[edgeindex]);
  }
  
  // Calculate the transformed position
  float transformedPosition = (float)(edgemodels[edgeindex][2])/(10000.)+
                              (float)(edgemodels[edgeindex][3])/(10000.)*position;
  
  // DEBUG
  static int debugCount = 0;
  if(debugCount < 10) {
    Serial.print("MODEL: edge="); Serial.print(edgeindex);
    Serial.print(" inPos="); Serial.print(position, 4);
    Serial.print(" [1]="); Serial.print(edgemodels[edgeindex][1]);
    Serial.print(" [2]="); Serial.print(edgemodels[edgeindex][2]);
    Serial.print(" outPos="); Serial.println(transformedPosition, 6);
    debugCount++;
  }
  
  // Use the colorfunctionclass object's returncolor method
  return colorfunctions[edgemodels[edgeindex][0]].returncolor(transformedPosition);
}

void colormodel::setColorFunction(int index, String name, ColorFunction func) {
  if (index >= 0 && index < 120) {
    colorfunctions[index] = colorfunctionclass(name, func);
  }
}

////////////////////////////////////
//
// Edge permutation methods - these create NEW models (by pointer)
//

colormodel* colormodel::applyEdgePermutation(const EdgePermutation& perm, String newName, bool registerModel) const {
  return applyEdgePermutation(perm.getPermutationArray(), newName, registerModel);
}

colormodel* colormodel::applyEdgePermutation(const std::array<int, 120>& permArray, String newName, bool registerModel) const {
  // Create new edge model array with the permutation applied
  std::array<std::array<int, 6>, 120> newEdgeModels;
  
  // Apply the permutation: newEdgeModels[i] = edgemodels[abs(permArray[i])]
  // If permArray[i] is negative, also flip the sign of the direction (index 1)
  for(int i = 0; i < 120; i++) {
    int permValue = permArray[i];
    int sourceIndex = abs(permValue);
    
    if(sourceIndex >= 0 && sourceIndex < 120) {
      newEdgeModels[i] = edgemodels[sourceIndex];
      
      // If the permutation value is negative, flip the direction
      if(permValue < 0) {
        // Index 1 contains the direction (1 or -1)
        newEdgeModels[i][1] = -newEdgeModels[i][1];
      }
    }
  }
  
  // Create the new model name
  String resultName = newName;
  if(resultName == "") {
    resultName = modelname + "_permuted";
  }
  
  // Create and return new model - only register if requested
  colormodel* newModel = new colormodel(newEdgeModels, resultName, registerModel);
  
  // Copy color functions to the new model
  for(int i = 0; i < 120; i++) {
    newModel->colorfunctions[i] = this->colorfunctions[i];
  }
  
  return newModel;
}

colormodel* colormodel::applyEdgePermutationSequence(const String* permNames, int numPerms, String newName) const {
  // Create temporary model WITHOUT registering it
  colormodel* currentModel = new colormodel(edgemodels, modelname + "_temp", false);
  
  // Copy color functions
  for(int i = 0; i < 120; i++) {
    currentModel->colorfunctions[i] = this->colorfunctions[i];
  }
  
  // Apply each permutation in sequence - NEVER register intermediate models
  for(int p = 0; p < numPerms; p++) {
    // Find the permutation by name using the new registry system
    EdgePermutation* perm = EdgePermutation::findPermutationByName(permNames[p]);
    
    if(perm != nullptr) {
      // Create new model WITHOUT registering (pass false)
      colormodel* nextModel = currentModel->applyEdgePermutation(
        *perm, 
        "", 
        false  // Don't register any intermediate models
      );
      
      // Delete the intermediate model (safe - it wasn't registered)
      delete currentModel;
      currentModel = nextModel;
      
      Serial.println("Applied permutation: " + permNames[p]);
    } else {
      Serial.println("Warning: Permutation '" + permNames[p] + "' not found");
    }
  }
  
  // Set the final name
  String resultName = newName;
  if(resultName == "") {
    resultName = modelname + "_permuted";
  }
  currentModel->modelname = resultName;
  
  // Now register with the correct name
  currentModel->registerSelf();
  
  return currentModel;
}

////////////////////////////////////
//
// Edge permutation methods - static versions (by name)
//

colormodel* colormodel::applyEdgePermutation(String modelName, const EdgePermutation& perm, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model == nullptr) {
    Serial.println("ERROR: Model '" + modelName + "' not found for applyEdgePermutation");
    return nullptr;
  }
  return model->applyEdgePermutation(perm, newName);
}

colormodel* colormodel::applyEdgePermutation(String modelName, const std::array<int, 120>& permArray, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model == nullptr) {
    Serial.println("ERROR: Model '" + modelName + "' not found for applyEdgePermutation");
    return nullptr;
  }
  return model->applyEdgePermutation(permArray, newName);
}

colormodel* colormodel::applyEdgePermutationSequence(String modelName, const String* permNames, int numPerms, String newName) {
  colormodel* model = findModelByName(modelName);
  if(model == nullptr) {
    Serial.println("ERROR: Model '" + modelName + "' not found for applyEdgePermutationSequence");
    return nullptr;
  }
  return model->applyEdgePermutationSequence(permNames, numPerms, newName);
}

////////////////////////////////////
//
// Model merging - creates a NEW model by combining two models
// Takes non-default values from model1, then model2
//

colormodel* colormodel::mergeModels(const colormodel* model1, const colormodel* model2, String newName) {
  if(model1 == nullptr || model2 == nullptr) {
    Serial.println("ERROR: Cannot merge null models");
    return nullptr;
  }
  
  // Create new edge model array
  std::array<std::array<int, 6>, 120> mergedEdgeModels;
  
  // Default pattern to check against
  std::array<int, 6> defaultPattern = {{0, 1, 0, 10000, 0, 10000}};
  
  // Merge the edge models
  for(int i = 0; i < 120; i++) {
    // Check if model1 has non-default value at this index
    if(model1->edgemodels[i] != defaultPattern) {
      mergedEdgeModels[i] = model1->edgemodels[i];
    }
    // Otherwise check model2
    else if(model2->edgemodels[i] != defaultPattern) {
      mergedEdgeModels[i] = model2->edgemodels[i];
    }
    // Both are default, use default
    else {
      mergedEdgeModels[i] = defaultPattern;
    }
  }
  
  // Create the new model (automatically registers itself)
  colormodel* newModel = new colormodel(mergedEdgeModels, newName);
  
  // Copy color functions from model1 (preferring first model)
  for(int i = 0; i < 120; i++) {
    newModel->colorfunctions[i] = model1->colorfunctions[i];
  }
  
  return newModel;
}

////////////////////////////////////
//
// Model merging - by name (overload)
//

colormodel* colormodel::mergeModels(String model1Name, String model2Name, String newName) {
  colormodel* model1 = findModelByName(model1Name);
  colormodel* model2 = findModelByName(model2Name);
  
  if(model1 == nullptr) {
    Serial.println("ERROR: Model '" + model1Name + "' not found for mergeModels");
    return nullptr;
  }
  
  if(model2 == nullptr) {
    Serial.println("ERROR: Model '" + model2Name + "' not found for mergeModels");
    return nullptr;
  }
  
  return mergeModels(model1, model2, newName);
}


//==========================================

void initializefancymodels(){
/*
  // Create merged models using string names - SO EASY!
  colormodel::mergeModels("flowoctahedron", "octachain", "flow_octa_merged");
  colormodel::mergeModels("cycle", "cycles", "cycle_merged");
  colormodel::mergeModels("cube", "hypercube", "cube_hyper_merged");
  
  // Create permuted models using string names
  String perms1[] = {"rotate30", "reflect"};
  colormodel::applyEdgePermutationSequence("flowoctahedron", perms1, 2, "flow_rotated_reflected");
  
  String perms2[] = {"invert"};
  colormodel::applyEdgePermutationSequence("allcycles", perms2, 1, "allcycles_inverted");
  */
  // Set color functions for all newly created models
  for(int modelIdx = 0; modelIdx < colormodel::getNumRegisteredModels(); modelIdx++) {
    colormodel* model = colormodel::getModelRegistry()[modelIdx];
    for(int funcIdx = 0; funcIdx < numcolorfunctions; funcIdx++) {
      model->setColorFunction(funcIdx, 
                              colorFunctionNames[funcIdx], 
                              colorFunctionArray[funcIdx]);
    }
  }}