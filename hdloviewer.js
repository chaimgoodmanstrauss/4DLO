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
// init the gui for the HDLO viwer
//
// This comes from the lil-gui library

// When any parameter is changed, we send the action to theModelChanged(), at the end of this file. 

 

import GUI from './resources/lib/lilgui.js';
import {FrequencyAnalyzer,} from './sound.js'
const ourFrequencyAnalyzer=new FrequencyAnalyzer();

var ourguiparams = {};

var ourgui = new GUI();

// to add a parameter, add its key to the ourguiparams dictionary, and then ourgui.add(it).
// See the documentation for lots of ways to use this. 

/*ourguiparams['sphere radius']=.03
ourgui.add(ourguiparams,'sphere radius',.01,.2).onChange(theModelChanged);

ourguiparams['transparency with height']=.2;
ourgui.add(ourguiparams,'transparency with height',0,1).onChange(theModelChanged);
*/

ourguiparams['show as'] = 'four-d';
ourgui.add(ourguiparams,'show as',
	['four-d','three-d']).onChange(thecamerachanged);



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







/////////////////////

//
//	
//


/*
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
*/




///////////////////////////////////////
// 
// SCENE AND MESH MANAGMENT FOR HDLO
//

// Because of improper memory management in the particular version
// of threejs, we will keep the meshes we need on hand, simply moving
// them into place as needed. 
// Different models will use different parts of the array.

const numourmeshes = 96; // The 1IJK 24-cell takes 96 edges in one 24-cell. 
	// as we need to, let's just update this.

var ourmeshregistry = [];// these are all of the meshes

//// colors for HDLO 

var defaultmeshcolors = Array(4*10*50).fill(.5);
	//note the size of the array, geared to the defaults
	// in tubeArc in quaternionicdisplay

var redmeshcolor = new Float32Array(fillarraywithrgba(1,0,0,1,10*50))
var greenmeshcolor = new Float32Array(fillarraywithrgba(0,1,0,1,10*50))
var bluemeshcolor = new Float32Array(fillarraywithrgba(0,0,1,1,10*50))
var graymeshcolor = new Float32Array(fillarraywithrgba(.5,.5,.5,1,10*50))



/////////////////////////////////////////
////////////////////////
/* set up the meshes */





function setupthemeshes(){


// set up some generic meshes

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



const CosetModelData = {'332 cayley graph':
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
		[["1111","1000",1],["1000","111-",1],,["1000","11-1",1],,["1000","1-11",1]]
	},//end of '24-cell'
'16-cell':
{
	vertmotions: makegroup([
		new qAction(qI,qOne), new qAction(qJ,qOne)]).groupElements, 
	edgemotions: makegroup([
		new qAction(qI,qOne),new qAction(qW,qW)]).groupElements,
	vertexbasepoints:[qOne,qW],
	edges:[["1000","0100",1]]

},//end of 16-cell
	
}


var ourmodeldata

function theModelChanged(){
	ourmodeldata =  CosetModelData[ourguiparams['the model']]
	updatethedrawing() // all the actual three.js object handling
}






function updatethedrawing(){
	



	// hide all of the meshes in case they're showing. 
	for(var i = 0; i<numourmeshes; i++){
		ourmeshregistry[i].visible = false;
	}


	// first update the positions of all of the tube meshes, based on how they
	// are placed by the UI

	// We compute the model data the first time a model is called, from a list
	// 
	// and then store this as an array of {indices to the color functions, 0/-/+ for flipping the color function around.
	// Initially, these indices are just from the cosets; soon we will be sculpting. 

	// then we update the colors


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
		
		if(ourguiparams['show as']=='four-d'){
		ourmeshregistry[edgeindexcount] = rejiggertubeArc(ourmeshregistry[edgeindexcount],
			ends[0], ends[1],.03,false,colorableMaterial,true);
		}
		else{
			ourmeshregistry[edgeindexcount] = rejiggertubeArc(ourmeshregistry[edgeindexcount],
			ends[0], ends[1],.03,false,colorableMaterial,false);
		}
		
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

// Somewhere, there needs to be an init();


initthethreejsscene()
let  materials = createMaterials()
    
setupthemeshes();

	// now draw the drawing for the first time
theModelChanged();

function animate() {
            requestAnimationFrame(animate);
            console.log('h');
            updatethedrawing();}

animate();

