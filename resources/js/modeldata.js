
///////////////////////////////////////
///////////////////////////////////////
//
//  Various Color Models of the 24-cell. 
//
//
//
// Each "model" consists of a list of 24 mesh coloring functions, f:([0,1]x(R^+))-> RGB
// taking a position x, 0≤x≤1 and a time t>0, and returning a color value. 
// 
// These models are generated using the modeldata that we outline 
// 
// These functions are parametrized by: 
// 		* some canonical function F:[0,1]xR->RBG, taking position and time to RBG. 
//			These may be further parametrized, eg in cosets by some color shift or style change.
//		* a value +1, -1, 0: If +1, f = F; if -1, f(x)=F(1-x); else f(x)=F(2|1/2-x|) 
// 			(or if there's any value to it, can add an option for F(1-2|1/2-x|))
//	
//  	



//////////////////////////////////////////////////////////
///
/// The Models 

// For a physical sculpture we will need the following information:

// Given a strand and the index of an LED upon it, return a color
// Within the Arduino code, we will have pre-computed tables that convert 
// [strand,index] to [edgeindex,x] where 0≤x≤1. 

// In this code, a "model" then, will be an array of values tied to a pre-computed and 
// organized list of edges. 


////////////////////////
///
/// Vertex Models
/// For now, this is it:
///


const standardverts =[ //indices from the list someplace
    [new quat(1.0001,.0001,.001,0).normalize()/*22*/,"darkgray"],
    [qI/*5*/,"red"],
    [qJ/*3*/,"green"],
    [qK/*1*/,"blue"]]

    // this is for a more general framework: 

const vertgroup =makegroup([new qAction(qI,qOne),
			new qAction(qW,qOne)],"Oxone").groupElements;

const vertices = vertgroup.map(m=>{return (m.acton(qOne))})

const vertexmaterials = [mats[1],mats[11],mats[14],mats[22]]

// Here are the vertices in order
/*[
0 +0i +0j –1k     // -K     
0 +0i +0j +1k     // K
0 +0i –1j +0k     // -J
0 +0i +1j +0k      // J
0 –1i +0j +0k      // -I
0 +1i +0j +0k      // I
.5 +.5i +.5j +.5k
–.5 –.5i –.5j –.5k
.5 +.5i +.5j –.5k
–.5 –.5i –.5j +.5k
.5 +.5i –.5j +.5k
–.5 –.5i +.5j –.5k
.5 +.5i –.5j –.5k
–.5 –.5i +.5j +.5k
.5 –.5i +.5j +.5k
–.5 +.5i –.5j –.5k
.5 –.5i +.5j –.5k
–.5 +.5i –.5j +.5k
.5 –.5i –.5j +.5k
–.5 +.5i +.5j –.5k
.5 –.5i –.5j –.5k
–.5 +.5i +.5j +.5k
1 +0i +0j +0k         // 1
–1 +0i +0j +0k       // -1
]*/


// just for the printing functions here:
function strfrom(n){
    switch(Math.round(100*n)){
        case 100:{return "p"}; break;
        case -100:{return "m"}; break;
        case 0:{return "z"}; break;
        case -0:{return "z"}; break;
        case -50:{return "m"}; break;   
        case 50:{return "p"}; break;
        case -71:{return "m"}; break;   
        case 71:{return "p"}; break;
        default :{return n.toString()}; break;
        }
}

function printvert(q){
    return strfrom(q.r)+strfrom(q.i)+strfrom(q.j)+strfrom(q.k)
}

const vertexids = vertices.map(v=>printvert(v))

function vertexindexpermutation(aqaction){
     
    return vertgroup.map(
        m=>{return vertexids.findIndex(
            v=>v==printvert(aqaction.inverse().composeon(m).acton(qOne)))})
            // WHY inverse?

        }
///////////////



var qone =qOne.positivize() // TBD: is qOne (still) messed up? What was the issue?


var edgebase0 = qOne.positivize(), edgebase1=  new quat(.5,.5,.5,.5);

// positioning the edges in a canonical way:

var edgegroupgen1 = new qAction(new quat(sqrt(.5),sqrt(.5),0,0),new quat(sqrt(.5),sqrt(.5),0,0))
    
var edgegroupgen2 = new qAction(qW,qOne)

const edgegroup = makegroup([edgegroupgen1,edgegroupgen2
]).groupElements


function cleanupid(s){
    var news=s;
    news = news.replaceAll('+-', '-');
    if(news.startsWith("-0.0")){
        news = news.slice(1)
    }
    news = news.replaceAll('-0.0', '+0.0');
    news = news.replaceAll('000', '');
    news = news.replaceAll('0.0', '0');

    /// TBD -- clean up cleanup --  simplify the output further
    
    return news

  
}
function generateidforedgeaction(q){

    /// TBD -- clean up cleanup --  simplify the output further
    var id,dir=1,end0,end1, e0, e1
    e0 = q.acton(edgebase0).toString()
    e1 = q.acton(edgebase1).toString()
    end0=cleanupid(e0)
    end1=cleanupid(e1)

    if(end0>end1){var temp = end0; end0=end1; end1=temp;dir=-1}
    return {id:end0+"::"+end1,direction:dir}
}

