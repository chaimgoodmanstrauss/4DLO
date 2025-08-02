//////////////////////////////////////////////
/////
/////    Miscellaneous general utilities
/////
/////

////  

////  First, many functions for objects and arrays
//// Second, many math shortcuts

//////////////////////////////////////////////
/////
/////    Functions for strings and arrays
/////
/////

//
//  convenient check if variable is defined 
//
function isDefined(x){
	return !(x === null);
}

//
//  convenient check if function is defined 
//
function isFunction(func){
  return (typeof func === 'function');
}


//
//  return value if value is defined
//  return defaultValue otherwise 
//
function getParam(value, defaultValue){
	
	if(isDefined(value)) 
		return value;
	else 
		return defaultValue;
}



// produce a range:
const rangearray = (a,b) => [...Array(b+1).keys()].slice(a)

const range = (n) => [...Array(n).keys()].slice(0)



function arrayEqualArray(a,b){
	if(Array.isArray(a)){
		if(Array.isArray(b)){
			var matchSoFarQ = (a.length==b.length);
			var i=0;
			while(matchSoFarQ && i<a.length)
			{
				matchSoFarQ = (arrayEqualArray(a[i],b[i]));
				i++;
			}
			return matchSoFarQ;
		}
		else return false;
	}
	else if (Array.isArray(b)){return false;}
	else return (a==b);
}

function arrayInArray(outer,inner){
	if(Array.isArray(outer)){
		if(Array.isArray(inner)){
			var foundQ = false;
			var i = 0;
			while(!foundQ && i<outer.length){
				foundQ = arrayEqualArray(inner, outer[i]);
				i++;
			}
			return foundQ
		}
		else {return outer.includes(inner)}
	}
	else return false
}

function arrayReverse(a){
	var revArray=[];
	for(i=0;i<a.length; i++){
		revArray.unshift(a[i])
	}
	return revArray
}




////////////////////////////////
//
//
//


function arraySort(arr){
  // presumes that the elements of array have lessThanEqualTo and equalTo defined.
  if (arr.length==0) return;
  let outarr=[arr[0]];
  let inarr = arr.slice(1);
  let start=0,mid=0,end=0,next,doneQ;
  
  while(inarr.length>0){
    doneQ=false;
    next = inarr.pop();
    start = 0; end = outarr.length-1; 
    if(next.lessThanEqualTo(outarr[start]))
    { doneQ = true; 
      outarr.unshift(next);}
    else if (outarr[end].lessThanEqualTo(next)){
      doneQ = true;
      outarr.push(next);
    }
    
    // at this moment forward, we may be confident that either we are done,
    // or start < next < end
    
    // In the special case that start = end = 0 (and outarr = [arr[0]])
    // we check which side to insert next.
    
    while(!doneQ){
      
      if(start==end){
        //ordinarily this should 
      }
      if(start+1==end){
        // if so the only possibility is that next should be inserted between start and end
        doneQ = true;
        outarr.splice(end, 0, next);
      }
      
      // for start + 1 < end, we have start < mid < end 
      mid =  (end+start-((end+start)%2))/2;// floor (e+s)/2 
      
      
      if(next.equalTo(outarr[mid])){
        // success; just splice in next at mid
        doneQ=true;
        outarr.splice(mid, 0, next);
      }
      else if(next.lessThan(outarr[mid])){
        // then start<next<mid; keep going and take:
        end=mid; 
      }
      else{
        // then outarr[mid].lessThan(next) and 
        // mid < next < end ; keep going and take
        start=mid;
        }
    }
    
    // let's take a look at the results:
   /* let string = "next"+next.toString()+"\n"+
    "mid"+mid.toString()+"\n";
    outarr.map(x=>(string=string+x.toString()+"\n"))
    console.log(
      string+"\n\n")*/
  }
  return outarr;
}











