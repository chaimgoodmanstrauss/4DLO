
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

var edgebase0 = qOne.positivize(), edgebase1=  new quat(.5,.5,.5,.5);

// positioning the edges in a canonical way:

const edgegroup = makegroup([
   // new qAction(qI, qOne), 
    
    new qAction(new quat(sqrt(.5),sqrt(.5),0,0),new quat(sqrt(.5),sqrt(.5),0,0)),
    new qAction(qW,qOne),
    
]).groupElements


function cleanupid(s){
    var news=s;
    news = news.replaceAll('+-', '-');
    if(news.startsWith("-0")){
        news = news.slice(1)
    }
    news = news.replaceAll('-0.0', '+0.0');
    news = news.replaceAll('000', '');
    return news

  
}
function generateidforedgeaction(q){
    var id,dir=1,end0,end1, e0, e1
    e0 = q.acton(edgebase0).toString()
    e1 = q.acton(edgebase1).toString()
   /* if (end0.startsWith("-0")) {
    end0=end0.slice(1)};
    if (end1.startsWith("-0")) {
    end1=end1.slice(1)};*/
    end0=cleanupid(e0)
    end1=cleanupid(e1)

    if(end0>end1){var temp = end0; end0=end1; end1=temp;dir=-1}
    return {id:end0+"::"+end1,direction:dir}
}

const edgeids = edgegroup.map(q=>generateidforedgeaction(q).id)


// more generally, we can manipulate edge indices 

function getindexforaction(q){
    var iddir = generateidforedgeaction(q);
    var index = edgeids.indexOf(iddir.id);
    if(index<=0){
        console.log('hi')}
    return [index,iddir.direction]
}

function getactiononedgegroupaspermutationofindices(q){
    var perms = edgegroup.map(g=>(getindexforaction(g.mult(q))))
    return perms
    // this translates between the indices of an edge before and after the action; 
    // [...[oldindex, directionchange]...]
}

function permutedgemodels(edgemodellist,q){
    // an edge model is index:[color, direction]
    // we create a new one as follows:
    var perms =getactiononedgegroupaspermutationofindices(q)
    var out=perms.map(p=>{
            var model = edgemodellist[p[0]]; 
            return [model[0],p[1]*model[1]]})
    return  out
}


function permutemodel(oldmodel,q,newname=""){
    var newmodel= structuredClone(oldmodel)
    newmodel.edgedata= permutedgemodels(oldmodel.edgedata,q)
    if(newname!=""){newmodel.name = newname}
    else{newmodel.name = oldmodel.name+"_"+q.toString()}
    return newmodel
}

function permutenamedmodel(name,q,newname="")
{
    return permutemodel(ourmodels.find(item => item.name === name),q,newname)
}

///// Here are the edges in order
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






const vertgroup =makegroup([new qAction(qI,qOne),
			new qAction(qW,qOne)],"Oxone").groupElements;

const vertices = vertgroup.map(m=>{return (m.acton(qOne))})

const vertexmaterials = [mats[1],mats[11],mats[14],mats[22]]
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





// makeGroupFromName("Oxone").groupElements; 

/*
{vertmotions:makegroup([new qAction(qI,qOne),
			new qAction(qW,qOne)],"Oxone").groupElements,
	edgemotions:
			//makegroup(
			//	[new qAction(qI,qOne),new qAction(qOne,qI),new qAction(qW,qOne),new qAction(qOne,qW)],"Oxone"),
		makeGroupFromName("Oxone").groupElements,
	vertexbasepoints:[qOne],
	edges://each coset gets one; the motions flip these around
		[["1111","1000",1],["1000","111-",1],,["1000","11-1",1],,["1000","1-11",1]]
	},//end of '24-cell'
*/

//////////////////////////////////////////////////////////
///
/// The Models 

// For a physical sculpture we will need the following information:

// Given a strand and the index of an LED upon it, return a color
// Within the Arduino code, we will have pre-computed tables that convert 
// [strand,index] to [edgeindex,x] where 0≤x≤1. 

// In this code, a "model" then, will be an array of values tied to a pre-computed and 
// organized list of edges. 
// {edgedata:[an array of edgemodels, each of which is [edgecoloringindex, direction]s, 
// presumable one for each element of edgegroup],

// (We keep the possibility of adding and coloring vertices for 
// debugging the geometry, but it's a different system 
// vertdata[an array of [index in the vert positions, vertcoloringindex]]}
// and for future work to align.)

// As it's a pain to type in 96 values, we'll further have a way of overlaying 
// structures on top of a blank model, further described below. 


var ourmodeldata
var ourmodels=[]

const blankModel = {name:"basic", edgedata:Array(96).fill([0,0])};
const basicmodel = blankModel

function addmodel(amodel){
    ourmodels = [...ourmodels,amodel]
}

function prependmodel(amodel){
    ourmodels = [amodel,...ourmodels]
}

function addmodels(models){
    ourmodels = [...ourmodels,...models]
}

addmodel(basicmodel)

addmodel({name:'twentyfourcell',edgedata:[
        [0,1],[0,1],[0,1],[0,1],[0,1],[0,1],
        [1,1],[1,1],[1,1],[1,1],[1,1],[1,1],
        [2,1],[2,1],[3,1],[2,1],[2,1],[2,1],
        [3,1],[3,1],[3,1],[3,1],[3,1],[3,1],
        [0,1],[0,1],[0,1],[0,1],[0,1],[0,1],
        [1,1],[1,1],[1,1],[1,1],[1,1],[1,1],
        [2,1],[2,1],[3,1],[2,1],[2,1],[2,1],
        [3,1],[3,1],[3,1],[3,1],[3,1],[3,1],
        [0,1],[0,1],[0,1],[0,1],[0,1],[0,1],
        [1,1],[1,1],[1,1],[1,1],[1,1],[1,1],
        [2,1],[2,1],[3,1],[2,1],[2,1],[2,1],
        [3,1],[3,1],[3,1],[3,1],[3,1],[3,1],
        [0,1],[0,1],[0,1],[0,1],[0,1],[0,1],
        [1,1],[1,1],[1,1],[1,1],[1,1],[1,1],
        [2,1],[2,1],[3,1],[2,1],[2,1],[2,1],
        [3,1],[3,1],[3,1],[3,1],[3,1],[3,1]
    ],vertdata:[[22,0],[1,2],[3,4],[5,6],
              //  [23,1],[0,3],[2,5],[4,7]
            ]})



// Given  { 
//      {indices:[],edgemodel}
//                    }
//  overlay this onto a model

function makemodel(modelinfo,oldmodel = basicmodel){
    var newmodel = structuredClone(oldmodel)
    newmodel.name = modelinfo.name
    modelinfo.listofindexandcolorlists.map(indexandcolorlist=>indexandcolorlist.indices.map(index=>{newmodel.edgedata[index]=indexandcolorlist.modelinfo}))
    return newmodel
}


const octahedron = makemodel({name:'firstoctahedron', 
    listofindexandcolorlists:[
        {indices:[95,91,70,34],modelinfo:[1,1]},
        {indices:[ 29, 43, 66, 83],modelinfo:[3,1]},
        {indices:[21,51,62],modelinfo:[2,1]},

        {indices:[87],modelinfo:[2,-1]}
    ]})


 //ourmodels = [octahedron,...ourmodels]
    addmodel( octahedron)

var themodel = permutenamedmodel('firstoctahedron',new qAction(qOne, qW))

prependmodel(themodel)