// TBD: clean up the global edge data structure; 
// each edge is to have a known direction, location, ends, length in R3, etc


const edgeids = edgegroup.map(q=>generateidforedgeaction(q))

const allids = edgeids.concat(vertexids)


// This function returns the index of the edge action. 
// If an action doesn't map 
// the "unit" edge, 1 to W, to an edge of the 24 cell, then the program will crash, 
// but there will be a console message. (Any such error needs to be removed.)

function getindexforaction(q){
    var iddir = generateidforedgeaction(q);// a canonical ID associated with the action, based on the end pts.
    var index = edgeids.findIndex(i=>i.id==iddir.id);// look it up in our edgeid list. 
    // // TBD edgedata.edgeids.findIndex etc. returning a name rather than an index
    if(index<0){//findIndex returns -1 if the id isn't found.
        console.log('FLAMES FLAMES; this is not a legit group action at for '+iddir.toString())}
    return {oldindex:index,  directionchange:iddir.direction*edgeids[index].direction}
}

function getactiononedgegroupaspermutationofindices(q){
    // q is a qAction
   // var debugcntr=0
    var perms = edgegroup.map(g=>{
        //if(debugcntr== 86){console.log("pausehere") }
     //   console.log(debugcntr++)
        var newq = q.inverse().composeon(g)
        var newindex = getindexforaction(newq)
        return newindex}) // of the form {oldindex,directionchange} 
        // // TBD old ID, direction change
    return perms
    // this translates between the indices of an edge before and after the action; 
    // [...{old index, directionchange}...]
}


function writeoutperm(aqaction){
    var content = ""
    var edgeperms = getactiononedgegroupaspermutationofindices(aqaction)
    content+=(edgeperms.map(p=>(p.oldindex * p.directionchange).toString())).join(",")
    content+=","+vertexindexpermutation(aqaction).map(i=>(i+96).toString()).join(",")
    return content
}

function writeSeveralActionsAsPermutationsToAFile(qActionList){
    var content = "" // our output.

    content+="/////////////////////////////////\n"
    content+="// a helpful list of vert perms\n"
    content+="//inline const char* octacellperms[] = {"
    
    qActionList.slice(0,24).map(n=>{
        content+="\""+n[1]+"\",";
    })
    content+="};\n//inline const char* cubecellperms[] = {"
    qActionList.slice(24,48).map(n=>{
        content+="\""+n[1]+"\",";
    })
    content+="};\n\n"



    // first the head
    content+="////// Named Permutations\n\n"


    ////// Named Permutations

    content+="#ifndef NAMEDPERMUTATIONS_H\n"
    content+="#define NAMEDPERMUTATIONS_H\n\n"
    content+="#include \"edgepermutations.h\"\n\n"

//// define each permutation:
    content+="//// define each permutation:\n"

   qActionList.map(item=>{
    content+="static inline std::array<int, 120> "+item[1]+"Perm = {{\n";
    content+=writeoutperm(item[0])+"}};\n\n"
    }
    )

    // we'll hand-code in this fake one:

    content+="static inline std::array<int, 120> simpletestPerm = {{1,2,3,4,5,0,\n";
    for(var i=6;i<120;i++){content+=i.toString()+"," }
    content=content.slice(0,-1)+"}};\n\n";

    content+="static inline std::array<int, 120> simpletest2Perm = {{2,4,0,-1,-3,-5,\n";
    for(var i=6;i<120;i++){content+=i.toString()+"," }
    content=content.slice(0,-1)+"}};\n\n";


    content+="\n"

    content +="// Function to register all named permutations - call from setup()\n"
    content +="inline void registerNamedPermutations() {\n"
    content +="    static bool registered = false;    \n"
    content +="    if(registered) return;     \n"
    content +="     registered = true; \n"  



    qActionList.map(item=>{
    content+="static EdgePermutation "+item[1]+"Permutation(\""+item[1]+"\", "+item[1]+"Perm);\n"
    })

    content+="static  EdgePermutation simpletestPermutation(\"simpletest\", simpletestPerm);\n"
    content+="static  EdgePermutation simpletest2Permutation(\"simpletest2\", simpletest2Perm);\n"

    content +="}\n\n#endif // NAMEDPERMUTATIONS_H"













    // Create a blob and download link
    let blob = new Blob([content], { type: 'text/plain' });
    let url = URL.createObjectURL(blob);
    let a = document.createElement('a');
    a.href = url;
    a.download = 'namedpermutations.h';
    a.click();
    URL.revokeObjectURL(url);
    
  //  return content;
}


