////////////////////////////////////////////
//
//
//   models.h
//
// a model organizes the edge and color information;
// for our convenience, a model has a name. 

#ifndef MODELS_H
#define MODELS_H

#include <FastLED.h>
#include <array>

typedef CRGB (*ColorFunction)(float);

// Forward declaration
class EdgePermutation;

////////////////////////////////////
//
// colorfunctionclass instances do one thing:
// return its colorfunction(position) 
// colorfunction may incorporate other information such as time
// (in some other manner than already linearly encoded in position)
// sound, or other data.
//

class colorfunctionclass{
  private:
    String name;
    ColorFunction colorfunction;

  public:
    // Add default constructor
    colorfunctionclass();
    colorfunctionclass(String inputname, ColorFunction inputcolorfunction);

    CRGB returncolor(float position);
};


class colormodel{
private: 
    std::array<std::array<int, 6>, 120> edgemodels;
    String modelname;
    std::array<colorfunctionclass, 120> colorfunctions;
    bool shouldRegister;  // Flag to control registration

    // Global model registry
    static const int MAX_MODELS = 100;
    static colormodel* modelRegistry[MAX_MODELS];
    static String modelNameRegistry[MAX_MODELS];
    static int numRegisteredModels;

  public:
    colormodel(std::array<std::array<int, 6>, 120> importedgemodels, String importname, bool registerModel = true);
    
    // Helper for registration (public so it can be called after construction)
    void registerSelf();
    
    // This allows us to return a color given an edge index and a position
    CRGB getcolorfunction(int edgeindex, float position);

    // Add method to set color functions
    void setColorFunction(int index, String name, ColorFunction func);

    String getModelName() const { return modelname; }
    
    // Edge permutation methods - these create NEW models
    // By pointer
    colormodel* applyEdgePermutation(const EdgePermutation& perm, String newName = "", bool registerModel = true) const;
    colormodel* applyEdgePermutation(const std::array<int, 120>& permArray, String newName = "", bool registerModel = true) const;
    colormodel* applyEdgePermutationSequence(const String* permNames, int numPerms, String newName = "") const;
    
    // By name (static versions)
    static colormodel* applyEdgePermutation(String modelName, const EdgePermutation& perm, String newName = "");
    static colormodel* applyEdgePermutation(String modelName, const std::array<int, 120>& permArray, String newName = "");
    static colormodel* applyEdgePermutationSequence(String modelName, const String* permNames, int numPerms, String newName = "");
    
    // Model merging - creates NEW model by combining two models
    // By pointer
    static colormodel* mergeModels(const colormodel* model1, const colormodel* model2, String newName);
    
    // By name (overload)
    static colormodel* mergeModels(String model1Name, String model2Name, String newName);
    
    // Get current edge model data (for debugging)
    const std::array<std::array<int, 6>, 120>& getEdgeModels() const { return edgemodels; }
    
    // Static methods to access the global registry
    static colormodel** getModelRegistry() { return modelRegistry; }
    static String* getModelNameRegistry() { return modelNameRegistry; }
    static int getNumRegisteredModels() { return numRegisteredModels; }
    static colormodel* findModelByName(String name);
    static void printRegistry();
};

#endif // MODELS_H