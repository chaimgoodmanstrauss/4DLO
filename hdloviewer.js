///////////////////////////
//
//
//
//
//  This program shows a family of symmetries of the hypersphere (S^3, the 3-sphere).
// The points of S^3 are unit quaternions -- see quaternions.js for the underlying code to manipulate and use these),
// These are  stereographically projected into our own three-dimensional space, R^3, with 1 sent to the origin (0,0,0)
// and I, J and K are sent to (1,0,0), (0,1,0) and (0,0,1) respectively.

// Quaternions with no real component are called imaginary. The unit imaginary quats all lie on a 
// unit three-dimensional sphere (that is, the 2-sphere) and this is stereographically projected to the 
// unit sphere in R^3. 

// We abbreviate, for example, writing for the unit quaternion 1/2 + I/2 + J/2 + K/2 = quat(1,1,1,1).normalize()
// simply 1111

// All of the symmetries we show are subgroups of pm [OxO], with orbits of edges connecting 1  either with 1111 
// or with 1100 -- that is, along an edge of the 24-cell/hypercube or an edge of the 16-cell.

 


//////////////////////////////
//////////////////////////////
//
// init the gui
//
// This comes from the lil-gui library

// When any parameter is changed, we send the action to theModelChanged(), at the end of this file. 

 

import GUI from './resources/lib/lilgui.js';

var ourguiparams = {};

var ourgui = new GUI();

// to add a parameter, add its key to the ourguiparams dictionary, and then ourgui.add(it).
// See the documentation for lots of ways to use this. 

ourguiparams['sphere radius']=.03
ourgui.add(ourguiparams,'sphere radius',.01,.2).onChange(theModelChanged);

ourguiparams['transparency with height']=.2;
ourgui.add(ourguiparams,'transparency with height',0,1).onChange(theModelChanged);


ourguiparams['the model'] = '332 cayley graph';
ourgui.add(ourguiparams,'the model',
	['332 cayley graph',
		'24-cell',
		'16-cell',
		'two 24-cells','a hypercube']).onChange(theModelChanged);


ourguiparams['Reset the camera position'] = 'scrolling';
ourgui.add(ourguiparams,'Reset the camera position',
	['scrolling','1000-1111 axis','1000-1100 axis']).onChange(thecamerachanged);



function thecamerachanged(){
	switch (ourguiparams['Reset the camera position']){
	case 'scrolling':break;
	case '1000-1111 axis':
		camera.position.set(10,10,10);

		controls.update();
		ourguiparams['Reset the camera position']='scrolling'
		break;
	case '1000-1100 axis':
		camera.position.set(17,0,0);
		controls.update();
		ourguiparams['Reset the camera position']='scrolling'
		break;

	//controls.target.set(0,0,0);
	controls.update();
	}}





// // these will remain switched as is for now:
ourguiparams['Show pedagogy']=false
//ourgui.add( ourguiparams, 'Show pedagogy').onChange(theModelChanged);
// // and 
ourguiparams['Shade by depth']=true
//ourgui.add( ourguiparams, 'Shade by depth').onChange(theModelChanged);


ourguiparams['offset 1 to I by'] = 0.1;
ourgui.add( ourguiparams, 'offset 1 to I by', 0,1).onChange(updateoffset); // and then call theModelChanged


ourguiparams['offset 1 to 1111 by'] = 0.1;
ourgui.add( ourguiparams, 'offset 1 to 1111 by', 0,1).onChange(updateoffset) // and then call theModelChanged



ourguiparams['Multiply the motion on the']='right'
ourgui.add(ourguiparams,'Multiply the motion on the',
	['left','right']).onChange(theModelChanged);


function updateoffset(){
	var s = .5*3.1415*ourguiparams['offset 1 to I by']; // up to Pi
	var t = .5*2.0944*ourguiparams['offset 1 to 1111 by'];  //up to 2 Pi/3
	var offset = new quat(Math.cos(s),Math.sin(s),0,0);
	offset = offset.multby(new quat(Math.cos(t),Math.sin(t)/Math.sqrt(3),Math.sin(t)/Math.sqrt(3),Math.sin(t)/Math.sqrt(3)))
	ourguiparams['the offset']= offset

	theModelChanged()
}



ourguiparams['the offset']=new quat(1,0,0,0);