///// Here are the edges in order; TBD this is the format they will be in.
/*
0: [-k,--++]
1: [-j,+-+-]
2: [j,-+-+]
3: [k,++--]
4: [k,-+--]
5: [-j,+++-]
6: [j,---+]
7: [-k,+-++]
8: [-j,--+-]
9: [-k,-+++]
10: [k,+---]
11: [j,++-+]
12: [-j,-++-]
13: [k,----]
14: [-k,++++]
15: [ij,+--+]
16: [+-+-,k]
17: [--++,-j]
18: [++--,j]
19: [-+-+,-k]
20: [+-++,-+-+]
21: [---+,++--]
22: [+++-,--++]
23: [-+--,+-+-]
24: [+---,--++]
25: [--+-,-+-+]
26: [++-+,+-+-]
27: [-+++,++--]
28: [+--+,-1]
29: [----,i]
30: [++++,-i]
31: [-++-,1]
32: [-i,-+-+]
33: [1,--++]                    ONE
34: [-1,++--]
35: [i,+-+-]
36: [++++,+---]
37: [-++-,++-+]
38: [+--+,--+-]
39: [----,-+++]
40: [+++-,-i]
41: [-+--,1]
42: [+-++,-1]
43: [---+,i]
44: [++-+,-k]
45: [-+++,-j]
46: [+---,j]
47: [--+-,k]
48: [++--,--+-]
49: [-+-+,+---]
50: [+-+-,-+++]
51: [--++,++-+]
52: [+-++,-j]
53: [+++-,k]
54: [---+,-k]
55: [-+--,j]
56: [+-+-,---+]
57: [++--,+-++]
58: [--++,-+--]
59: [-+-+,+++-]
60: [+--+,-+--]
61: [++++,---+]
62: [----,+++-]
63: [-++-,+-++]
64: [+---,-1]
65: [++-+,-i]
66: [--+-,i]
67: [-+++,1]
68: [i,+---]
69: [1,--+-]                    ONE
70: [-1,++-+]
71: [-i,-+++]
72: [+++-,+--+]
73: [+-++,----]
74: [-+--,++++]
75: [---+,-++-]
76: [++++,-j]
77: [+--+,-k]
78: [-++-,k]
79: [----,j]
80: [++--,-i]
81: [+-+-,-1]
82: [-+-+,1]
83: [--++,i]
84: [++-+,----]
85: [+---,-++-]
86: [-+++,+--+]
87: [--+-,++++]
88: [1,---+]                    ONE
89: [-i,-+--]
90: [i,+-++]
91: [-1,+++-]
92: [1,----]                    ONE
93: [i,+--+]
94: [-i,-++-]
95: [-1,++++]

*/



////////////////////////
///
/// Edge models. Each edge, say anedge, of the mathematical object can return an rgb by
//  anedge.evaluate(position), which incorporates time into some colorfunction, which 
// further may gather other input data. 


const  DEFAULT_TUBE_RADIUS = .03, SMALL_TUBE_RADIUS = .02

const REALRESOLUTION = 10000
class edgemodel{

    // the edgemodel defaults are: 
    // edgemodel:{
    // coloringfunction:"colorname"(or function or index),
    // coloringfunctionindex:0,
    // coloringfunctionoptions:{},direction:1,
    // shiftposition:0,
    // scaleposition:2,
    // shifttime:0,
    // scaletime:1,
    // fordisplayQ:true}

    
// For this application, rework the use of colorfunctions
// Currently, each edgemodel has an coloringfunction (an int or a function name) 
// a coloring function name (deprecated), 
// and a coloringfunction index (which is the real info needed in a model), with no real
// effort to check that these are in alignment. 


    constructor(options={}){
    
        if(options.direction){this.direction = options.direction}
        else this.direction = 1 // -1 can and prob should be controlled by scale. 
        // however, for emerging or converging in the middle, use this flag,
        // conventions yet tbd. 

        
        if(options.coloringfunction){this.coloringfunction = options.coloringfunction}
        else(this.coloringfunction = ourColorFunctionRegistry["blank"])// this is the "blank" color
   
        if(options.coloringfunctionname){this.coloringfunctionname=options.coloringfunctionname
            this.coloringfunction=ourColorFunctionRegistry[this.coloringfunctionname]
        }
        else this.coloringfunctionname =this.coloringfunction.name  


        if(options.coloringfunctionoptions){this.coloringfunctionoptions = options.coloringfunctionoptions}
        
        if(options.scaleposition){this.scaleposition = options.scaleposition}
        else(this.scaleposition = 1) // scale position by

        if(options.shiftposition){this.shiftposition = options.shiftposition}
        else(this.shiftposition = 0) // shift position by
       
        if(options.scaletime){this.scaletime = options.scaletime}
        else(this.scaletime = 1) // scale time by

        if(options.shifttime){this.shifttime = options.shifttime}
        else(this.shifttime = 0) // shift time by

		// this isn't actually used
        if(options.tuberadius){this.tuberadius = options.tuberadius}
        else(this.tuberadius = DEFAULT_TUBE_RADIUS)// defined in hdloviewer, until moved

        // this is useful way to keep track of the color models for export,
        // and hence TBD can rewrite existing models and the use of functions
        // for this.
        if(options.coloringfunctionindex){this.coloringfunctionindex=options.coloringfunctionindex}
        else{this.coloringfunctionindex=0}
      }

