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
// init the GUI for the HDLO viwer
//
// This comes from the lil-gui library

// When any parameter is changed, we send the action to theModelChanged(), at the end of this file. 

 

import GUI from './resources/lib/lilgui.js';
//import {FrequencyAnalyzer,} from './sound.js'
//const ourFrequencyAnalyzer=new FrequencyAnalyzer();

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
//ourgui.add(ourguiparams,'show as',['four-d','three-d']).onChange(thecamerachanged);


// which model should we display? 
// TBD: turn this into entire programs, add options for color ways, etc.

ourguiparams['the model'] = defaultmodel
//ourgui.add(ourguiparams,'the model',ourmodels.map(x=>x.name)).onChange(theModelChanged);

var displaymodelkeys = Object.keys(ourModelRegistry).filter(key => {
    // For each key, check if its value is an object and has a key `foo` with a value of `true`.
    // The optional chaining operator (`?.`) prevents errors if `obj[key]` is not an object.
    return ourModelRegistry[key]?.fordisplayQ === true;
  })

ourgui.add(ourguiparams,'the model',displaymodelkeys).onChange(theModelChanged);


//ourguiparams['color way']=defaultcolorway


ourguiparams['Reset the camera position'] = 'scrolling';
//ourgui.add(ourguiparams,'Reset the camera position',	['scrolling','1000-1111 axis','1000-1100 axis']).onChange(thecamerachanged);

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

	controls.update();
	}}


ourguiparams['show axes']=true;
ourgui.add( ourguiparams, 'show axes').onChange(theModelChanged)

// // these will remain switched as is for now:
//ourguiparams['Show pedagogy']=false
//ourgui.add( ourguiparams, 'Show pedagogy').onChange(theModelChanged);
// // and 
//ourguiparams['Shade by depth']=true
//ourgui.add( ourguiparams, 'Shade by depth').onChange(theModelChanged);


ourguiparams['offset 1 to I by'] = 0.1;
ourgui.add( ourguiparams, 'offset 1 to I by', 0,1).onChange(updateoffset); // and then call theModelChanged


ourguiparams['offset 1 to 1111 by'] = 0.1;
ourgui.add( ourguiparams, 'offset 1 to 1111 by', 0,1).onChange(updateoffset) // and then call theModelChanged



ourguiparams['Multiply the motion on the']='right'
//ourgui.add(ourguiparams,'Multiply the motion on the',['left','right']).onChange(theModelChanged);


function updateoffset(){
	var s = .5*3.1415*ourguiparams['offset 1 to I by']; // up to Pi
	var t = .5*2.0944*ourguiparams['offset 1 to 1111 by'];  //up to 2 Pi/3
	var offset = new quat(Math.cos(s),Math.sin(s),0,0);
	offset = offset.multby(new quat(Math.cos(t),Math.sin(t)/Math.sqrt(3),Math.sin(t)/Math.sqrt(3),Math.sin(t)/Math.sqrt(3)))
	ourguiparams['the offset']= offset

	theModelChanged()
}



ourguiparams['the offset']=new quat(1,0,0,0);

// this controls whether just a single test edge is shown, or all of them.
ourguiparams['show all edges'] = true;
//ourgui.add(ourguiparams,'show all edges').onChange(theModelChanged);

// when anything changes:

function theModelChanged(){
	// all group data will be precomputed. 
	updatethedrawing() // all the actual three.js object handling
}

//////////////////////////////////////








///////////////////////////////////////
// 
// SCENE AND MESH MANAGMENT FOR HDLO
//

// Because of improper memory management in the particular version
// of threejs, we will keep the meshes we need on hand, simply moving
// them into place as needed. 
// Different models will use different parts of the array.

const numedgemeshes = 96;// The 1IJK 24-cell takes 96 edges in one 24-cell. 
const numvertmeshes = 24;


const numourmeshes = numedgemeshes + numvertmeshes;//+numvertmeshes;
	// 0-95 are edges of the 24 cell.
	// for the moment, just a test sphere; will add the  
	// 96-119 are vertices of the 24 cell. 

var ourmeshregistry = [];

const materialregistry=[]



//////////////////////////////
//
// set up some generic meshes, to be run at initialization. 
//

function setupthemeshes(){


	for(var i = 0; i<numourmeshes; i++){
		var a = 3.141/2*i/numourmeshes;
		var s = Math.sin(a);
		var c = Math.cos(a);
		var q1 = new quat(s+Math.random(),s,c,c-Math.random());
		var q2 = new quat(c,c+Math.random(),-s,-s+Math.random());
		// give them the geometry of a tubeArc, for now
		ourmeshregistry[i] = tubeArc(q1 ,q2,.03,false,colorablematerial)
		ourmeshregistry[i].visible = false;
		ourmeshregistry[i].name = 'mesh'+i.toString()

		scene.add(ourmeshregistry[i])

		// let's keep track of the meshes so that we don't have a memory leak swapping in and out
		materialregistry[i]=ourmeshregistry[i].material
		
		
	}

}