function objectToString(object, forMathematicaQ=false,prec = 4){
	// we'll assume we are only dealing with arrays, objects, NaN, null, undefined, number, boolean
	var out="";
	var C="{",D="}"; 
	var type = typeof object;
	if (object == null){type = "null"}
	if (Array.isArray(object)){type = "array";if(!forMathematicaQ){C="[";D="]";}
	}
//	if (object instanceof complexN){type = "complexN";}
	switch(type){
		case "complexN": return object.toString(forMathematicaQ,prec); break;
		case "boolean": if(forMathematicaQ){if(object){return "True"}else {return "False"}; break;}
			else{if(object){return "T"}else {return "F"}}; break;
		case "number": 
			if (isNaN(object)){return "NaN";}
			else if(Number.isInteger(object)){return object.toString();}
			else{return object.toFixed(prec)}; break;
		case "undefined": return "undefined"; break;
		case "null": return "null"; break;
		case "string": return object; break;
		case "array":
			var out=C, cnt=0;
			object.forEach(function(element){cnt++; out+=objectToString(element,forMathematicaQ,prec)+","})
			if(cnt>0){out=out.slice(0,-1);}
			out+=D;
			return out; break;
		case "object": 
			var out = C, cnt = 0;
			Object.keys(object).forEach(function(key){
				cnt++; if(forMathematicaQ){
					out+=C+objectToString(key,forMathematicaQ)+","+objectToString(object[key],forMathematicaQ)+D+",";
				} 
				else 
					{out+=objectToString(key,forMathematicaQ)+":"+objectToString(object[key],forMathematicaQ)+","}
			})
			if(cnt>0){out=out.slice(0,-1);}
			out+=D;
			return out; break;
		default: return "";
	}
}



function arrayToString(array,forMathematicaQ=false, prec=4){
	var out="";
    var C,D; if(forMathematicaQ){C="{"; D="}";} else {C="[";D="]";}
	if (typeof array === "boolean"){
		if(forMathematicaQ){if(array){out+="True"}else{out+="False"}}
		else{if(array){out+="true"}else{out+="false"}}
	}
	else if(Array.isArray(array)){
    	out=C;
    	var cnt = 0;
		array.forEach(function(element){cnt++;out+=arrayToString(element,forMathematicaQ)+","})
		if(cnt>0){out = out.slice(0,-1);}
    	out+=D}
	else if(isNaN(array))
    	{out+=array.toString()}
    else if(Number.isInteger(array))
        {out+=array.toString()}
    else {out+=array.toFixed(prec)}
	return out
}


//
// convert float array into a string with given precision, with line breaks
//
function iArrayToString(f, precision){
	var s = "[";
	var slen = 80;
	for(var i = 0; i < f.length; i++){
		s += f[i].toFixed(precision);
		if(i < f.length-1) s += ", ";
		if(s.length - slen > 80){ 
			s += "\n";
			slen += s.length;
		}
	}
	s += "]";
	return s;
}


function splanesToString(t, precision){
  if(!isDefined(t))
    return '[undefined]';
	var s = "[";
	for(var i = 0; i < t.length; i++){
		s += splaneToString(t[i], precision);
	}
	return s+']';  
}

function transformToString(t, precision){
	
	return splanesToString(t,precision);
  
}



//////////////////////////////////////////////
/////
/////    Math shortcuts
/////
/////



const PI = Math.PI ;
const TPI = 2*PI;
const HPI = PI/2;
const TORADIANS = PI/180.;
//const EPSILON = 2*Number.EPSILON
const EPSILON = 1e-13;
const SHORTEPSILON = .0000001; 
const SHORTEREPSILON = .0001;  

// for convenience; could convert to, say, Stampfli's Fast methods

function log(x){return Math.log(x)}
function cos(a){return Math.cos(a)}
function sin(a){return Math.sin(a)}
function sec(a){return Math.sec(a)}
function csc(a){return Math.csc(a)}
function tan(a){return Math.tan(a)}
function cot(a){return 1/(Math.tan(a))}
function cosh(a){return Math.cosh(a)}
function tanh(a){return Math.tanh(a)}
function acosh(a){return Math.acosh(a)}
function asin(a){return Math.asin(a)}
function atan(a){return Math.atan(a)}
function atan2(a){return Math.atan2(a)}
function asinh(a){return Math.asinh(a)}
function atanh(a){return Math.atanh(a)}
function sinh(a){return Math.sinh(a)}
function coth(a){return 1/(Math.tanh(a))}
function abs(a) {return Math.abs(a)}
function sqrt(a){return Math.sqrt(a)}
function exp(x){return Math.exp(x)}
function max(x,y){return Math.max(x,y)}
function mod(a,b){
  if(a<0){return (a%b)+abs(b)}
  else return a%b}
function random(){return Math.random()}
function floor(a){return Math.floor(a)}