      updatecolorfunction(newcolorfunctionname){// this seems like a good one to abstract
        if(ourColorFunctionRegistry[newcolorfunctionname]){
            this.coloringfunction=ourColorFunctionRegistry[newcolorfunctionname]
            this.coloringfunctionname = newcolorfunctionname
        }
        else {console.log("Hey, that's not a legit color function")}
       }

    // the main task of an edge model is to return an RGB value for a given position:
    // at a specific time, which is indicated for the first time here. 

      evaluateat(position){ 
	//	if(this.coloringfunctionname =='blue'){console.log('stopforasec')}
        if(typeof this.coloringfunction == 'string')
        {this.coloringfunction=ourColorFunctionRegistry[this.coloringfunction]}
        if(typeof this.edgecoloringfunction == 'number'){
            // defaultcolorfunctions are defined in edgecolorfunctions.js
            if(this.edgecoloringfunction<=defaultcolorfunctions.length)
                this.coloringfunction=defaultcolorfunctions[this.edgecoloringfunction]
            else
                this.coloringfunction=defaultcolorfunctions[1]
        }
        if(typeof this.coloringfunction != 'function'){
           this.coloringfunction=ourColorFunctionRegistry['defaultcolorfunction']
        }
        
        var adjustposition = position;
        if(this.direction){
            if(this.direction==-1)
                {adjustposition=1-position}}

        
        adjustposition=this.scaleposition*adjustposition+this.shiftposition;
      
        //TBD: Make this work correctly:
        if(!this.direction){
                // then the colorfunction is doubled, flowing to the middle
                adjustposition = 1-2*Math.abs(1/2-adjustposition)}
        else if(this.direction==0){adjustposition =2*Math.abs(1/2-adjustposition)}
               
		//else if(this.direction ==  -1){
       //         adjustposition= 1-adjustposition }
        // else no further adjustments.

		  //var f =ourColorFunctionRegistry[this.coloringfunction.name]
		  var adjusttime = this.scaletime*(Date.now()*.001)+this.shifttime


        return this.coloringfunction(
                adjustposition,
                adjusttime)
            }


    // the basic operations on an edge model are standard dictionary operations.

        copy(){return new edgemodel({
			coloringfunction:this.coloringfunction,
            direction:this.direction,
            coloringfunctionname:this.coloringfunctionname,
            coloringfunctionoptions:this.coloringfunctionoptions,
            shiftposition:this.shiftposition,
            scaleposition:this.scaleposition,
            shifttime:this.shifttime,
            scaletime:this.scaletime,
           // forexportQ:this.forexportQ,
            coloringfunctionindex:this.coloringfunctionindex})

        }


    toArray(){
        // this is for exporting to the Teensy. 
        // We are just assuming that coloringfunctionindex is defined;
        // 
        return[this.coloringfunctionindex,
            this.direction,
            Math.round(REALRESOLUTION* this.shiftposition),
            Math.round(REALRESOLUTION* this.scaleposition),
            Math.round(REALRESOLUTION*this.shifttime ),
            Math.round(REALRESOLUTION*this.scaletime )
        ]
        
       

    
    }
}

/////////////////////////////////////////////
///
/// HDLO models. Lighting it up!
//

// An hdlomodel is an assignment of edgemodels to each of the 96 edges. 
// The fundamental task of a model is to be able to evaluate 
// anhdlomodel.evaluate(edge,position)
// with time and any other parameters input downstream.
// 
// An hdlo model specifies for each edge which edgecolor model, and in which direction, 
// and with what further linear transformations to apply to space and time, to apply 

// TBD: edges are identified by indices; much better by unique ID. 
// TBD: vertices are now being handled in an ad hoc manner, and only for 1IJK. 
 
let ourModelRegistry=[]//,names:[],order:[]}

class hdlomodel{
    constructor(options={}){
        
        this.addToRegistryQ = false
        if("addToRegistryQ" in options){
            this.addToRegistryQ=options.addToRegistryQ
        }
        

        if( "fordisplayQ" in options){this.fordisplayQ = options.fordisplayQ}
        else{this.fordisplayQ = false}//only if true, show in the gui
        
       // if( "forexportQ" in options){this.forexportQ = options.forexportQ}
       // else{this.forexportQ = false}//only if true, export


        if(options.name){this.name = options.name }
        else this.name = "amodel"+(Object.keys(ourModelRegistry).length)


        if(this.addToRegistryQ){
        ourModelRegistry[this.name]=this}//automatically update the registry
        // not really sure if this is doing anything or not right now. 
      

        // can specify the explicit list of edgemodels to fill in
        if(options.edgemodels){ this.edgemodels =options.edgemodels }
        else {
			this.edgemodels=[]
			for(var i=0;i<96;i++){
			this.edgemodels[i] = new edgemodel()}
		}

        //  if there is a list of edgemodel data, 
        if(options.listofedmodels){
            //  distribute this data into new edge models
            this.fillinmodelfromdata(options.listofedmodels)
        }
          //ourModelRegistry.names = [...ourModelRegistry.names,this.name]
        return this
    }


