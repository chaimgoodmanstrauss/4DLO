////////////////////////////////////
//
//   models.cpp
//
// Implementation of colormodel and ColorFunctionFactory
//

#include "models.h"
#include "colorfunctions.h"
#include "edgepermutations.h"
#include "paletteregistry.h"
#include "globalids.h"

// Static member initialization
ColorFunctionFactory* ColorFunctionFactory::instance = nullptr;
colormodel* colormodel::modelRegistry[colormodel::MAX_MODELS];
int colormodel::modelIdRegistry[colormodel::MAX_MODELS];
int colormodel::numRegisteredModels = 0;

////////////////////////////////////
// ColorFunctionFactory implementation

std::shared_ptr<StatefulColorFunction> ColorFunctionFactory::getShared(int id) {
    auto it = cache.find(id);
    if(it != cache.end()) {
        return it->second;
    }
    
    StatefulColorFunction* raw = create(id);
    if(!raw) return nullptr;
    
    std::shared_ptr<StatefulColorFunction> shared(raw);
    cache[id] = shared;
    return shared;
}

std::shared_ptr<StatefulColorFunction> ColorFunctionFactory::getSharedConfigured(
    int functionId, int paletteId, const std::vector<FunctionParameter>& params) {
    
    uint32_t hash = functionId * 10000 + paletteId;
    for(size_t i = 0; i < params.size() && i < 4; i++) {
        uint32_t* fp = (uint32_t*)&params[i].value;
        hash ^= (*fp << (i * 8));
    }
    
    auto it = cache.find(hash);
    if(it != cache.end()) {
        return it->second;
    }
    
    StatefulColorFunction* raw = create(functionId);
    if(!raw) return nullptr;
    
    if(paletteId != 0) {
        raw->setPalette(paletteId);
    }
    if(!params.empty()) {
        raw->setParameters(params);
    }
    
    std::shared_ptr<StatefulColorFunction> shared(raw);
    cache[hash] = shared;
    return shared;
}

StatefulColorFunction* ColorFunctionFactory::create(int id) {
    auto it = creators.find(id);
    if(it != creators.end()) {
        return it->second();
    }
    Serial.print("Error: Function ID ");
    Serial.print(id);
    Serial.println(" not found");
    return nullptr;
}

void ColorFunctionFactory::clearCache() {
    cache.clear();
}

void ColorFunctionFactory::clearFunction(int id) {
    auto it = cache.find(id);
    if(it != cache.end()) {
        cache.erase(it);
    }
}

void ColorFunctionFactory::listFunctions() {
    Serial.println("\n=== Registered Functions ===");
    for(const auto& pair : creators) {
        Serial.print("  ID: ");
        Serial.println(pair.first);
    }
    Serial.println("============================\n");
}

void ColorFunctionFactory::printCacheStats() {
    Serial.println("\n=== Function Cache ===");
    Serial.print("Cached: ");
    Serial.println(cache.size());
    Serial.println("======================\n");
}

////////////////////////////////////
// colormodel implementation

colormodel::colormodel(std::array<std::array<int, 6>, 120> importedgemodels, 
                       int importId, bool registerModel)
    : edgemodels(importedgemodels), modelId(importId), shouldRegister(registerModel) {
    
    for(int i = 0; i < 120; i++) {
        edgeFunctions[i] = nullptr;
        edgePalettes[i] = 0;
    }
    
    if(shouldRegister) {
        registerSelf();
    }
}

colormodel::colormodel(const colormodel& other)
    : edgemodels(other.edgemodels), modelId(other.modelId), shouldRegister(false) {
    
    for(int i = 0; i < 120; i++) {
        edgeFunctions[i] = other.edgeFunctions[i];
        edgePalettes[i] = other.edgePalettes[i];
    }
}

void colormodel::registerSelf() {
    if(numRegisteredModels >= MAX_MODELS) {
        Serial.println("Error: Model registry full");
        return;
    }
    
    for(int i = 0; i < numRegisteredModels; i++) {
        if(modelIdRegistry[i] == modelId) {
            Serial.print("Warning: Model ID ");
            Serial.print(modelId);
            Serial.println(" already registered");
            return;
        }
    }
    
    modelRegistry[numRegisteredModels] = this;
    modelIdRegistry[numRegisteredModels] = modelId;
    numRegisteredModels++;
    
    Serial.print("Registered model ID: ");
    Serial.println(modelId);
}

colormodel* colormodel::findModelById(int id) {
    for(int i = 0; i < numRegisteredModels; i++) {
        if(modelIdRegistry[i] == id) {
            return modelRegistry[i];
        }
    }
    return nullptr;
}

CRGB colormodel::getcolorfunction(int edgeindex, float position) {
    if(edgeindex < 0 || edgeindex >= 120) {
        return CRGB::Black;
    }
    
    if(edgeFunctions[edgeindex]) {
        edgeFunctions[edgeindex]->updateIfNeeded(millis());
        return edgeFunctions[edgeindex]->getColor(position);
    }
    
    return CRGB::Black;
}

