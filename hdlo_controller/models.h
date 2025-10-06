////////////////////////////////////////////
//
//
//   models.h
//
// a model organizes the edge and color information;
// for our convenience, a model has a name. 
// Mainly how


#ifndef MODELS_H
#define MODELS_H

#include <FastLED.h>
#include <array>

typedef CRGB (*ColorFunction)(float);

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

  public:
    colormodel(std::array<std::array<int, 6>, 120> importedgemodels, String importname);
    
    // This allows us to return a color given an edge index and a position
    CRGB getcolorfunction(int edgeindex, float position);

    // Add method to set color functions
    void setColorFunction(int index, String name, ColorFunction func);

    String getModelName() const { return modelname; }
};

#endif // MODELS_H