    // the fundamental task that this hdlomodel class does:

     evaluate(edgeindex,position){// scaling and shifting, and the time are be added to colorfunction in the edgemodel
        //other external information is built into the colorfunctions, defined in this thread
        
        return this.edgemodels[edgeindex].evaluateat(position)
     }


     fillinmodelfromdata(listofedmodeldata){
        // each element of the listofedmodeldata will be of the form:
        // name:"a model",
        // listofedgemodels:{indices:[i,j,..],
                // the indices of the edges in order.
                // negative indices are ok
        // distributeby:false 
                //whether to distribute the effect across all of the indices
        // edgemodel: an edgemodel, as described above.
        listofedmodeldata.map(
        edgemodeldata=>{
            var spread = edgemodeldata.indices.length
            var edgecount = 0 // if there is a spread, this keeps track of where we are
            var direction = 1;
            if(edgemodeldata.edgemodel.direction){
                direction = edgemodeldata.edgemodel.direction}
            edgemodeldata.indices.map(
                index=>{
                    var iindex = Math.abs(index)
                    this.edgemodels[iindex]=edgemodeldata.edgemodel.copy()
                    this.edgemodels[iindex].direction =  Math.sign(index)*direction
    
                    if(edgemodeldata.distributeby){
                        // then we add the spreading to the info. 
                        this.edgemodels[iindex].scaleposition = 1/spread //multiply position by this before evaluating the color function
						if(edgemodeldata.scaleposition){
							this.edgemodels[iindex].scaleposition=edgemodels[iindex].scaleposition*edgemodeldata.scaleposition}
                        this.edgemodels[iindex].shiftposition= edgecount++/spread 
						if(edgemodeldata.shiftposition){
							this.edgemodels[iindex].shiftposition= this.edgemodels[iindex].shiftposition+edgemodeldata.shiftposition
						}
                    }
                    
                })
        })
    }

    changename(newname){
        // this ensures that the model registry is kept up to date
        // delete the old key from the model registry
        // if the new key is already being used, delete that too!
        // Notice that this overwrites any model with that name
        // add the new key to the model registry
        var oldname = (this.name)
        delete ourModelRegistry.oldname
        if(ourModelRegistry[newname]){
            delete ourModelRegistry.newname
        }
        ourModelRegistry[newname]=this
        this.name= newname
        return this
    }

    delete(){
        delete ourModelRegistry.this.name
    }

    copy(options={})
    {   
        var newname=this.name+" copy"
        if(options.name){
            newname = options.name}

        var copyofedgemodels=[]
        for(var i = 0;i<96;i++){
            copyofedgemodels[i]=this.edgemodels[i].copy()
            if(options.colorpermutations){
            if(options.colorpermutations[copyofedgemodels[i].coloringfunctionname]) // is this edgecolor one to be permuted?
                {
                    copyofedgemodels[i].updatecolorfunction(
                    options.colorpermutations[copyofedgemodels[i].coloringfunctionname])
                }
        }}
        var fordisplayQ=this.fordisplayQ
        if("fordisplayQ" in options){fordisplayQ=options.fordisplayQ}

        //var forexportQ=this.forexportQ
       // if("forexportQ" in options){forexportQ=options.forexportQ}

        var addToRegistryQ = this.addToRegistryQ
        if("addToRegistryQ" in options){addToRegistryQ=options.addToRegistryQ}
        var newmodel = new hdlomodel(
         {...options, name:newname,edgemodels:copyofedgemodels, 
            fordisplayQ:fordisplayQ,//forexportQ:forexportQ, 
             addToRegistryQ:addToRegistryQ})
        return newmodel 
    }


    //TBD: Add the ability to permute additional features

    permute(quatoraction,options={})
        //return a new model, the old model shifted by a quat (on right iff onrightq)
        //or by an action (for convenience, these are specified as {l,r} for q->(l^-1 q r)
        //Color permutations are by a dictionary on color names (if a key isn't present, don't permute)
    {   
        // options include name, onrightq, fordisplayQ, 
        

        var newname=this.name+(quatoraction.toString())//TBD make short string 
        if(options.name){newname = options.name}

        // we presume permutations are to show off
        
        var newfordisplayQ = this.fordisplayQ;
        if("fordisplayQ" in options){newfordisplayQ=options.fordisplayQ}
        
        var newaddToRegistryQ = true;
        if("addToRegistryQ" in options){newaddToRegistryQ=options.addToRegistryQ}

        var qaction = quatoraction
        if(quatoraction.constructor.name=='quat'){
            if(!options.onrightq){qaction = new qAction(quatoraction,qOne)}
            else qaction = new qAction(qOne,quatoraction)
        }

        var newedgemodels  = []
        var perms = getactiononedgegroupaspermutationofindices(qaction)
        // of the form {oldindex,directionchange} 
        for(var i = 0; i<96; i++){
			newedgemodels[i]=this.edgemodels[perms[i].oldindex].copy()
            //this.edgemodels[perms[i]] is the  edge models of this. 
            newedgemodels[i].direction =(newedgemodels[i].direction)* (perms[i].directionchange)
            if(options.colorpermutations){
            if(options.colorpermutations[newedgemodels[i].coloringfunctionname]) // is this edgecolor one to be permuted?
                {newedgemodels[i].updatecolorfunction(
                    options.colorpermutations[newedgemodels[i].coloringfunctionname])
            }}
        }
        var newmodel = new hdlomodel({...options, edgemodels:newedgemodels, fordisplayQ:newfordisplayQ,addToRegistryQ:newaddToRegistryQ})
		return newmodel

    }

