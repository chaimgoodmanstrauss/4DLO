////////////////////////////////////
//
//   models.h
//
// Color function and model system with factory pattern
//
#ifndef MODELS_H
#define MODELS_H

#include <FastLED.h>
#include <array>
#include <map>
#include <functional>
#include <memory>
#include <vector>

// Forward declarations
struct FunctionParameter;  // Full definition in colorfunctions.h
class IColorFunction;
class ColorFunctionFactory;
class EdgePermutation;

/////////////////////////////////////////
// COLOR FUNCTION FACTORY
//
class ColorFunctionFactory {
private:
    std::map<String, std::function<IColorFunction*()>> creators;
    static ColorFunctionFactory* instance;
    
    ColorFunctionFactory() {}
    
public:
    static ColorFunctionFactory& getInstance() {
        if (instance == nullptr) {
            instance = new ColorFunctionFactory();
        }
        return *instance;
    }
    
    void registerFunction(const String& name, std::function<IColorFunction*()> creator) {
        creators[name] = creator;
    }
    
    IColorFunction* create(const String& name);
    
    bool hasFunction(const String& name) const {
        return creators.find(name) != creators.end();
    }
    
    void listFunctions();
};

////////////////////////////////////
//
// colormodel class
//
class colormodel {
private: 
    std::array<std::array<int, 6>, 120> edgemodels;
    String modelname;
    std::array<std::unique_ptr<IColorFunction>, 120> edgeFunctions;
    std::array<String, 120> edgePalettes;
    bool shouldRegister;

    static const int MAX_MODELS = 100;
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
    
    static colormodel* applyEdgePermutation(String modelName, const EdgePermutation& perm, String newName = "");
    static colormodel* applyEdgePermutation(String modelName, const std::array<int, 120>& permArray, String newName = "");
    static colormodel* applyEdgePermutationSequence(String modelName, const String* permNames, int numPerms, String newName = "");
    
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
