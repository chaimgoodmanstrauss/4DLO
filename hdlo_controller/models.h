////////////////////////////////////
//
//   models.h
//
// Color function and model system with factory pattern
//
#ifndef MODELS_H
#define MODELS_H

 const bool AUDIODEBUGGING = false; // this is just upstream of everything else.

#include <FastLED.h>
#include <array>
#include <map>
#include <functional>
#include <memory>
#include <vector>

// Forward declarations
struct FunctionParameter;  // Full definition in colorfunctions.h
class StatefulColorFunction;
class ColorFunctionFactory;
class EdgePermutation;

/////////////////////////////////////////
// COLOR FUNCTION FACTORY with caching
// Manages shared instances of color functions to reduce memory fragmentation
// and ensure consistency (e.g., Perlin noise looks the same across all segments)
//
class ColorFunctionFactory {
private:
    std::map<String, std::function<StatefulColorFunction*()>> creators;
    std::map<String, std::shared_ptr<StatefulColorFunction>> cache;  // Shared instance cache
    static ColorFunctionFactory* instance;
    
    ColorFunctionFactory() {}
    
public:
    static ColorFunctionFactory& getInstance() {
        if (instance == nullptr) {
            instance = new ColorFunctionFactory();
        }
        return *instance;
    }
    
    void registerFunction(const String& name, std::function<StatefulColorFunction*()> creator) {
        creators[name] = creator;
    }
    
    // Get or create a shared instance from cache (by function name only - deprecated)
    std::shared_ptr<StatefulColorFunction> getShared(const String& name);
    
    // Get or create a shared instance with full configuration (PREFERRED)
    std::shared_ptr<StatefulColorFunction> getSharedConfigured(
        const String& name,
        const String& paletteName = "",
        const std::vector<FunctionParameter>& params = {});
    
    // Legacy method - creates unique instance (for backwards compatibility)
    StatefulColorFunction* create(const String& name);
    
    // Clear all cached instances (useful for memory management)
    void clearCache();
    
    // Clear specific function from cache
    void clearFunction(const String& name);
    
    bool hasFunction(const String& name) const {
        return creators.find(name) != creators.end();
    }
    
    void listFunctions();
    void printCacheStats();
};

////////////////////////////////////
//
// colormodel class
//
class colormodel {
private: 
    std::array<std::array<int, 6>, 120> edgemodels;
    String modelname;
    std::array<std::shared_ptr<StatefulColorFunction>, 120> edgeFunctions;  // Changed to shared_ptr
    std::array<String, 120> edgePalettes;
    bool shouldRegister;

    static const int MAX_MODELS = 40;
    static colormodel* modelRegistry[MAX_MODELS];
    static String modelNameRegistry[MAX_MODELS];
    static int numRegisteredModels;

public:
    colormodel(std::array<std::array<int, 6>, 120> importedgemodels, 
               String importname, bool registerModel = true);
    
    colormodel(const colormodel& other);
    
    void registerSelf();
    
    CRGB getcolorfunction(int edgeindex, float position);
    
    void setColorFunction(int edgeindex, const String& functionName, 
                         const String& paletteName = "",
                         const std::vector<FunctionParameter>& params = {});
    
    String getModelName() const { return modelname; }
    
    int getEdgeFunctionIndex(int edgeindex) const {
        if(edgeindex >= 0 && edgeindex < 120) {
            return edgemodels[edgeindex][0];
        }
        return 0;
    }
    
    // Edge permutation methods
    colormodel* applyEdgePermutation(const EdgePermutation& perm, String newName = "", bool registerModel = true) const;
    colormodel* applyEdgePermutation(const std::array<int, 120>& permArray, String newName = "", bool registerModel = true) const;
    colormodel* applyEdgePermutationSequence(const String* permNames, int numPerms, String newName = "") const;
    colormodel* applyEdgePermutationSequence(std::initializer_list<String> permNames, String newName = "", bool mergeWithOriginal = false) const;
    
    static colormodel* applyEdgePermutation(String modelName, const EdgePermutation& perm, String newName = "");
    static colormodel* applyEdgePermutation(String modelName, const std::array<int, 120>& permArray, String newName = "");
    static colormodel* applyEdgePermutation(String modelName, String permName, String newName = "");
    static colormodel* applyEdgePermutationSequence(String modelName, const String* permNames, int numPerms, String newName = "");
    static colormodel* applyEdgePermutationSequence(String modelName, std::initializer_list<String> permNames, String newName = "", bool mergeWithOriginal = false);
    
    static colormodel* mergeModels(const colormodel* model1, const colormodel* model2, String newName);
    static colormodel* mergeModels(String model1Name, String model2Name, String newName);
    
    const std::array<std::array<int, 6>, 120>& getEdgeModels() const { return edgemodels; }
    
    static colormodel** getModelRegistry() { return modelRegistry; }
    static String* getModelNameRegistry() { return modelNameRegistry; }
    static int getNumRegisteredModels() { return numRegisteredModels; }
    static colormodel* findModelByName(String name);
    static void printRegistry();
};

// Initialize fancy model variations using edge permutations
void initializefancymodels();

#endif // MODELS_H