    mergeonto(anothermodel,options={})//overwrites this model over anothermodel, 
    // producing a newmodel
    {
        var deleteOntoQ = false
        if(options.deleteOntoQ){
            deleteOntoQ=options.deleteOntoQ
        }
        var newmodel =  anothermodel.copy(options)
        for(var i = 0; i<96; i++){
            if(this.edgemodels[i].coloringfunctionname!='blank'){
                newmodel.edgemodels[i]=this.edgemodels[i].copy()
            }
        }
        if(options.name){ 
            newmodel.name = options.name}
        else{newmodel.name = this.name+" & "+anothermodel.name}

        if(options.fordisplayQ){newmodel.fordisplayQ=true}
        else{newmodel.fordisplayQ = this.fordisplayQ}

        return newmodel
    }

    applyactions(actionoptionlist,options={}){
        // actionoptionlist is either 
        //[transform1 (a quat or qaction), transform2, ... ]
        // or (more likely)
        // [{transform:transform1, ... additional options1},...]

        // We may have additonal options for the actions as a whole:
        // name, fordisplayQ for the final piece, as well as the individual ones.

        var newname = this.name+" compound "+(Object.keys(ourModelRegistry)).length
        if(options.name){newname =options.name}
        
        var fordisplayQ = false
        if(options.fordisplayQ){fordisplayQ=options.fordisplayQ}
        
        var addToRegistryQ = false
        if(options.addToRegistryQ){addToRegistryQ=options.addToRegistryQ}

        
        var finalobject = new hdlomodel({name:newname,fordisplayQ:fordisplayQ, addToRegistryQ:addToRegistryQ}) // to merge onto
        var displayeachQ = false
        var colorcount = 1;
        if(options.displayeachQ){displayeachQ=options.displayeachQ}

         actionoptionlist.map(actionoroption=>{
            // which are we?
            
            // if actionoroption is a quat or a qAction:
            var newoptions = {fordisplayQ:displayeachQ}
            var transform = actionoroption
            //else
            if(!actionoroption.constructor.name=='quat'&&
                !actionoroption.constructor.name=='qAction'){
                    transform =actionoroption.transform// we need a transform,
                    // but we may not have options
                    if(actionoroption.options){
                        newoptions =actionoroption.options
                        if(actionoroption.options.fordisplayQ){
                            options.fordisplayQ=actionoroption.options.fordisplayQ
                        }
                    }
                    
                    
                }
            if(options.cosetcolorsQ){
                var perms = {1:colorcount++}
                newoptions["colorpermutations"]=perms;
            }
            var nextobject=this.permute(transform,{...newoptions,addToRegistryQ:false})
            //this is the only place this is used!
            finalobject = finalobject.mergeonto(nextobject,{addToRegistryQ:false})
        }

        )
        if(options.fordisplayQ){finalobject.fordisplayQ=options.fordisplayQ}
        var oldkey = Object.keys(ourModelRegistry).at(-1)
        delete ourModelRegistry.oldkey
        return finalobject.changename(newname)
        
    }

   
}


///// And a way to print ourModelRegistry out :