ourguiparams['show all edges'] = true;
ourgui.add(ourguiparams,'show all edges').onChange(theModelChanged);





//////////////////////////////
//////////////////////////////
//
// Init the quaternions etc needed 
// (move into so4subgroups etc.)
//

///////////////////////////////
// 

// recall from so4subgroups.js :

/*

const qOne = new quat(1,0,0,0)
const qW = new quat(-.5,.5,.5,.5) // note normalized
const qI = new quat(0,1,0,0)
const qJ = new quat(0,0,1,0)
const qK = new quat(0,0,0,1)
const qIco = new quat(0,.5,(sqrt(5)-1)/4, (sqrt(5)+1)/4)
const qO = new quat(0,0,sqrt(.5),sqrt(.5))
*/




const usefulPoints = {
	"1000":[1,0,0,0],"0100":[0,1,0,0],"0010":[0,0,1,0],"0001":[0,0,0,1],
	"0-00":[0,-1,0,0],"00-0":[0,0,-1,0],"000-":[0,0,0,-1],

	"1111":[1,1,1,1],  "111-":[1,1,1,-1], "11-1":[1,1,-1,1], "11--":[1,1,-1,-1],
	"1-11":[1,-1,1,1],"1-1-":[1,-1,1,-1],"1--1":[1,-1,-1,1],"1---":[1,-1,-1,-1],

	"-111":[-1,1,1,1],  "-11-":[-1,1,1,-1], "-1-1":[-1,1,-1,1], "-1--":[-1,1,-1,-1],
	"--11":[-1,-1,1,1],"--1-":[-1,-1,1,-1],"---1":[-1,-1,-1,1],"----":[-1,-1,-1,-1],

	"1100":[1,1,0,0],  "1010":[1,0,1,0],  "1001":[1,0,0,1],  "0110":[0,1,1,0],  "0101":[0,1,0,1],  "0011":[0,0,1,1],
	"1-00":[1,-1,0,0], "10-0":[1,0,-1,0], "100-":[1,0,0,-1], "01-0":[0,1,-1,0], "010-":[0,1,0,-1], "001-":[0,0,1,-1],
	"-100":[-1,1,0,0], "-010":[-1,0,1,0], "-001":[-1,0,0,1], "0-10":[0,-1,1,0], "0-01":[0,-1,0,1], "00-1":[0,0,-1,1],
	"--00":[-1,-1,0,0],"-0-0":[-1,0,-1,0],"-00-":[-1,0,0,-1],"0--0":[0,-1,-1,0],"0-0-":[0,-1,0,-1],"00--":[0,0,-1,-1],

}


var usefulQuats={};

(Object.keys(usefulPoints)).map(x=>{
	usefulQuats[x]=(new quat(...usefulPoints[x])).normalize()})



var cubekeys =[
	"1111","111-","11-1","11--","1-11","1-1-","1--1","1---",
	"-111","-11-","-1-1","-1--","--11","--1-","---1","----"]

var orthokeys = ["1000","0100","0010","0001","0-00","00-0","000-"]

var counterkeys = [
	"1100","1010","1001","0110","0101","0011",
	"-100","-010","-001","0-10","0-01","00-1",
	"1-00","10-0","100-","01-0","010-","001-",
	"--00","-0-0","-00-","0--0","0-0-","00--"]

var halfcubekeys = [
	"1111","111-","11-1","11--","1-11","1-1-","1--1","1---"]


var halforthokeys = ["1000","0100","0010","0001","0-00","00-0"]

var halfcounterkeys=[
	"1100","1010","1001","0110","0101","0011",
	                     "0-10","0-01","00-1",
	"1-00","10-0","100-","01-0","010-","001-",
	                     "0--0","0-0-","00--"]

var tetkeys = ["1111","11--","1-1-","1--1","-11-","-1-1","--11","----"]


var yellowpuzzlenodes=
["1000","0100","0010","0001","0-00","00-0","000-","0101","010-","0-01","0-0-","1010","10-0"]

var greenpuzzlenodes = ["0011","001-","00-1","00--","1111","1100","11--","1--1","1-00","1-1-"]

var redpuzzlenodes = ["0110","01-0","0-10","0--0","11-1","1001","1-11","1---","100-","111-"]
   
//bitstodraw=[halfcubekeys,halforthokeys,halfcounterkeys]

