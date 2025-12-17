////////////////////////////////////
//
//   models.h
//
// Color function and model system with integer ID lookup
//
#ifndef MODELS_H
#define MODELS_H

const bool AUDIODEBUGGING = false;

#include <FastLED.h>
#include <array>
#include <map>
#include <functional>
#include <memory>
#include <vector>
#include "globalids.h"

// Forward declarations
struct FunctionParameter;
class StatefulColorFunction;
class ColorFunctionFactory;
class EdgePermutation;

/////////////////////////////////////////
// COLOR FUNCTION FACTORY with integer ID lookup
//
class ColorFunctionFactory {
private:
    std::map<int, std::function<StatefulColorFunction*()>> creators;
    std::map<uint32_t, std::shared_ptr<StatefulColorFunction>> cache;
    static ColorFunctionFactory* instance;
    
    ColorFunctionFactory() {}
    
public:
    static ColorFunctionFactory& getInstance() {
        if (instance == nullptr) {
            instance = new ColorFunctionFactory();
        }
        return *instance;
    }
    
    void registerFunction(int id, std::function<StatefulColorFunction*()> creator) {
        creators[id] = creator;
    }
    
    std::shared_ptr<StatefulColorFunction> getShared(int id);
    
    std::shared_ptr<StatefulColorFunction> getSharedConfigured(
        int functionId,
        int paletteId = 0,
        const std::vector<FunctionParameter>& params = {});
    
    StatefulColorFunction* create(int id);
    
    void clearCache();
    void clearFunction(int id);
    
    bool hasFunction(int id) const {
        return creators.find(id) != creators.end();
    }
    
    void listFunctions();
    void printCacheStats();
};

////////////////////////////////////
//
// colormodel class with integer ID lookup
//
class colormodel {
private: 
    std::array<std::array<int, 6>, 120> edgemodels;
    int modelId;
    std::array<std::shared_ptr<StatefulColorFunction>, 120> edgeFunctions;
    std::array<int, 120> edgePalettes;
    bool shouldRegister;

    static const int MAX_MODELS = 40;
    static colormodel* modelRegistry[MAX_MODELS];
    static int modelIdRegistry[MAX_MODELS];
    static int numRegisteredModels;

public:
    colormodel(std::array<std::array<int, 6>, 120> importedgemodels, 
               int importId, bool registerModel = true);
    
    colormodel(const colormodel& other);
    
    void registerSelf();
    
    CRGB getcolorfunction(int edgeindex, float position);
    
    void setColorFunction(int edgeindex, int functionId, 
                         int paletteId = 0,
                         const std::vector<FunctionParameter>& params = {});
    
    int getModelId() const { return modelId; }
    
    // Backward compatibility - returns string representation of ID
    String getModelName() const { return String(modelId); }
    
    int getEdgeFunctionIndex(int edgeindex) const {
        if(edgeindex >= 0 && edgeindex < 120) {
            return edgemodels[edgeindex][0];
        }
        return 0;
    }
    
    // Edge permutation methods
    colormodel* applyEdgePermutation(const EdgePermutation& perm, int newId = 0, bool registerModel = true) const;
    colormodel* applyEdgePermutation(const std::array<int, 120>& permArray, int newId = 0, bool registerModel = true) const;
    colormodel* applyEdgePermutationSequence(const int* permIds, int numPerms, int newId = 0) const;
    colormodel* applyEdgePermutationSequence(std::initializer_list<int> permIds, int newId = 0, bool mergeWithOriginal = false) const;
    
    static colormodel* applyEdgePermutation(int modelId, const EdgePermutation& perm, int newId = 0);
    static colormodel* applyEdgePermutation(int modelId, const std::array<int, 120>& permArray, int newId = 0);
    static colormodel* applyEdgePermutation(int modelId, int permId, int newId = 0);
    static colormodel* applyEdgePermutationSequence(int modelId, const int* permIds, int numPerms, int newId = 0);
    static colormodel* applyEdgePermutationSequence(int modelId, std::initializer_list<int> permIds, int newId = 0, bool mergeWithOriginal = false);
    
    static colormodel* mergeModels(const colormodel* model1, const colormodel* model2, int newId);
    static colormodel* mergeModels(int model1Id, int model2Id, int newId);
    
    const std::array<std::array<int, 6>, 120>& getEdgeModels() const { return edgemodels; }
    
    static colormodel** getModelRegistry() { return modelRegistry; }
    static int* getModelIdRegistry() { return modelIdRegistry; }
    static int getNumRegisteredModels() { return numRegisteredModels; }
    static colormodel* findModelById(int id);
    static void printRegistry();
};

void initializefancymodels();

#endif // MODELS_H
