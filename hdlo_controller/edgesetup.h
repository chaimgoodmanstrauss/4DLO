
#include <math.h>

///// Edge set up

// given a point [a,b,c] in R3 (intended to be along an edge of tubing)
// and other (meant to be the front end of the tube)
// lift to x,y,z,w on the hypersphere

// This is the inverse of the map xyzw -> xyz/(1+w)

// That is, abc -> 1/(1+D)*(2a,2b,2c,1-D) where D = a^2+b^2+c^2

/*
float lifttoS3(float pt[],float spt[]){

  float   a = pt[0];
    float b = pt[1];
    float c = pt[2];
    float delta = a*a+b*b+c*c;
    float w = (1-delta)/(1+delta);
    float x = 2*a/(1+delta), y= 2*b/(1+delta),z=2*c/(1+delta);
    spt[0]=x;spt[1]=y;spt[2]=z;spt[3]=w;
    
}*/

// We next convert from LED coordinates on an edge, which we'll express as a
// number from 0 to 1 (the nth LED on a strand of M LEDs is at (n-1/2)/M )

// In the sculpture there are only a few different kinds of edge, and we tailor this
// function to them.

// Our function is therefore
//          positiononedge(int lednumber, int numberofleds, int edgetype,int sign)

// [We write - for -1 and abbreviate as we can.]



// On a line from (000) to (∞∞∞) there are three classes of straight line;
// We parametrize these together, but in a switch adjust t to run from 0 to 1,
// and then further based on the counted number of LEDs in the actual individual strip.


// 0: from 1  to (1111)/2 upstairs, (000) to  (111)/3 downstairs `
//     (upstairs edge # 95); downstairs (0<t<1/sqrt3)
// 1: from 1111/2 to -111/2, (111)/3 to 111
//     (upstairs edge #39); (1/sqrt(3)<t<sqrt(3))
// 2: from -111 to -∞∞∞, best understood as (111) to some point (AAA),
//      (edge # 64); (sqrt(3)<t<[[ some formula based on the length of the pipe we want]]
const float SOMESCALINGTBD = .3;  // TBD! Need to work this out.

// We choose our other representatives from the circle centered at (011)/2, perp to 100,
// parametrized by c*sqrt(3/2)*(011)/sqrt(2)+s*sqrt(3/2)*(100)+(011)/2.
// where c and s are cosine and sine as we measure counter-clockwise around in theta from the 011 direction.
// On this circle, we are interested in the arcs (-11) to (111) to (001) to (1--)/3 to (---)/3,
// their upstairs counterparts running from (--11) to (-111) to (0100) to (11--) to (1---)
// As the center of the circle is at (011)/2, the vector directions from the center to these
// points are (11(-2))/2; (112)/2; (--2)/2; ((-5/6)(-5/6)(1/3)); ((-5/6)(-5/6)(-1/3))
// The angles between these are the arccosines of the dot products of these, namely:

const float anglea = acos(-1. / 3.);   //~ 2*54.7356°;
const float angleb = acos(1. / 3.);    // ~ 70.5207°;
const float anglec = acos(7. / 9.);    // ~  38.9429°;
const float angled = acos(23. / 27.);  // ~ 31.586°;

// 3: from (--11)/2 to (-111)/2 or (-11) to (111)
// upstairs edge # 74; downstairs (-a/2<theta<a/2)
// 4: from (-111)/2 to I, or (111) to (100)
// upstairs edge # 64;  downstairs (a/2<theta < a/2+b)
// 5: from I to (11--)/2, or (100) to (1--)/3
// upstairs edge # -83 (ie runs in the opposite direction upstairs, as it happens); downstairs (a/2+b<theta<a/2+b+c)
// 6: from (11--)/2 to (1---)/2, or (1--)/3 to (---)/3
// upstairs edge # -27; downstairs (a/2+b+c<theta<a/2+b+c+d)


// NOTE: LEDs are numbered in the human fashion, 1,2,3...

