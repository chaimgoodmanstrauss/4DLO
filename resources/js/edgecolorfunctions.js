/////////////////////////////////
///
//    Some color conversions 
//
//  Not really clear which of these is best; 
//  can experiment with in the cycle command.
//  Two of these are LLM generated. 



//adjusted and better
function hsbToRgb(h, s, b) {
  // Handle grayscale case (no saturation)
  if (s === 0) {
    return [b, b, b];
  }
  
  if (h>0){h=h%1}else
  {h=1+(h%1)}

  //h= h - .2*Math.tanh(.5*Math.sin(3*3.141*h)-.7)-.12087
  
  //if (h>0){h=h%1}else
  //{h=1+(h%1)}
  
   h = h-(Math.sin(6*3.141*h))/30 
  // this decreases CMY and increases RGB in the colorwheel



  // Convert hue to 0-6 range and find which sector we're in
  const hue = (h) * 6;
  const sector = Math.floor(hue);
  const fractional = hue - sector;
  
  // Calculate intermediate values
  const p = b * (1 - s);
  const q = b * (1 - s * fractional);
  const t = b * (1 - s * (1 - fractional));
  
  // Determine RGB based on which sector of the color wheel
  switch (sector % 6) {
    case 0: return [b, t, p]; // Red to Yellow
    case 1: return [q, b, p]; // Yellow to Green
    case 2: return [p, b, t]; // Green to Cyan
    case 3: return [p, q, b]; // Cyan to Blue
    case 4: return [t, p, b]; // Blue to Magenta
    case 5: return [b, p, q]; // Magenta to Red
  }
}


function hsbToRgbold(h, s, b) {
  // Handle grayscale case (no saturation)
  if (s === 0) {
    return [b, b, b];
  }
  
  // Properly normalize hue to 0-1 range
  h = ((h % 1) + 1) % 1;

  
  
  // Convert hue to 0-6 range and find which sector we're in
  const hue = h * 6;
  const sector = Math.floor(hue);
  const fractional = hue - sector;
  
  // Calculate intermediate values
  const p = b * (1 - s);
  const q = b * (1 - s * fractional);
  const t = b * (1 - s * (1 - fractional));
  
  // Determine RGB based on which sector of the color wheel
  switch (sector) {
    case 0: return [b, t, p]; // Red to Yellow
    case 1: return [q, b, p]; // Yellow to Green
    case 2: return [p, b, t]; // Green to Cyan
    case 3: return [p, q, b]; // Cyan to Blue
    case 4: return [t, p, b]; // Blue to Magenta
    case 5: return [b, p, q]; // Magenta to Red
    default: return [b, t, p]; // Fallback (shouldn't happen)
  }
}





////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////
////  Color Functions

//
//  Each color function is a function that
// 	takes in a time, a value in [0,1], a small index, 
//     and whether to reflect
//  
// For more flexibility, the optional parameters are 
// in a dictionary, named options.

/*let colorFunctionRegistry={}

class colorfunction{
  
  constructor(options){
    //we expect, at minimum, a function of (x,t) returning [r,g,b] or [r,g,b,a]
    // each between 0 and 1;

    // We can also allow optional further parameters (eg also functions of time)
    // but that can be controlled 
    
    if(options.colorfunction){this.colorfunction=options.colorfunction}
    else{hsbToRgb(time+position*3.14159,1,1)}
    
    if(options.name){this.name = options.name}
    else{this.name = 'color function '+Object.keys(colorFunctionRegistry).length}

    if(options.functionparameters){this.functionparameters=options.functionparameters}
    else{this.functionparameters={}}
    
    colorFunctionRegistry[this.name]=this
  }

}*/



//////////////////////////////////////////////////
///
///   Lots of examples
///
//  This set up ensures functions of (x,t) are automatically added to a central
//  registry, to help keep track of what we have. 
//  Functions all are named, and can be composed. 
//  

/*
new colorfunction({
  colorfunction:
  function(x,t,saturation, brightness){return hsbToRgb(hue0+hueW*Math.sin(time+position*3.1416),saturation,brightness)},
  parameters:{saturation:1,brightness:1}
})
*/

/////////
//
// a simple color wheel. 
function cyclecolorfunction(position,time,options={})
  {var hue0=0, hueW=1, saturation=1, brightness = 1
    //TBD turn this into something else. 
    if(options.hue0){hue0=options.hue0}
    if(options.hueW){hueW=options.hueW}
    if(options.saturation){saturation=options.saturation}
    if(options.brightness){brightness=options.brightness}
    
    return hsbToRgb(hue0+hueW*Math.sin(time+position*3.1416),saturation,brightness)}

///////////
//
// Template functions 
// 

//of one variable

//function gaussian(x,bottom=0,height=1,sigma = .01)

function gaussiancolorfunction(position, time, hue0=0, hueW=1, sigma= .01,  saturation=1, brightness=1){
  var x = position, t = time%1
  var color = hsbToRgb(hue0+hueW*(Math.exp(-(x+t)*(x+t)/sigma/sigma/2))/sigma/2.5,
  saturation,brightness)
  // scale and shift x and t in the application of gaussiancolorfunction.
  return  color
}

