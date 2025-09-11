
/////////////////
//
//
//  Our color function
//  All this needs to be is a function that
// 	takes in a time, a value in [0,1], a small index, and whether to reflect
//

// replace these as we wish.


/*
function testcolorfunction(x,index=0,reflectQ=false,time=Date.now()* 0.001 ){
	var color
	var xx = x
	if(reflectQ){
		xx=1-Math.abs(1-2*x)
	}
	switch(index)
	{
		case 0: color = [ourFrequencyAnalyzer.analyze(300,500).dbLevel/60,
		ourFrequencyAnalyzer.analyze(500,900).dbLevel/60,
		ourFrequencyAnalyzer.analyze(900,3000).dbLevel/60,1]; break
		case 1: color = [0,xx,1-x,1]; break
		case 2: color = [0,.5+.5*Math.sin(6.14*5*xx+time),0,1]; break
		case 3: color = [0,.5+.5*Math.cos(6.14*5*xx+6*time),.5+.5*Math.cos(6.14*5*xx+7*time),1]; break
	}
	return color
}
*/



function hsbToRgb3(h, s, b) {
  // Handle grayscale case (no saturation)
  if (s === 0) {
    return [b, b, b];
  }
  
  // Properly normalize hue to 0-1 range
  h = ((h % 1) + 1) % 1;
  
  // Remap hue to expand green range
  // Standard green is roughly 0.167-0.5 (120°-180° expanded to 240°)
  if (h >= 0.167 && h <= 0.667) {
    // Expand this range (0.167-0.667 = 0.5 of input) to occupy more of the wheel
    // Map to 0.167-0.75 (expanding green/cyan region)
    h = 0.167 + (h - 0.167) * 1.166; // stretch factor of ~1.166
  } else if (h > 0.667) {
    // Compress the remaining range (0.667-1.0) into smaller space
    h = 0.75 + (h - 0.667) * 0.75; // compress remaining colors
  }
  
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
    default: return [b, t, p]; // Fallback
  }
}


function hsbToRgb2(h, s, b) {
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


function hsbToRgb(h, s, b) {
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


function testcolorfunction(x,index=0,reflectQ=false,time=Date.now()* 0.001 ){
	var color
	var xx = x
	if(reflectQ){
		xx=1-Math.abs(1-2*x)

	}

	var rgb;
	switch(index)
	{
		case 0:  
			color = hsbToRgb(.5+.06*Math.sin(time+xx*3.141),1,1); break
		case 1: color = hsbToRgb(.7+.06*Math.sin(time+xx*3.141),1,1); break
		case 2: color = hsbToRgb((.95+.06*Math.sin(1*time+xx*3.141)),1,1);break//.25+.5*Math.sin(1*time+(xx+.5)*3.141)); break;
		case 3: color = hsbToRgb(.25+.06*Math.sin(time+xx*3.141),1,.5+.5*
			(.6+.4*Math.sin(2*time))*
			(Math.sqrt(1+Math.sin(20*time+xx*3.141*10)))); break
	}
	return color
}


function cyclecolorfunction(position,time,  hue0=0,speed=1/*cycle per second*/, inneramp=.1, saturation=1, brightness = 1){
    return hsbToRgb(hue0+inneramp*Math.sin(speed*time+position*3.1416),saturation,brightness);
}

initGaussianTables()

function gaussiancolorfunction(position, time, hue0=0, variance=".01", timeshift=0,speed = 1,inner=.4, saturation=1, brightness=1){
  var temp = Math.floor(position+speed*(time-timeshift))
  var hue = hue0+inner*gaussianHeight(position+speed*(time+timeshift)-temp,
    GAUSSIAN_TABLES[variance])
  return hsbToRgb(
    hue,saturation,brightness)
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
        console.log('Function info:', func.getInfo());
        const y = func.evaluate(0.3);
        console.log('f(0.3) =', y);
    }
}

example1()


function edgecolorfunction(x,modeldata,time=Date.now()* 0.001, colorprogram=defaultcolorprogram){
        var rgb; 
        var position; 

// modeldata is [0] = index of color
//              [1] = direction 
//              [2] = xstretch, if defined
//              [3] = x0 if defined
//              [4] = time stretch, if defined
//              [5] = t0 if defined


        // if the direction=modeldata[1] is negative, reverse the colors 
        if(modeldata[1]==-1){
          position = 1-x}
        // if it's positive, keep it as it is
        else if(modeldata[1]==1){
          position = x}
        // if it's 0, reverse it in the middle. 
        else{
          position = 2*Math.abs(.5-x)}
        // we could add a NaN option to reverse that
        
        var xscale = 1, tscale = 1, x0 = 0, t0=0
        // If there is further information in the modeldata, let's use it. 
        switch(modeldata.length){
          case 6: // we have a t0
            t0=modeldata[5]
          case 5: // we have a tscale
            tscale = modeldata[4]
          case 4: // we have an x0
            x0= modeldata[3]
          case 3:// an xscale
            xscale = modeldata[2]
        }
        
        rgb =colorfunctions[colorprogram[modeldata[0]]](position*xscale+x0, time*tscale+t0)


		return rgb;
	}


/// put the colorfunctions here. 
/// Each has names and a function
/// returning an rgb 


const colorfunctions={
  blank:function(x,t){return [.6,.6,.6,1]},
  basiccycle:  function(x,t){return cyclecolorfunction(x,t)},
  black:function(x,t){return [0,0,0,1]},
  white:function(x,t){return [1,1,1,1]},
  red:function(x,t){return [1,0,0,1]},
  huewheel:function(x,t){return hsbToRgb3(x+t/5,1,1)},
  huewheel2:function(x,t){return hsbToRgb2(x+t/5,1,1)},
  throbbingred:function(x,t){
    return hsbToRgb(0,1,1)
   // 0,1-.5*Math.abs(Math.sin(t/5)), 1-.5*Math.abs(Math.sin(t/5))
   
  },
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

/// A color program is an array of names, a look up table of colorfunctions

var defaultcolorprogram=['blank','pulse2','pulse','purplepulse','basiccycle','cyanpulse']

const pulsingcolors=['blank','redpulse','greenpulse','bluepulse','purplepulse','cyanpulse']

const solidcolors = ['blank','red','green','blue','yellow','black','white']

const testingcolors = ['blank','redpulse','green','blue','blank','blank','blank']

 defaultcolorprogram = 
['blank',
  'huewheel2','huewheel',
  'redpulse','greenpulse','bluepulse','purplepulse',
  'redspikepulse','bluespikepulse', 'yellowspikepulse','spikepulse4',
  'spikepulse5','spikepulse6',
  'throbbing red',
  'red','green','blue']