// these are the types of each kind of edge:
const int edgeclasses[120]={4,5,5,5,4,4,5,5,4,5,4,5,4,3,6,3,6,3,1,1,6,2,5,5,4,0,4,2,0,5,1,6,3,1,4,4,0,2,5,4,5,4,5,3,6,5,3,6,4,4,5,5,3,1,1,6,4,3,3,6,6,2,4,5,0,5,2,5,0,4,1,3,6,1,4,4,5,5,4,4,2,0,5,3,3,6,6,2,4,4,5,0,2,5,4,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const int edgedirections[120]={};

float positiononedge(int lednumber, int numberofleds, int edgeindex, int sign) {
  
  int edgeclass = edgeclasses[edgeindex];
  
  float position;
  // this is the value we wish to return, the position upstairs on the edge, represented as a value in (0,1).
  // Each hdlo model returns a color value on the corresponding edge-- here we just use the position.

  // First: convert the index of a physical LED to a value of a parameter t in (0,1);
  float t = (lednumber - .5) / numberofleds;// better distribution
  //float t = (lednumber-1.)/(numberofleds-1.);//easier to debug

  // we next convert t to the appropriate domain, either along a straight segment 000 to ∞∞∞ or an arc on the
  // circle of radius sqrt(3/2) centered at 011/2 perp to 100

  float endpt[4], liftedpt[4];
  endpt[0] = endpt[1] = endpt[2] = endpt[3] = liftedpt[0] = liftedpt[1] = liftedpt[2] = liftedpt[3] = 0;
  float delta;
  float adjt = t;
  switch (edgeclass) {
    case 0: // 1 to (1111)/2 (8 of these)
    case 1: //(1111)/2 to (-111)/2 (8 of these)
    case 2: // (-111)/2 to -1 (8 of these)
    // The coordinates will be |adjt,adjt,adjt|
      switch (edgeclass) {
        case 0:  //1  to (1111)/2
          adjt = t/3.;// adjt in [0,1/3] 
          endpt[0] = 1;
          endpt[1] = endpt[2] = endpt[3] = 0;
          break;  // we transform the interval (0,1) to (0,1/sqrt3)
        case 1:   //from 1111/2 to -111/2
          adjt = 1./3. + 2.*t /3.;
          endpt[0] = .5,endpt[1]  = .5,endpt[2] = .5,endpt[3] = .5;
          break;  //
        case 2:   //from -111 to -∞∞∞
          adjt = 1 + t * SOMESCALINGTBD;
          endpt[0] = -.5;
          endpt[1] = endpt[2] = endpt[3] = .5;
          break;
      }
      // adjt has been adjusted so that it now
      // measures  our position along adj*(1,1,1)=(adjt,adjt,adjt)
      // lifting to:
      delta = 3 * adjt * adjt;
      liftedpt[0] = (1 - delta) / (1 + delta);
      liftedpt[1] = liftedpt[2] = liftedpt[3] = 2 * adjt / (1 + delta);
      break;
    case 3: // (--11)/2 to (-111)/2 (12, the outer cube)
    case 4: //(-111)/2 to I (24, the crosses)
    case 5: //I to (11--)/2 (24, the cupolas)
    case 6: //(11--)/2 to (1---)/2 (12 the inner cuber)

      float a = anglea, a2 = a*.5,  b = angleb, c = anglec, d = angled;

      switch (edgeclass) {
        case 3:  //(-a/2<adjt<a/2)
          adjt = -a2 + t * a;
          //from (--11)/2 to (-111)/2
          endpt[0] = endpt[1] = -.5;
          endpt[2] = endpt[3] = .5;
          break;
        case 4:
          // (-111)/2 to I; (a/2<adjt < a/2+b)
          adjt = a2 + b * t;
          endpt[0] = -.5;
          endpt[1] = endpt[2] = endpt[3] = .5;
          break;
        case 5:  //I to (11--)/2;(a/2+b<adjt<a/2+b+c)
          adjt = a2 + b + c * t;
          endpt[1] = 1;
          endpt[0] = endpt[2] = endpt[3] = 0;
          break;
        case 6:  //(11--)/2 to (1---)/2;(a/2+b+c<adjt<a/2+b+c+d)
          adjt = a2 + b + c + d * t;
          endpt[0] = endpt[1] = .5;
          endpt[2] = endpt[3] = -.5;
          break;
      }

      // adjt now measures around from 011/sqrt2 to 100 etc. therefore we wish to lift:
      float circpt[3];
      circpt[0] = 0 +     sqrt(1.5) * sin(adjt);
      circpt[1] = .5 + sqrt(3.) *.5 * cos(adjt);
      circpt[2] = .5 + sqrt(3.) *.5 * cos(adjt);
      delta = circpt[0] * circpt[0] + circpt[1] * circpt[1] + circpt[2] * circpt[2];
      liftedpt[0] = (1. - delta) / (1. + delta);
      liftedpt[1] = 2. * circpt[0] / (1. + delta);
      liftedpt[2] = 2. * circpt[1] / (1. + delta);
      liftedpt[3] = 2. * circpt[2] / (1. + delta);
      break;
  }

  /*float temppt[3]; 
    temppt[0]=liftedpt[1]/(1+liftedpt[0]);
    temppt[1]=liftedpt[2]/(1+liftedpt[0]);
    temppt[2]=liftedpt[3]/(1+liftedpt[0]);*/
  
  float cosangle = 
      endpt[0] * liftedpt[0] +
      endpt[1] * liftedpt[1] + 
      endpt[2] * liftedpt[2] + 
      endpt[3] * liftedpt[3];

// We express our position as a portion of an angle Pi/3, the length of each of these arcs upstairs.

  position = acos(cosangle) / 3.1416 * 180./60.;//This should be a value between 0 and 1.
 
 // Reverse direction if the sign is negative (-1, presumably)
  if(sign<0){
    position = 1-position;
  }

  //Reverse again if the direction is reversed***

Serial.println(" led "+String(lednumber)+" on edge "+String(edgeindex)+" of class "+ String(edgeclass)+" has position "+String(position,5));
/*Serial.print("   ");
Serial.print("Adjusted t:" + String(adjt));
Serial.print(" The end point (" + String(endpt[0]) + ", " + String(endpt[1]) + ", " + String(endpt[2]) + ", " + String(endpt[3]) + "); The lifted pt: (" + String(liftedpt[0]) + ", " + String(liftedpt[1]) + ", " + String(liftedpt[2]) + ", " + String(liftedpt[3]) + ")");
Serial.print(" lifted from " + String(temppt[0]) + ", " + String(temppt[1]) + ", " + String(temppt[2]) + ")");
Serial.println(" cosine = " +String(cosangle));
*/
  return position;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//
//      Strand Information
//  
// strandtable relates the positions of LEDs on a strand 
// (and more specifically indices in rgbarray)
// to positions x in [0,1] measured by arclength along 
// some edge of the 24-cell. 
// 
// That is, strandtable[maxledsperstrand*strandnumber+lednumber] = {edge type, position}
// 
// To enter this information, we use stranddata. 
// which will have format { //strand1, strand2, ..} where each 
// strand is {segment1, segment2, ...} where each 
// segment is {
//              starting LED (counting like a person, from 1), 
//              edge type (a value 1-96, a const, the index of a specific prototype edge in the 24 cell),
//              relative direction (1= in agreement, -1 = reversed)
//             }
// 

// We define stranddata in ledconstants.h where it is more convenient

const int positionresolution = 10000;
//const int temp = ledsperstrip * numberofpins;
int strandtable[ledsperstrip * numberofpins][2]={{1}}; // strips, then pins; edge index, position in 10,000ths.

void constructstrandtable(){
  //we make a table of every led in every strand, mapping it to an 
  // {edge, position on the edge (a value from 0 to 1)}.
  //If an LED number doesn't correspond to a real
  // LED, it isn't mapped to an edge and its strandata remains initialized to {0,0}.

  
  for(int strandnumber=0;strandnumber<numberofpins;strandnumber++){
    // now walk down the stranddata. Because stranddata is a C array, 
    // it is large enough to hold as many entries as we _might_ need -- which 
    // may be more than the number we _do_ need. In C, the remaining entries 
    // are zeroed out -- we check for this: 
  
    if(stranddata[strandnumber][0][0]!=0){
      // we proceed, otherwise, nothing happens for this strandnumber
      
      int lengthofprevioussegments = 0;

      for(int segmentnumber=0;segmentnumber<maxedgesperstrand;segmentnumber++ ){
        
        // now we go through the segments, again discarding those that are zeroed out:
        if(stranddata[strandnumber][segmentnumber][0]!=0){
          
          // so now we have something: 
          // We can assume that [0]= the length of the segment in LEDS
          // [1]=edgetype and [2]= direction. We fill stranddata with 
          // [0]=position and [1]=edge type

          // We walk from the first LED in the segment

          for(int ledindex = lengthofprevioussegments ; 
            ledindex< lengthofprevioussegments+stranddata[strandnumber][segmentnumber][0];ledindex++){
            strandtable[strandnumber*ledsperstrip+ledindex][0]=
            (int)(positionresolution*  // cast to an integer from 0 to 10000, so the table can be ints.
            positiononedge(
              ledindex-lengthofprevioussegments+1,//led number from 1 to
              stranddata[strandnumber][segmentnumber][0],// the length of the segment in LEDs
              stranddata[strandnumber][segmentnumber][1], // mapping to edge #
              stranddata[strandnumber][segmentnumber][2])); // reversed or not
            strandtable[strandnumber*ledsperstrip+ledindex][1]=stranddata[strandnumber][segmentnumber][1];
          }
          lengthofprevioussegments+=stranddata[strandnumber][segmentnumber][0];
        }// else there's no segment here
      }


    } //else there's no strand here

    }
 }


 ///////////////////////////////
 //
 //
  

void initedgedata(){
 /* int res =100;
  for(int edgetype = 0; edgetype<7; edgetype++){
    for(int lednumber = 0; lednumber<res;lednumber++){
      positiononedge(lednumber,res,edgetype,1);
    }
  }*/
  Serial.println("constructing the strand table...");
    constructstrandtable();
  
  //let's take a look at what we've got:

   for(int i = 0; i<numberofpins; i++){
     Serial.println("pin # "+String(i)+":");
     for(int j = 0; j<ledsperstrip;j++){
       Serial.println("  led: "+String(j)+": "+String(strandtable[i*ledsperstrip+j][0])+" on edge "+String(strandtable[i*ledsperstrip+j][1]));
     }
   }
}