function writeModelsToFile(modelRegistry) {
    
    let headercontent="",bodycontent="";
    headercontent+="///////////////////////////\n";
    headercontent+="// HDLO Models\n\n// define the data:\n\n";
    
    bodycontent = headercontent;
    
    headercontent +="#ifndef HDLO_MODELS_H\n#define HDLO_MODELS_H\n#include <array>\n#include \"models.h\"\n\n";
    bodycontent+="#include \"hdlo_models.h\"\n\n";

    let modelkeys =  Object.keys(ourModelRegistry).filter(
                key=>ourModelRegistry[key].fordisplayQ)

    let nummodels = modelkeys.length;

    modelkeys.map(key=>{
        let name = key.replace(/[^a-zA-Z0-9]/g, '');
        headercontent+= "extern std::array<std::array<int, 6>, 120> "+name+"data;\n";
        bodycontent  += "std::array<std::array<int, 6>, 120> "+name+"data={{\n";
                for(let i=0; i<120; i++){
                    bodycontent+="  {{";
                    if(i<96){
                        var nname = ourModelRegistry[key].edgemodels[i].coloringfunctionname;
                        if(nname == "blank"){nname = "0"}
                        //Still need to add vertex model information for 
                        // entries above 96. TBD. For now we just make a stub
                    bodycontent+=
                        nname+","+
                        ourModelRegistry[key].edgemodels[i].direction+","+
                        Math.round(ourModelRegistry[key].edgemodels[i].shiftposition*10000)+","+
                        Math.round(ourModelRegistry[key].edgemodels[i].scaleposition*10000)+","+
                        Math.round(ourModelRegistry[key].edgemodels[i].shifttime*10000)+","+
                        Math.round(ourModelRegistry[key].edgemodels[i].scaletime*10000)+'}},   // edge '+i+'\n'
                    }
                    else bodycontent+="0,1,0,10000,0,10000}}, // vertex "+(i-96)+' (line '+i+')\n'
                
                }
                bodycontent+="}};//end of " +name+'data\n\n';
            }
        
        );

    headercontent+="// array of pointers\nconst int nummodels = "+nummodels+";\n"
    headercontent+="extern colormodel* ourcolormodels["+nummodels+"];\n"
    
    headercontent+="\nextern String modelNames["+nummodels+"]; // Model names matching ourcolormodels order\n"
    headercontent+="void initializemodels();\n"   
    headercontent+="class modelsequence; // Forward declaration;\n"
    headercontent+="void initializeSequences(modelsequence* seq);\n"
    headercontent+="#endif // HDLO_MODELS_H\n"





    bodycontent += "\ncolormodel* ourcolormodels["+nummodels+"];\n\n";
    bodycontent += "void initializemodels(){\n"
    
    
    for(let i = 0; i<nummodels;i++){
        let key = modelkeys[i];
        let name = key.replace(/[^a-zA-Z0-9]/g, '');
        bodycontent+="  ourcolormodels["+i+"] = new colormodel("+name+"data, \""+name+"\");\n"                  
    }
    
    bodycontent+="}\n";

    bodycontent+="\nString modelNames["+nummodels+"] ={\n";
    for(let i=0; i<nummodels;i++){
        let key = modelkeys[i];
        let name = key.replace(/[^a-zA-Z0-9]/g, '');
        bodycontent+="\""+name+"\",\n"
    }
     
    bodycontent+="};\n\n"
    

    // Create a blob and download link
    let blob = new Blob([headercontent], { type: 'text/plain' });
    let url = URL.createObjectURL(blob);
    let a = document.createElement('a');
    a.href = url;
    a.download = 'hdlo_models.h';
    a.click();
    URL.revokeObjectURL(url);
    
    // Create a blob and download link
    blob = new Blob([bodycontent], { type: 'text/plain' });
    url = URL.createObjectURL(blob);
    a = document.createElement('a');
    a.href = url;
    a.download = 'hdlo_models.cpp';
    a.click();
    URL.revokeObjectURL(url);
    

  //  return content;
}






////////////////////////////////
// 
// Our models
//
//

// In this edition of the code, we are focusing on the 
// fundamental building blocks that we can assemble in the teensy
// using merge and permute operators. 


const basichdlomodel = new hdlomodel({name:'basicModel'})

basichdlomodel.name = 'basicModel'

//defaultmodel ='six paths'
defaultmodel = 'strands'

const hypercube = new hdlomodel ({name:'hypercube',
    listofedmodels:
    [{indices:[59,37,38,56,
        63,49,50,60,
        62,51,48,61,
        39,58,57,36],
        edgemodel:new edgemodel({coloringfunctionname:1, direction:1,scaleposition:.4})},
    {indices:[74,27,24,73,87,21,22,84,23,86,20,85,75,25,26,72],
        edgemodel:new edgemodel({coloringfunctionname:1, direction:-1,shiftposition:.4,scaleposition:.4})},
],fordisplayQ:true,addToRegistryQ:true})

const shiftedcube = hypercube.applyactions([qW],
    {   fordisplayQ:true, 
        cosetcolorsQ:true,
        addToRegistryQ:true,
        name:'rotated hypercube',
    })

const threecubes = hypercube.applyactions([qOne,qW,new quat(-1,-1,1,1).normalize()],
    {   fordisplayQ:true, 
        cosetcolorsQ:true,
        addToRegistryQ:true,
        name:'hypercubes',
    })


const twentyfourcell = threecubes.permute(qOne, {colorpermutations:[1,1,1,1,1,1],name:"twentyfourcell"})
twentyfourcell.fordisplayQ=true; 



/*
const test=new hdlomodel({
    name:'test',
    listofedmodels:[
        {indices:[0],edgemodel:new edgemodel(
            {coloringfunctionindex:1, coloringfunctionname:1})},
        {indices:[1],edgemodel:new edgemodel(
            {coloringfunctionindex:2, coloringfunctionname:2})},
        {indices:[2],edgemodel:new edgemodel(
            {coloringfunctionindex:3, coloringfunctionname:3})},
        {indices:[3],edgemodel:new edgemodel(
            {coloringfunctionindex:4, coloringfunctionname:4})},
        {indices:[4],edgemodel:new edgemodel(
            {coloringfunctionindex:5, coloringfunctionname:5})},
        {indices:[5],edgemodel:new edgemodel(
            {coloringfunctionindex:6, coloringfunctionname:6})}],fordisplayQ:true,addToRegistryQ:true})
*/
 
