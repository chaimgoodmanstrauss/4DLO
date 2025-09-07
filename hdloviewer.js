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



ourguiparams['the model'] = Object.keys(ourmodels)[0];
ourgui.add(ourguiparams,'the model',
	Object.keys(ourmodels)).onChange(theModelChanged);


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

	controls.update();
	}}





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

// this controls whether just a single test edge is shown, or all of them.
ourguiparams['show all edges'] = true;
ourgui.add(ourguiparams,'show all edges').onChange(theModelChanged);

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



function setupthemeshes(){
// set up some generic meshes, to be run at initialization. 


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









function updatethedrawing(){
	var offset = ourguiparams['the offset']
	offset =qOne.positivize();

	ourmodeldata = ourmodels[ourguiparams['the model']];

	// hide all of the meshes in case they're showing. 
	for(var meshindex = 0; meshindex<numourmeshes; meshindex++){
		ourmeshregistry[meshindex].visible = false;
	}

	var numedges = Math.min(ourmodeldata.edgedata.length,numedgemeshes,edgegroup.length) 
	var numverts = Math.min(ourmodeldata.vertdata.length,numvertmeshes,vertgroup.length) 

	var kindofmesh = 'edge'
	
	var e0,e1;
	for(var meshindex = 0; meshindex<numourmeshes;meshindex++)
	{	
		if(meshindex<numedges){
			ourmeshregistry[meshindex].visible= true;
			kindofmesh = 'edge'
			var edgeindex= meshindex
			var m = edgegroup[edgeindex]
			
			if(ourguiparams['Multiply the motion on the']=='left'){
				e0 = offset.mult(m.acton(edgebase0));
				e1 = offset.mult(m.acton(edgebase1));
			}
			else {
				e0 = (m.acton(edgebase0)).mult(offset);
				e1 = (m.acton(edgebase1)).mult(offset);
			}
		


			if(ourguiparams['show as']=='four-d'){
			ourmeshregistry[edgeindex] = rejiggertubeArc(ourmeshregistry[edgeindex],
				e0, e1,.03,false,colorableMaterial,true);
			}
			else{
				ourmeshregistry[edgeindex] = rejiggertubeArc(ourmeshregistry[edgeindex],
				e0, e1,.03,false,colorableMaterial,false);
			}
			
			// now color all of the vertices on the edge. 
			for(var i = 0; i<500; i++)
				{	//var s = Math.random();
					ourmeshregistry[edgeindex].geometry.attributes.color.array.set(
						edgecolorfunction((i%50)/50,ourmodeldata.edgedata[edgeindex]),i*4)}
		

			ourmeshregistry[meshindex].geometry.attributes.position.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.attributes.color.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.computeVertexNormals(); // Recalculate normals for proper lighting
		



		}
		// and that's the edges


		else if(meshindex>=numedgemeshes && meshindex<numedgemeshes+numverts) // The vertices:
		{ 	ourmeshregistry[meshindex].visible = true;
			kindofmesh = 'vertex';
			var vindex = meshindex - numedgemeshes;
			var vertdataindex = ourmodeldata.vertdata[vindex][0] ;
			var vert =vertices[vertdataindex];
			if(ourguiparams['Multiply the motion on the']=='left'){
				vert = offset.mult(vert);
			}
			else {
				vert = (vert).mult(offset);
			}

			ourmeshregistry[meshindex]=rejiggersphere(ourmeshregistry[meshindex],
				mapQToWorld(vert),.1,false,mats[0],false)

			// now color all of the vertices on the vertex 
			for(var i = 0; i<500; i++)
				{	ourmeshregistry[meshindex].geometry.attributes.color.array.set(
						[[0,0,0,1],[1,0,0,1],[0,1,0,1],[0,0,1,1]][vindex],
					// hsbToRgb((meshindex%4)/4,1,1),
						  i*4)}
		



			ourmeshregistry[meshindex].geometry.attributes.position.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.attributes.color.needsUpdate = true;
			ourmeshregistry[meshindex].geometry.computeVertexNormals(); // Recalculate normals for proper lighting
		
		
			
		}



	}


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
           // console.log('h');
            updatethedrawing();}

animate();