function updatethedrawing(){
	var offset = ourguiparams['the offset']
	//offset =qOne.positivize();

	//ourmodeldata = ourmodels[ourguiparams['the model']];

	// Use the find() method to get the dictionary where foo is 'fee'
	var ourmodeldata = ourModelRegistry[ourguiparams['the model']]


	// hide all of the meshes in case they're showing. 
	for(var meshindex = 0; meshindex<numourmeshes; meshindex++){
		ourmeshregistry[meshindex].visible = false;
	}

	var numedges = 0,  numverts =0
	if(ourmodeldata.edgemodels){
		numedges = Math.min(ourmodeldata.edgemodels.length,numedgemeshes,edgegroup.length) }
	if(ourguiparams['show axes']
		){numverts = Math.min(standardverts.length, //constant for this implementation
			numvertmeshes,vertgroup.length) 
	}

	if (!ourguiparams["show all edges"]){numedges=1}

	var kindofmesh = 'edge'
	
	var e0,e1;
	for(var meshindex = 0; meshindex<numourmeshes;meshindex++)
	{	
		if(meshindex<numedges){
			ourmeshregistry[meshindex].visible= true;
			kindofmesh = 'edge'
			var edgeindex= meshindex
			//edgeindex = 79;
			var m = edgegroup[edgeindex]
			
			if(ourguiparams['Multiply the motion on the']=='left'){
				e0 = offset.mult(m.acton(edgebase0));
				e1 = offset.mult(m.acton(edgebase1));
			}
			else {
				e0 = (m.acton(edgebase0)).mult(offset);
				e1 = (m.acton(edgebase1)).mult(offset);
			}
		
			var tuberadius =DEFAULT_TUBE_RADIUS;
			if(ourmodeldata.edgemodels[edgeindex].coloringfunctionname=='blank'){tuberadius = SMALL_TUBE_RADIUS}

			if(ourguiparams['show as']=='four-d'){
			ourmeshregistry[edgeindex] = rejiggertubeArc(ourmeshregistry[edgeindex],
				e0, e1,tuberadius,false,colorablematerial,true);
			}
			else{// TBD; redo this as regular sized tubes
				ourmeshregistry[edgeindex] = rejiggertubeArc(ourmeshregistry[edgeindex],
				e0, e1,tuberadius,false,colorablematerial,false);
			}
			
			// now color all of the vertices on the edge. If ourmodeldata.edgedata[edgeindex] == 0, 
			// switch materials. 
			switch(ourmodeldata.edgemodels[edgeindex].coloringfunctionname){
				case 'blank':// we reserve this index for the basic material.
				case 0:
					ourmeshregistry[edgeindex].material= transparentlightmaterial
					break
				default: 
					ourmeshregistry[edgeindex].material= materialregistry[edgeindex]//bring our material in (this should always be the same one in any case)
					for(var i = 0; i<500; i++)
					{	
						var stepnumber = (i%50)/50 // the tubes are ten around and 50 long, 
						// but the vertices are numbered lengthwise. i%50/50 is the length 
						// along a tube
						var colorvalue = ourmodeldata.evaluate(edgeindex,stepnumber);
						// if coloringfunctionname is an int,
						// then we will be applying defaultcoloringfunctions[coloringfunctionname]


						ourmeshregistry[edgeindex].geometry.attributes.color.array.set(colorvalue,i*4)}

			}
			

			ourmeshregistry[meshindex].geometry.attributes.position.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.attributes.color.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.computeVertexNormals(); // Recalculate normals for proper lighting
		



		}
		// and that's the edges


		else if(meshindex>=numedgemeshes && meshindex<numedgemeshes+numverts) // The vertices:
		{ 	ourmeshregistry[meshindex].visible = true;
			kindofmesh = 'vertex';
			var vindex = meshindex - numedgemeshes;
			var vdata = standardverts[vindex];// using the standard verts for now.
			// TBD: add this to the model, as it was in earlier versions of the code.
			
			var vert = vdata[0] 
			if(ourguiparams['Multiply the motion on the']=='left'){
				vert = offset.mult(vert);
			}
			else {
				vert = (vert).mult(offset);
			}

			var vertradius = .06

			ourmeshregistry[meshindex]=rejiggersphere(ourmeshregistry[meshindex],
				mapQToWorld(vert),vertradius,false,mats[0],false)

			// now color all of the vertices on the vertex 
			for(var i = 0; i<500; i++)
				{	ourmeshregistry[meshindex].geometry.attributes.color.array.set(
						ourColorFunctionRegistry[vdata[1]](),
					// hsbToRgb((meshindex%4)/4,1,1),
						  i*4)}
		



			ourmeshregistry[meshindex].geometry.attributes.position.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.attributes.color.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.computeVertexNormals(); // Recalculate normals for proper lighting
		
		
			
		}



	}


}




initthethreejsscene()
let  materials = createMaterials()
    
setupthemeshes();

	// now draw the drawing for the first time
theModelChanged();



////////////
// If we are printing out the models to a file that 
// the teensy hdlo controller manages, uncomment this 
// (which is defined and managed around line 720 of modeldata.js)

///writeModelsToFile(ourModelRegistry)

// or comment to turn this off

function animate() {
            requestAnimationFrame(animate);
           // console.log('h');
            updatethedrawing();}

animate();

