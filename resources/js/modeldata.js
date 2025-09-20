
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
    //var debugcntr=0
    var perms = edgegroup.map(g=>{
        //if(debugcntr== 86){console.log("pausehere") }
        //debugcntr++
        var newq = q.inverse().composeon(g)
        var newindex = getindexforaction(newq)
        return newindex}) // of the form {oldindex,directionchange} 
        // // TBD old ID, direction change
    return perms
    // this translates between the indices of an edge before and after the action; 
    // [...{old index, directionchange}...]
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


class edgemodel{

    // the edgemodel defaults are: 
    // edgemodel:{
    // coloringfunction:"colorname"(or function or index),
    // coloringfunctionoptions:{},direction:1,
    // shiftposition:0,
    // scaleposition:2,
    // shifttime:0,
    // scaletime:1,
    // fordisplayQ:true}

    
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
        //if(typeof this.edgecoloringfunction == 'number')// let's get rid of these
        if(typeof this.coloringfunction != 'function'){
           {this.coloringfunction=ourColorFunctionRegistry['defaultcolorfunction']}
        }
        
        var direction = 1
        if(this.direction){
            if(this.direction==-1)
                {direction=-1}}

        var adjustposition = direction*this.scaleposition*position+this.shiftposition;
      
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
            fordisplayQ:this.fordisplayQ})

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
        

        if(options.name){this.name = options.name }
        else this.name = "amodel"+(Object.keys(ourModelRegistry).length)


        if(this.addToRegistryQ){
        ourModelRegistry[this.name]=this}//automatically update the registry
      

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

        var addToRegistryQ = this.addToRegistryQ
        if("addToRegistryQ" in options){addToRegistryQ=options.addToRegistryQ}
        var newmodel = new hdlomodel(
         {...options, name:newname,edgemodels:copyofedgemodels, 
            fordisplayQ:fordisplayQ, addToRegistryQ:addToRegistryQ})
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
        if(options.displayeachQ){displayeachQ=options.displayeachQ}

         actionoptionlist.map(actionoroption=>{
            // which are we?
            
            // if actionoroption is a quat or a qAction:
            var options = {fordisplayQ:displayeachQ}
            var transform = actionoroption
            //else
            if(!actionoroption.constructor.name=='quat'&&
                !actionoroption.constructor.name=='qAction'){
                    transform =actionoroption.transform// we need a transform,
                    // but we may not have options
                    if(actionoroption.options){
                        options =actionoroption.options
                        if(actionoroption.options.fordisplayQ){
                            options.fordisplayQ=actionoroption.options.fordisplayQ
                        }
                    }
                    
                    
                }
            var nextobject=this.permute(transform,{...options,addToRegistryQ:false})
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








////////////////////////////////
// 
// Our models
//
//



const basichdlomodel = new hdlomodel({name:'basicModel'})

basichdlomodel.name = 'basicModel'


// a basic octahedron, with flow from one end (at to the other. This is positio
const baseflowingoctahedron = new hdlomodel(
    {name:'flow octahedron', 
    listofedmodels:[
        {indices:[95,91,70,34],distributeby:false, edgemodel:new edgemodel({coloringfunctionname:1,scaleposition:.5})},
        {indices:[29,43,66,83],distributeby:false, edgemodel:new edgemodel({coloringfunctionname:1,shiftposition:.5,scaleposition:.5})},
        {indices:[21,51,87,62],distributeby:true, edgemodel:new edgemodel({coloringfunctionname:2,direction:1,shiftposition:0,scaleposition:1,scaletime:1})}
    ],fordisplayQ:false,addToRegistryQ:false})

// to this we can add colorways: 


const flowoct = baseflowingoctahedron.permute(qOneOne,{name:'flow oct', 
    colorpermutations:{1:"basiccycle", 2:"huewheel"},fordisplayQ:false,addToRegistryQ:false})


const anoctachain = baseflowingoctahedron.applyactions([qOneOne,qIOne,qMOneOne,qmIOneOne
],{fordisplayQ:false, addToRegistryQ:false, name:''})

const octachain  = anoctachain.permute(qOne,
    {name:"octachain",colorpermutations:{2:"bluespikepulse", 1:"huewheel"}
,fordisplayQ:true})

const octachain2 = octachain.permute(qW,{name:"octachain shifted by ++++",
    colorpermutations:{"huewheel":"redspikepulse"},fordisplayQ:true
})


var templist =[ 
        {indices:[95,29,66,70],distributeby:true, 
            edgemodel:new edgemodel({
                coloringfunctionname:"redspikepulse",scaleposition:.5})},
        {indices:[83,34,91,43],distributeby:true, 
            edgemodel:new edgemodel({
                coloringfunctionname:"greenspikepulse",shiftposition:.5,scaleposition:.5})},
        {indices:[21,51,87,62],distributeby:true, 
            edgemodel:new edgemodel({
                coloringfunctionname:"bluespikepulse",direction:1,shiftposition:0,scaleposition:1,scaletime:1})}
    ]

    /*
const rotatingocta = new hdlomodel(
    {name:'rotatingocta', 
    listofedmodels:templist,fordisplayQ:true,addToRegistryQ:true})
   
*/




// +--- (67) 1 (95) ++++ (39) -+++ (64) -1 (92) ---- (36) +---

const cycle = new hdlomodel(
    {name:'cycle',
    listofedmodels:
    [{indices:
        [67,95,39,64,92,36],
        distributeby:true,
        edgemodel:new edgemodel({coloringfunction:"testgauss"})}],fordisplayQ:true,addToRegistryQ:true})


const cyclestemplate = cycle.applyactions(shiftcyclesright,
    {fordisplayQ:false, addToRegistryQ:false, name:'cycles template',
    })

registercolorfunction("testgauss",
    function(x,t){return gaussiancolorfunction(x+.3,3*t,.2, .5, sigma= .1)})

    const cycles = cyclestemplate.permute(qOne,
    {name:"cycles",
    colorpermutations:{1:"testgauss"},fordisplayQ:true})

defaultmodel ='basicModel'
defaultmodel ='octahedron'
defaultmodel ='new oct'
defaultmodel ='octachain'
defaultmodel ='rotatingocta'
defaultmodel ='cycles'

/*
var fourcycles = structuredClone(cycle)
rightcycleclass.map(m=>fourcycles = mergemodels(fourcycles,m))


const cycleactionscosets =//[[qOneOne,12], [qOqO,11],[new qAction(qI,qI),13] ]
[[qOneOne,12], [qOqO,11],[qIOne,13]]

const cycleperms = cycleactionscosets.map(q=>
    permutemodel(fourcycles,q[0],"-",[q[1]])
)
//cycleperms.map(m=>addmodel(m))

allcycles = structuredClone(fourcycles)
cycleperms.map(a=>{allcycles = mergemodels(allcycles, a
,"all cycles template")})

fourcycles.name="four cycles"

*/


/*
const leftcycleclass = [new qAction(qone, qone),// new qAction(qO,qone), 
    new qAction(qI,qone), //new qAction(new quat(-Math.sqrt(.5),Math.sqrt(.5)),qone) 
        ].map(q=>permutemodel(cycle, q,""))
*/
/*
const leftcycles = permutemodel(mergemodels(cycle, 
    mergemodels(leftcycleclass[1], 
        mergemodels(leftcycleclass[2],leftcycleclass[3]))),
        new qAction(qone, qone),[7])
*//*
const fourleftcycles = [[new qAction(qone, qone),5], [new qAction(qO,qO),6], [new qAction(qI,qI),7], 
[new qAction(new quat(-Math.sqrt(.5),Math.sqrt(.5)),new quat(-Math.sqrt(.5),Math.sqrt(.5))),8]
        ].map(q=>permutemodel(leftcycles, q[0],q[1]))

*/


//addmodel(fourleftcycles)


/* // important cycle info!
function makecycle(colorindices,direction=1){
    var models
    switch(colorindices.length){
        case 1: 
            models = [{indices:[95,64,36,67,92,39],modelinfo:[colorindices[0],direction]}]
            break;
        case 2:
            models = [{indices:[95,64,36], modelinfo:[colorindices[0],direction]},
                {indices:[67,92,39],modelinfo:[colorindices[1],direction]}]
            break;
        case 3:
            models = [{indices:[39,36], modelinfo:[colorindices[0],direction]},
            {indices:[67,64],modelinfo:[colorindices[1],direction]},
            {indices:[92,95],modelinfo:[colorindices[2],direction]}]
            break;
    }

    return makemodel(
        {
        name:"cycle "+colorindices.toString()+" "+direction.toString(),
            listofindexandcolorlists:models}
    )
}
*/



///////////////
//  Here are three sample colorings, a 1 coloring, 2 coloring, or 3

//var acycle = makecycle([1])
//var qq= new qAction(qOne.positivize(), qI)

//addmodel(permutemodel(makecycle([1]),qq,'cycle 1'))
//addmodel(permutemodel(makecycle([1,2]),qq,'cycle 2'))
//addmodel(permutemodel(makecycle([1,2,3]),qq,'cycle 3'))

//addmodel(makemodel({name:'basic cube',listofindexandcolorlists:   [{indices:[74,59,37,27,87,21,63,49,62,51,23,86],modelinfo:[1,1]}]}))

/*
const basiccube = makemodel({name:'cube',listofindexandcolorlists:
    [{indices:[59,37],modelinfo:[1,1]},
    {indices:[63,49],modelinfo:[2,1]},
    {indices:[62,51],modelinfo:[3,1]},
    {indices:[74,27],modelinfo:[1,-1]},
    {indices:[87,21,],modelinfo:[2,-1]},
    {indices:[23,86],modelinfo:[3,-1]},
]})

const misccubes = [[qJ,"-- cube 2"],[qI.mult(qW),"-- cube 3"],[qmone,"-- cube 4"],[qI.mult(qW).mult(qI),"-- cube 5"]].map(q=>{
    var qa = new qAction(qone,q[0])
    return permutemodel(basiccube,qa,q[1] )
}
)


const basichypercube =  makemodel({name:'four color hypercube',listofindexandcolorlists:
    [{indices:[59,37,38,56],modelinfo:[1,1]},
    {indices:[63,49,50,60],modelinfo:[2,1]},
    {indices:[62,51,48,61],modelinfo:[3,1]},
    {indices:[39,58,57,36],modelinfo:[4,1]},
    {indices:[74,27,24,73],modelinfo:[1,-1]},
    {indices:[87,21,22,84],modelinfo:[2,-1]},
    {indices:[23,86,20,85],modelinfo:[3,-1]},
    {indices:[75,25,26,72],modelinfo:[4,-1]},
]

})





const hypercube = makemodel({name:'hypercube',
    colorways:['redpulse','greenpulse','bluepulse'],
    listofindexandcolorlists:
    [{indices:[59,37,38,56,
        63,49,50,60,
        62,51,48,61,
        39,58,57,36],modelinfo:[1,1]},
    {indices:[74,27,24,73,87,21,22,84,23,86,20,85,75,25,26,72],modelinfo:[1,-1]},
]
})


var tempp = [qW,new quat(-1,1,1,1).normalize()]

var hypercubes=[qW,new quat(-1,-1,1,1).normalize()].map(q=>
permutemodel(hypercube, new qAction(qOne.positivize(), q),"hypercube"+tempcntr++))
    
hypercubes = [hypercube,...hypercubes ]



tempcntr = 1;
var rr = new quat(-1,-1,1,1).normalize();

var coloredhypercubes=[[qone,[5]],[qW,[6]],[rr,[10]]].map(q=>
permutemodel(hypercube, new qAction(qone, q[0]),"colored hypercubes"+tempcntr++,q[1]))
    


const compoundofhypercubes = mergemodels(coloredhypercubes[0],
						mergemodels(coloredhypercubes[1],coloredhypercubes[2]),'three hypercubes')



const twentyfourcell = mergemodels(hypercubes[0],
                            mergemodels(hypercubes[1],hypercubes[2]),'twenty-four cell')


const twentyfourcell2 = permutemodel(twentyfourcell,new qAction(qone,qone),'twenty-four cell 2',[4])

//hypercubes.map(h=>addmodel(h))


///// a little gray code action:


const graycode = makemodel({name:'gray code',
    listofindexandcolorlists:
    [{indices:[22,57,23,49,86,37,-51,-21,-62,39,85,38,84,-73,20,56],modelinfo:[5]},
    {indices:[61,72,63,74,-87,25,50,24,48,26,59,75,60,-58,-27,-36],modelinfo:[6,-1]}
]})




	
const graycode2 = makemodel(
	{
	name:'two gray codes',
    listofindexandcolorlists:[
        {indices:[22,57,23,49,86,37,-51,-21,-62,39,85,38,84,-73,20,56],modelinfo:[1],spread:2,timing:1},
    {indices:[61,72,63,74,-87,25,50,24,48,26,59,75,60,-58,-27,-36],modelinfo:[2],spread:1}
]})



//a compound of six hamiltonian paths

tempcntr = 0;
var graycodes=[[qone,[5,6]],[qW,[7,8]],[rr,[3,4]]].map(q=>
permutemodel(graycode2, new qAction(qone, q[0]),"gray code"+tempcntr++,q[1]))
    


const compoundofgraycodes = mergemodels(graycodes[0],mergemodels(graycodes[1],graycodes[2]),'six paths')
*/
/*const centralcube = makemodel({name:"centralcube",
    listofindexandcolorlists:[{indices:[]
    }]})
*/
/*
const possibleunit = makemodel({name:"possibleunit",
    listofindexandcolorlists:[{indices:[69, -48,80,90,-7,0,18,
        -55,-57,-24,-39,-14,9,46,-79,29,71,50,81
    ],modelinfo:[1],spread:1,timing:1}]
})



const rot = new qAction(new quat(1,1,0,0).normalize(),new quat(1,1,0,0).normalize())
const possibleunitpieces = 
[[qOneOne,[11]], [rot,[12]],
[new qAction(new quat(0,1,0,0).normalize(),new quat(0,1,0,0).normalize()),[13]],
[new qAction(new quat(1,-1,0,0).normalize(),new quat(1,-1,0,0).normalize()),[8]],
//,[qOqO.mult(qOqO).mult(qOqO),[10]]
						   ].map(q=>
    permutemodel(possibleunit,q[0],"-",[q[1]]))

var possibleunits =possibleunit; //
     mergemodels(possibleunitpieces[0],possibleunitpieces[1]) ;
possibleunitpieces.map(m=>possibleunits=mergemodels(possibleunits,m))
possibleunits.name = "possibleunits"
addmodel(possibleunits)


*/
/*
//addmodel(cycle)
//rightcycleclass.map(m=>addmodel(m))
addmodel(fourcycles)
//addmodel(allcycles)

addmodel( octahedron)
moreoctas.map(o=>addmodel(o)) 

addmodel(basiccube)
misccubes.map(o=>addmodel(o)) 
//addmodel(basichypercube)
addmodel(hypercube)
addmodel(permutemodel(hypercube,new qAction(qone, qI.mult(qW)),"   hypercube 2"))
addmodel(twentyfourcell)
addmodel(twentyfourcell2)
addmodel(compoundofhypercubes)

//addmodel(graycode)

addmodel(graycode2)
addmodel(compoundofgraycodes)

//tet cube
//& hcube

// hamiltonian path(s!)

// chains of spinning octahedra

//cycles



//defaultmodel = 'hypercube'
defaultmodel = 'three hypercubes'
defaultmodel = 'gray code'
defaultmodel = 'six hamiltonian paths'
defaultmodel = 'two gray codes'
defaultmodel = 'twenty-four cell';
//defaultmodel = 'octachain2'
defaultmodel = 'four cycles';
//defaultmodel = 'all cycles';

//defaultmodel = 'possibleunits'


*/