var bitstodraw=[yellowpuzzlenodes,greenpuzzlenodes,redpuzzlenodes]





/////////////////////

//
//	
//


function transparentbehindcamera(amesh, cameraposition, camerafocus){
 for (let i = 0; i < gridPositions.length; i++) {
                const x = gridPositions[i].x;
                const y = gridPositions[i].y;
                
                // Update height using sine wave
                const z = Math.sin(time + x + y) * 0.2;
                positions[i * 3 + 2] = z;
                
                // Update color: interpolate between red and blue based on time and x
                const colorFactor = (Math.sin(time * 2 + x * 4) + 1) * 0.5; // 0 to 1
                const red = 1 - colorFactor;   // Red component
                const blue = colorFactor;      // Blue component
                const green = 0;               // Keep green at 0
                
                // Update transparency: vary with time and y direction (more dramatic range)
                const alpha = (Math.sin(time * 1.5 + y * 3) + 1) * 0.35 + 0.3; // 0.3 to 1.0 (more visible variation)
                
                colors[i * 4] = red;      // R
                colors[i * 4 + 1] = green; // G
                colors[i * 4 + 2] = blue;  // B
                colors[i * 4 + 3] = alpha; // A (transparency)
            }
            
            // Mark geometry as needing update
            amesh.attributes.position.needsUpdate = true;
            amesh.attributes.color.needsUpdate = true;
            amesh.computeVertexNormals(); // Recalculate normals for proper lighting
}










///////////////////////////////////////
// 
// SCENE AND MESH MANAGMENT
//

// Because of improper memory management in the particular version
// of threejs, we will keep the meshes we need on hand, simply moving
// them into place as needed. 
// Different models will use different parts of the array.

const numourspheres = 50; // 0-47 reserved for the special vertices
const numourmeshes = 100; // each model takes its own


var oursphereregistry =[];// these are all of the spheres
var ourmeshregistry = [];// these are all of the 

var ourspherescaleregistry=[];// to keep track of size changes

var spherematerials = []



////////////////////////
/* set up the meshes */

const colorableMaterial = new THREE.MeshLambertMaterial({ 
            vertexColors: true,
            transparent: true,
            side: THREE.DoubleSide,
            alphaTest: 0.1 // Helps with rendering transparent surfaces

        });







var defaultspherecolors = Array(4*40*40).fill(.75);
	//note the size of the array, geared to the defaults
	// in makesphereAt in threestuff



var defaultmeshcolors = Array(4*10*50).fill(.5);
	//note the size of the array, geared to the defaults
	// in tubeArc in quaternionicdisplay


function fillarraywithrgba(r,g,b,a,n){
	var arr = []
	for(var i=0;i<n;i++){
		arr.push(r,g,b,a)
	}
	return arr
}

function fillarraywithrgb(r,g,b,n){
	var arr = []
	for(var i=0;i<n;i++){
		arr.push(r,g,b)
	}
	return arr
}


var redmeshcolor = new Float32Array(fillarraywithrgba(1,0,0,1,10*50))
var greenmeshcolor = new Float32Array(fillarraywithrgba(0,1,0,1,10*50))
var bluemeshcolor = new Float32Array(fillarraywithrgba(0,0,1,1,10*50))
var graymeshcolor = new Float32Array(fillarraywithrgba(.5,.5,.5,1,10*50))


/*var redmeshcolor = new Float32Array(fillarraywithrgb(1,0,0,10*50))
var greenmeshcolor = new Float32Array(fillarraywithrgb(0,1,0,10*50))
var bluemeshcolor = new Float32Array(fillarraywithrgb(0,0,1,10*50))
var graymeshcolor = new Float32Array(fillarraywithrgb(.5,.5,1,10*50))
*/




