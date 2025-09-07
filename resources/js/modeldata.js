
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

const edgegroup = 
/*[new qAction(qOne,qOne),
 
    new qAction(
        new quat(sqrt(.5),-sqrt(.5),0,0),
        new quat(sqrt(.5),-sqrt(.5),0,0)),
    new qAction(
        new quat(0,1,0,0),
        new quat(0,1,0,0)),
    new qAction(
        new quat(sqrt(.5),sqrt(.5),0,0),
        new quat(sqrt(.5),sqrt(.5),0,0)),

]*/
        


makegroup([
    new qAction(qI, qOne), 
    new qAction(qW,qOne),
    new qAction(new quat(sqrt(.5),0,-sqrt(.5),0),new quat(sqrt(.5),0,sqrt(.5),0)),
    
]).groupElements

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



var ourmodeldata


var ourmodels =  {twentyfourcell:{edgedata:[
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
            ]},
    testing:{edgedata:[[1,1],[2,-1]]},
    sixcycle:{edgedata:[[0,1],[0,1],[0,1],[0,1],[0,1],[0,1]]}

};