function spikecolorfunction(x,t,hue0=0,colorspread=.3, spacespread = .4, direction=1, saturation=1, brightness = 1){
  // within spacespread/2 of 1/2, the colors are spread about hue0
  // in short:
  var shift = direction*colorspread*(1-Math.min(Math.abs(((x+t)%1)*2/spacespread-.5),1))
  return hsbToRgb( hue0+shift,saturation,brightness)
}

// this loads a function of one variable as a 
async function loadbasefunction(filename,functionname='', filepath = 'resources/graphs/')
{ const func = new discreteFunction('resources/graphs/testfunc.json');
    
    await func.waitForLoad();
    return function(x){return func.evaluate(x)}
}





/// put the colorfunctions here. 
/// Each has names and a function
/// returning an rgb 
let ourColorFunctionRegistry={}


//// For some reason, this doesn't work.

function registercolorfunction(name, rgbfunctionofxt){
    ourColorFunctionRegistry[name]=rgbfunctionofxt
}

ourColorFunctionRegistry={...ourColorFunctionRegistry,...{// these can be functions, or dictionaries that include the colorfunction key.
  
  generic:function(x,t,f,options){return f(x,t,options)},
  // TBD this is why we need to abstract this. Want to be able to add and manage
  // these on the fly. 

  blank:function(x,t){return [.6,.6,.6,1]},

  // some wheels
  huewheel:function(x,t){return hsbToRgb(x+t/5,1,1)},
  
  // using the cycle function 
  basiccycle:  function(x,t){return cyclecolorfunction(x,t)},
  pulse2:function(x,t){return cyclecolorfunction(x*3,t,{hue0:0,hueW:.22,saturation:.1,brightness:.9})},
  pulse:function(x,t){return cyclecolorfunction(x,t,{hue0:.4,hueW:1,saturation:.5,brightness:.7})},
  
  // basic colors 
  black:function(x,t){return [0,0,0,1]},
  darkgray:function(x,t){return [.1,.1,.1,1]},
  gray:function(x,t){return [.6,.6,.6,1]},
  lightgray:function(x,t){return [.8,.8,.8,1]},
  white:function(x,t){return [1,1,1,1]},
  red:function(x,t){return [1,0,0,1]},
  blue:function(x,t){return [0,0,1,1]},
  green:function(x,t){return [0,1,0,1]},
  yellow:function(x,t){return [1,1,0,1]},
  cyan:function(x,t){return[0,1,1,1]},
  orange:function(x,t){return[1,.5,0,1]},
  purple:function(x,t){return[.6,0,1,1]},

  // using the gaussian 
  purplepulse:function(x,t){return gaussiancolorfunction(x,t,.9,".01",.3,.3,1,1)},
  cyanpulse:function(x,t){return gaussiancolorfunction(x,t,.8,".01",.3,.3,.1)},
  redpulse:function(x,t){return gaussiancolorfunction(x,t,0,".01",.3,.3,.2)},
  greenpulse:function(x,t){return gaussiancolorfunction(x,t,.4,".01",.3,.3,.2,1,.5)},
  bluepulse:function(x,t){return gaussiancolorfunction(x,t,.65,".01",.3,.3,.2,1,.7)},
  
  crazy:function(x,t){
    return cyclecolorfunction(x,Math.sin(t),
        {hue0:.4,hueW:1,saturation:(Math.sin(.232*x+2*t)),brightness:.7})},

  pulse:function(x,t){
    return cyclecolorfunction(x,t,
      {hue0:.4,hueW:1,saturation:.5,brightness:.7})},
  
  // draw some. 

  yellowspikepulse:function(x,t){return spikecolorfunction(x,t/5,0,.2,.2)},
  bluespikepulse:function(x,t){return spikecolorfunction(x,t/5,.6,.3,.4)},
  redspikepulse:function(x,t){return spikecolorfunction(x,t/5,.2,.3,.2,-1)},
  spikepulse4:function(x,t){return spikecolorfunction(x,t/5,.9,.2,.2,1)},
  spikepulse5:function(x,t){return spikecolorfunction(x,t/5,.45,.3,.2,1)},
  spikepulse6:function(x,t){return spikecolorfunction(x,t/5,.75,.4,.2,1)},
}
}
/*
ourColorFunctionRegistry[0]=ourColorFunctionRegistry.red
ourColorFunctionRegistry[1]=ourColorFunctionRegistry.green
ourColorFunctionRegistry[2]=ourColorFunctionRegistry.blue
*/

var temp=0
const defaultcolorfunctions=[
  "huewheel",
 
  //"crazy",
   "huewheel",
  "bluespikepulse",
  "yellowspikepulse",
  "purplepulse",
  "redspikepulse",
  "spikepulse4",
  "spikepulse5",
  "spikepulse",
  "basiccycle",
   "red",
  "green",
  "huewheel",
  "blue",
  "cyan",
  "purple",
  "yellow",
  "white",

].map(name=>{
  ourColorFunctionRegistry[temp]=
    ourColorFunctionRegistry[name];
    temp++;
  return ourColorFunctionRegistry[name]
})