function setupthemeshes(){

/*for(var i = 0; i<numourspheres; i++){
	var center = (new quat(Math.random(),Math.random(),0*Math.random(),0*Math.random())).normalize()
	oursphereregistry[i] = qSphereInWorld(center,.1);//, colorableMaterial) // new THREE.mesh line 498 threestuff
	//new THREE.Mesh(geometries.sphere, amaterial); 
	oursphereregistry[i].material=colorableMaterial;
	ourspherescaleregistry[i]=1; //to keep track of size changes
	oursphereregistry[i].visible = false;
	oursphereregistry[i].name = 'sphere'+i.toString()
	oursphereregistry[i].geometry.addAttribute('color',new THREE.Float32BufferAttribute(defaultmeshcolors, 4)); // 4 components for RGBA
	//oursphereregistry[i].geometry.attributes.color.array=fillarraywithrgba(1,0,0,1,1600)
	scene.add(oursphereregistry[i])
}
*/

for(var i = 0; i<numourmeshes; i++){
	var a = 3.141/2*i/numourmeshes;
	var s = Math.sin(a);
	var c = Math.cos(a);
	var q1 = new quat(s+Math.random(),s,c,c-Math.random());
	var q2 = new quat(c,c+Math.random(),-s,-s+Math.random());
	ourmeshregistry[i] = tubeArc(
		q1 ,q2,.03,false,colorableMaterial)
	ourmeshregistry[i].visible = false;
	ourmeshregistry[i].name = 'mesh'+i.toString()
	
	scene.add(ourmeshregistry[i])
}
}



// DO NOT USE

//  Keeping track of what's in the scene
//  Each time the objects are added or deleted, update this registry.

var thescencesobjects =[];
var temporarymaterials = []


/*** WARNING: MEMORY LEAK!! ***/
function deletescenesobjects(){
	thescencesobjects.map(y=>{scene.remove(y)})
	thescencesobjects=[]
	temporarymaterials.map(y=>y.dispose())
}






///////////////////////////////////////
///////////////////////////////////////
//
// Good drawing commands 
//

/*
var cubeCornerSphs = cubeCorners.map(x=>{qSphereInWorld(x,.1,materials.mat7)})
var orthoCornerSphs = orthoCorners.map(x=>{qSphereInWorld(x,.1,materials.mat10)})
var counterCornerSphs = counterCorners.map(x=>{qSphereInWorld(x,.03,materials.mat30)})
*/






const allmodeldata = {'332 cayley graph':
	{vertmotions:makegroup([new qAction(qI,qOne),
			new qAction(qW,qOne)],"Oxone").groupElements,
	edgemotions:
			//makegroup(
			//	[new qAction(qI,qOne),new qAction(qOne,qI),new qAction(qW,qOne),new qAction(qOne,qW)],"Oxone"),
		makeGroupFromName("Oxone").groupElements,
	vertexbasepoints:[qOne],
	edges://each coset gets one; the motions flip these around
		[["1000","1111",1],["1000","111-",0],,["1000","11-1",2],,["1000","1-11",3]]
		},//end of '332 cayley graph'
'24-cell':
	{vertmotions:makegroup([new qAction(qI,qOne),
			new qAction(qW,qOne)],"Oxone").groupElements,
	edgemotions:
			//makegroup(
			//	[new qAction(qI,qOne),new qAction(qOne,qI),new qAction(qW,qOne),new qAction(qOne,qW)],"Oxone"),
		makeGroupFromName("Oxone").groupElements,
	vertexbasepoints:[qOne],
	edges://each coset gets one; the motions flip these around
		[["1000","1111",1],["1000","111-",1],,["1000","11-1",1],,["1000","1-11",1]]
	},//end of '24-cell'
'16-cell':
{
	vertmotions: makegroup([
		new qAction(qI,qOne), new qAction(qJ,qOne)]).groupElements, 
	edgemotions: makegroup([
		new qAction(qI,qOne),new qAction(qW,qW)]).groupElements,
	vertexbasepoints:[qOne,qW],
	edges:[["1000","0100"]]

},//end of 16-cell
	

}



var ourmodeldata

function theModelChanged(){
	 ourmodeldata =  allmodeldata[ourguiparams['the model']]// for the moment nothing happens here, but this is where the
	// group and other data is to be assembled
	
	/* */
	updatethedrawing() // all the actual three.js object handling
}



function edgecolorfunction(x,coset){
	var color
	switch(coset)
	{
		case 0: color = [x,0,0,1]; break
		case 1: color = [0,x,1-x,1]; break
		case 2: color = [0,.5+.5*Math.sin(6.14*x+(Date.now() * 0.001)),0,1]; break
		case 3: color = [.5+.5*Math.cos(6.14*x+(Date.now() * 0.001)),0,0,1]; break
	}
	return color
}