void colormodel::setColorFunction(int edgeindex, int functionId, 
                                   int paletteId,
                                   const std::vector<FunctionParameter>& params) {
    if(edgeindex < 0 || edgeindex >= 120) return;
    
    auto& factory = ColorFunctionFactory::getInstance();
    auto func = factory.getSharedConfigured(functionId, paletteId, params);
    
    if(func) {
        edgeFunctions[edgeindex] = func;
        edgePalettes[edgeindex] = paletteId;
    }
}

colormodel* colormodel::applyEdgePermutation(const EdgePermutation& perm, int newId, bool registerModel) const {
    std::array<std::array<int, 6>, 120> newEdgeModels;
    
    for(int i = 0; i < 120; i++) {
        int sourceIndex = perm.getPermutation(i);
        bool flipDirection = false;
        
        if(sourceIndex < 0) {
            sourceIndex = abs(sourceIndex);
            flipDirection = true;
        }
        
        if(sourceIndex >= 0 && sourceIndex < 120) {
            newEdgeModels[i] = edgemodels[sourceIndex];
            if(flipDirection) {
                newEdgeModels[i][1] *= -1;
            }
        } else {
            newEdgeModels[i] = {{0, 1, 0, 10000}};
        }
    }
    
    return new colormodel(newEdgeModels, newId, registerModel);
}

colormodel* colormodel::applyEdgePermutation(const std::array<int, 120>& permArray, int newId, bool registerModel) const {
    EdgePermutation tempPerm(0, permArray, false);
    return applyEdgePermutation(tempPerm, newId, registerModel);
}

colormodel* colormodel::applyEdgePermutationSequence(const int* permIds, int numPerms, int newId) const {
    EdgePermutation composed = EdgePermutation::composeById(permIds, numPerms, 0, false);
    return applyEdgePermutation(composed, newId, true);
}

colormodel* colormodel::applyEdgePermutationSequence(std::initializer_list<int> permIds, int newId, bool mergeWithOriginal) const {
    std::vector<int> idVec(permIds);
    return applyEdgePermutationSequence(idVec.data(), idVec.size(), newId);
}

colormodel* colormodel::applyEdgePermutation(int modelId, const EdgePermutation& perm, int newId) {
    colormodel* model = findModelById(modelId);
    if(!model) {
        Serial.print("Error: Model ID ");
        Serial.print(modelId);
        Serial.println(" not found");
        return nullptr;
    }
    return model->applyEdgePermutation(perm, newId, true);
}

colormodel* colormodel::applyEdgePermutation(int modelId, const std::array<int, 120>& permArray, int newId) {
    colormodel* model = findModelById(modelId);
    if(!model) return nullptr;
    return model->applyEdgePermutation(permArray, newId, true);
}

colormodel* colormodel::applyEdgePermutation(int modelId, int permId, int newId) {
    colormodel* model = findModelById(modelId);
    if(!model) {
        Serial.print("Error: Model ID ");
        Serial.print(modelId);
        Serial.println(" not found");
        return nullptr;
    }
    
    EdgePermutation* perm = EdgePermutation::findPermutationById(permId);
    if(!perm) {
        Serial.print("Error: Permutation ID ");
        Serial.print(permId);
        Serial.println(" not found");
        return nullptr;
    }
    
    return model->applyEdgePermutation(*perm, newId, true);
}

colormodel* colormodel::applyEdgePermutationSequence(int modelId, const int* permIds, int numPerms, int newId) {
    colormodel* model = findModelById(modelId);
    if(!model) return nullptr;
    return model->applyEdgePermutationSequence(permIds, numPerms, newId);
}

colormodel* colormodel::applyEdgePermutationSequence(int modelId, std::initializer_list<int> permIds, int newId, bool mergeWithOriginal) {
    colormodel* model = findModelById(modelId);
    if(!model) return nullptr;
    return model->applyEdgePermutationSequence(permIds, newId, mergeWithOriginal);
}

colormodel* colormodel::mergeModels(const colormodel* model1, const colormodel* model2, int newId) {
    if(!model1 || !model2) return nullptr;
    
    std::array<std::array<int, 6>, 120> merged;
    
    for(int i = 0; i < 120; i++) {
        if(model1->edgemodels[i][0] > 0) {
            merged[i] = model1->edgemodels[i];
        } else if(model2->edgemodels[i][0] > 0) {
            merged[i] = model2->edgemodels[i];
        } else {
            merged[i] = {{0, 1, 0, 10000}};
        }
    }
    
    return new colormodel(merged, newId, true);
}

colormodel* colormodel::mergeModels(int model1Id, int model2Id, int newId) {
    colormodel* m1 = findModelById(model1Id);
    colormodel* m2 = findModelById(model2Id);
    return mergeModels(m1, m2, newId);
}

void colormodel::printRegistry() {
    Serial.println("\n=== Model Registry ===");
    Serial.print("Registered: ");
    Serial.println(numRegisteredModels);
    for(int i = 0; i < numRegisteredModels; i++) {
        Serial.print("  [");
        Serial.print(i);
        Serial.print("] ID=");
        Serial.println(modelIdRegistry[i]);
    }
    Serial.println("======================\n");
}

void initializefancymodels() {
    // Placeholder for fancy model initialization
}
