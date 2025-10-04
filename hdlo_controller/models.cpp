#include <FastLED.h>
#include "models.h"

//typedef CRGB (*ColorFunction)(float);

////////////////////////////////////
//
// colorfunctionclass instances do one thing:
// return its colorfunction(position)
// colorfunction may incorporate other information such as time
// (in some other manner than already linearly encoded in position)
// sound, or other data.
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

colormodel::colormodel(std::array<std::array<int, 6>, 120> importedgemodels, String importname)
  : edgemodels(importedgemodels), modelname(importname) {}

CRGB colormodel::getcolorfunction(int edgeindex, float position) {
  // Use the colorfunctionclass object's returncolor method
  return colorfunctions[edgemodels[edgeindex][0]].returncolor(position);
}

void colormodel::setColorFunction(int index, String name, ColorFunction func) {
  if (index >= 0 && index < 120) {
    colorfunctions[index] = colorfunctionclass(name, func);
  }
}