function updatethedrawing(){
	/* */
	//deletescenesobjects(); // we are no longer deleting our objects, but are updating them. 
	
	
	/*for(var i = 0; i<numourspheres; i++){
		oursphereregistry[i].visible = false;//only matters when the number of verts changes

	}*/

	for(var i = 0; i<numourmeshes; i++){
		ourmeshregistry[i].visible = false;//only matters when the number of verts changes

	}

		//draw spheres the vertices using modeldata: 

	/*var counter = 0;
	(ourmodeldata.vertmotions).forEach((m)=>{
		for(var i=0; i<ourmodeldata.vertexbasepoints.length;i++){
		var center;
		if(ourguiparams['Multiply the motion on the']=='left'){
			center = ourguiparams['the offset'].mult(m.acton(ourmodeldata.vertexbasepoints[i]))
			}
		else {
			center = (m.acton(ourmodeldata.vertexbasepoints[i])).mult(ourguiparams['the offset']);
			}	
	
		
		var dat=qSphereToWorld(center,.1,false);

		var d = dat.center
		
		oursphereregistry[counter].position.set(d[0],d[1],d[2]);
  		oursphereregistry[counter].scale.setScalar(ourguiparams['sphere radius']*30*dat.radius);

  		
  		oursphereregistry[counter].visible = true;
  		oursphereregistry[counter].name = 'sphere'+counter.toString()


		// next check to see if behind the camera, other color effects, etc

		oursphereregistry[counter].geometry.attributes.position.needsUpdate = true;
        oursphereregistry[counter].geometry.attributes.color.needsUpdate = true;
        oursphereregistry[counter].geometry.computeVertexNormals(); // Recalculate normals for proper lighting


		counter++;
		}});*/
	

// draw some edges

var motions
if(ourguiparams['show all edges']){motions = ourmodeldata.edgemotions}
	else {motions = [qIdentity];}

var edgeindexcount=0;

motions.forEach((m)=>{
//	var whichcoset = 0;
ourmodeldata.edges.forEach((e)=>
{	

//console.log('another edge',e,m,ourguiparams['the offset'])
// an edge has precomputed end points and a color:
	var ends=[];
	if(ourguiparams['Multiply the motion on the']=='left'){
		ends[0] = ourguiparams['the offset'].mult(m.acton(usefulQuats[e[0]]));
		ends[1] = ourguiparams['the offset'].mult(m.acton(usefulQuats[e[1]]));
	}
	else {
		ends[0] = (m.acton(usefulQuats[e[0]])).mult(ourguiparams['the offset']);
		ends[1] = (m.acton(usefulQuats[e[1]])).mult(ourguiparams['the offset']);
	}
	//var mats = [materials.mat0,materials.mat9,materials.mat15,materials.mat22]
	//var mat =mats[e[2]]
	

	ourmeshregistry[edgeindexcount] = rejiggertubeArc(ourmeshregistry[edgeindexcount],
		ends[0], ends[1],.03,false,colorableMaterial);
	
	//[redmeshcolor,bluemeshcolor,greenmeshcolor][edgeindexcount%4]

		//mats[edgeindexcount%4])

	ourmeshregistry[edgeindexcount].visible = true;

	//transparentlayerize(ourmeshregistry[edgeindexcount],camera.position)
	//ourmeshregistry[edgeindexcount].geometry.attributes.color.array=bluemeshcolor;

	for(var i = 0; i<500; i++)
		{	var s = Math.random();
			ourmeshregistry[edgeindexcount].geometry.attributes.color.array.set(edgecolorfunction((i%50)/50,e[2]),
			i*4)}
	
	ourmeshregistry[edgeindexcount].geometry.attributes.position.needsUpdate = true;
	ourmeshregistry[edgeindexcount].geometry.attributes.color.needsUpdate = true;
    ourmeshregistry[edgeindexcount].geometry.computeVertexNormals(); // Recalculate normals for proper lighting



	edgeindexcount++;


})
})


}



//const geometry1 = new THREE.SphereGeometry(.1, 32, 16 ); 
//const material1 = new THREE.MeshBasicMaterial( { color: 0xffff00 } ); 
//const sphere1 = new THREE.Mesh( geometry1, material1 ); scene.add( sphere1 );

setupthemeshes();

	// now draw the drawing for the first time
theModelChanged();

function animate() {
            requestAnimationFrame(animate);
            console.log('h');
            updatethedrawing();}

animate();