////////////////////////////
////
//// cycles

// +--- (67) 1 (95) ++++ (39) -+++ (64) -1 (92) ---- (36) +---

const cycle = new hdlomodel(
    {name:'cycle',
    listofedmodels:
    [{indices:
        [67,95,39,64,92,36],
        distributeby:true,
        edgemodel:new edgemodel({coloringfunctionindex:1,
            coloringfunctionname:1})}],fordisplayQ:false,addToRegistryQ:true})


const cycles = cycle.applyactions(shiftcyclesright,
    {fordisplayQ:true, 
        addToRegistryQ:true,
        name:'cycles',
    })


const allcycles = cycles.applyactions(rots4X,
    {   fordisplayQ:true, 
        cosetcolorsQ:true,
        addToRegistryQ:true,
        name:'all cycles',
    })



const graycode = new hdlomodel(
	{
	name:'gray codes',
    listofedmodels:[
        {indices:[22,57,23,49,86,37,-51,-21,-62,39,85,38,84,-73,20,56],
            distributeby:true,
            edgemodel:new edgemodel({coloringfunctionname:1, direction:1})
            },
    {indices:[61,72,63,74,-87,25,50,24,48,26,59,75,60,-58,-27,-36],distributeby:true,
            edgemodel:new edgemodel({coloringfunctionname:2, direction:1})}
],fordisplayQ:true, 
        addToRegistryQ:true,})

const threecodes = graycode.applyactions([qOne,qW,new quat(-1,-1,1,1).normalize()],
    {   fordisplayQ:true, 
        cosetcolorsQ:true,
        addToRegistryQ:true,
        name:'six paths',
    })




    const octahedron = new hdlomodel(
    {name:'octahedron', 
    listofedmodels:[
        {indices:[91,43,-66,-70],distributeby:true, edgemodel:new edgemodel({
            coloringfunctionindex:3, coloringfunctionname:1})},
            {indices:[95,29,-83,-34],distributeby:true, edgemodel:new edgemodel({
            coloringfunctionindex:1, coloringfunctionname:2})},
        {indices:[21,51,87,62],distributeby:true, edgemodel:new edgemodel({
            coloringfunctionindex:2,coloringfunctionname:3,scaletime:2})}
    ],
    fordisplayQ:true,
    //forexportQ:true, 
    addToRegistryQ:true})

const basiccube = new hdlomodel({name:'cube',
    listofedmodels:
    [{indices:[-87,-74,62],
        edgemodel:new edgemodel({coloringfunctionname:1})},
    {indices:[49,59,-23],
        edgemodel:new edgemodel({coloringfunctionname:2})},
    {indices:[-27,-21,51],
        edgemodel:new edgemodel({coloringfunctionname:3})},
    {indices:[37,63,-86],
        edgemodel:new edgemodel({coloringfunctionname:4})}],
    fordisplayQ:true,
    addToRegistryQ:true})
        
const strandinfo= new hdlomodel({name:'strands',
    listofedmodels:
    [
     //   {indices:[39,-71,-29,62],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:1})},
    //   {indices:[-57,80,90,23],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:2})},
  {indices:[79,-46,85],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:3})},
    {indices:[55,-18,48],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:1})},
    {indices:[-14,7,74,-95,-41],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:2})},
  //  {indices:[9,0],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:6})},
  //  {indices:[-64,24,-33],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:7})}
   ],
    fordisplayQ:true,
    addToRegistryQ:true})



const allstrandinfo= new hdlomodel({name:'all strands',
    listofedmodels:
    [
        {indices:[39,-71,-29,62],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:1})},
       {indices:[-57,80,90,23],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:2})},
    {indices:[-14,46,85],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:3})},
    {indices:[-7,-18,48],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:4})},
    {indices:[79,-55,74,-95,-41],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:5})},
    {indices:[-9,0],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:6})},
    {indices:[-64,24,-33],distributeby:true,edgemodel:new edgemodel({coloringfunctionname:7})}
   ],
    fordisplayQ:true,
    addToRegistryQ:true})

/*
const basiccube = new hdlomodel({name:'cube',
    listofedmodels:
    [{indices:[59,37],
        edgemodel:new edgemodel({coloringfunctionname:1, direction:1})},
    {indices:[63,49],
        edgemodel:new edgemodel({coloringfunctionname:2, direction:1})},
    {indices:[62,51],
        edgemodel:new edgemodel({coloringfunctionname:3, direction:1})},
    {indices:[74,27],
        edgemodel:new edgemodel({coloringfunctionname:1, direction:-1})},
    {indices:[87,21,],
        edgemodel:new edgemodel({coloringfunctionname:2, direction:-1})},
    {indices:[23,86],
        edgemodel:new edgemodel({coloringfunctionname:3, direction:-1})},
],fordisplayQ:true,addToRegistryQ:true})
*/