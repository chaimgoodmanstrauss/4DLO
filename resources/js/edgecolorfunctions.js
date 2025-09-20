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

let modelfunctionregistry ={}

//////////////////////////////////////////////////
///
///   Lots of examples
///
//  This set up ensures functions of (x,t) are automatically added to a central
//  registry, to help keep track of what we have. 
//  Functions all are named, and can be composed. 
//  


/////////
//
// a simple color wheel. 
function cyclecolorfunction(position,time,hue0=0, hueW=.1, saturation=1, brightness=1){
    return hsbToRgb(hue0+hueW*Math.sin(time+position*3.1416),saturation,brightness)}

///////////
//


function gaussiancolorfunction(position, time, hue0=0, sigma= .01, range=[0,1],  saturation=1, brightness=1){
 /* 
  var adjustedposition  // putting it in line with the origin at 0
  //Gaussian height: (1/(σ√(2π))) * exp(-(x-μ)²/(2σ²))
  var hue = hue0+(range[1]-range[0])*
      gaussianHeight(position+time-Math.floor(position+time), // %1 is probably correct, as time is >>0, but just in case. ,
      GAUSSIAN_TABLES[variance])// these are precomputed for 
  return hsbToRgb(
    hue,saturation,brightness)
    */
  //RETURN TO THIS
  return [.4,.4,.4]
}

function spikecolorfunction(x,t,hue0=0,colorspread=.3, spacespread = .4, direction=1, saturation=1, brightness = 1){
  // within spacespread/2 of 1/2, the colors are spread about hue0
  // in short:
  var shift = direction*colorspread*(1-Math.min(Math.abs(((x+t)%1)*2/spacespread-.5),1))
  return hsbToRgb( hue0+shift,saturation,brightness)
}




async function example1() {
    const func = new discreteFunction('resources/graphs/testfunc.json');
    await func.waitForLoad();
    
    if (func.isReady()) {
        console.log('testing the loading of discrete function info', func.getInfo());
        const y = func.evaluate(0.3);
        console.log('for which f(0.3) =', y);
    }
}

example1()



/// put the colorfunctions here. 
/// Each has names and a function
/// returning an rgb 
let ourColorFunctionRegistry={}


ourColorFunctionRegistry={...ourColorFunctionRegistry,...{// these can be functions, or dictionaries that include the colorfunction key.
  blank:function(x,t){return [.6,.6,.6,1]},
  huewheel:function(x,t){return hsbToRgb(x+t/5,1,1)},
  colorwheel:function(x,t){return hsbToRgb(x+t/5,1,1)},
  huewheel2:function(x,t){return hsbToRgb2(x+t/5,1,1)},
  throbbingred:function(x,t){
    return hsbToRgb(0,1,1)//something is wrong here
   // 0,1-.5*Math.abs(Math.sin(t/5)), 1-.5*Math.abs(Math.sin(t/5))
  },
  defaultcolorfunction:function(x,t){return hsbToRgb2(x+t/5,1,1)},
  basiccycle:  function(x,t){return cyclecolorfunction(x,t)},
  black:function(x,t){return [0,0,0,1]},
  white:function(x,t){return [1,1,1,1]},
  red:function(x,t){return [1,0,0,1]},
  blue:function(x,t){return [0,0,1,1]},
  green:function(x,t){return [0,1,0,1]},
  yellow:function(x,t){return [1,1,0,1]},
  pulse2:function(x,t){return cyclecolorfunction(x*3,t,0,.22,.1,.9)},
  pulse:function(x,t){return cyclecolorfunction(x,t,.4,1,.5,.7)},
  purplepulse:function(x,t){return gaussiancolorfunction(x,t,.9,".01",.3,.3,1,1)},
  cyanpulse:function(x,t){return gaussiancolorfunction(x,t,.8,".01",.3,.3,.1)},
  redpulse:function(x,t){return gaussiancolorfunction(x,t,0,".01",.3,.3,.2)},
  greenpulse:function(x,t){return gaussiancolorfunction(x,t,.4,".01",.3,.3,.2,1,.5)},
  bluepulse:function(x,t){return gaussiancolorfunction(x,t,.65,".01",.3,.3,.2,1,.7)},
  yellowspikepulse:function(x,t){return spikecolorfunction(x,t/5,0,.2,.2)},
  bluespikepulse:function(x,t){return spikecolorfunction(x,t/5,.6,.3,.4)},
  redspikepulse:function(x,t){return spikecolorfunction(x,t/5,.2,.3,.2,-1)},
  spikepulse4:function(x,t){return spikecolorfunction(x,t/5,.9,.2,.2,1)},
  spikepulse5:function(x,t){return spikecolorfunction(x,t/5,.45,.3,.2,1)},
  spikepulse6:function(x,t){return spikecolorfunction(x,t/5,.75,.4,.2,1)},
}
}

ourColorFunctionRegistry[0]=ourColorFunctionRegistry.red
ourColorFunctionRegistry[1]=ourColorFunctionRegistry.green
ourColorFunctionRegistry[2]=ourColorFunctionRegistry.blue


