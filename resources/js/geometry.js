////////////////////////////////////
///
///
///  Geometry
///
///


 function planesFromMesh( vertices, indices ) {

        // creates a clipping volume from a convex triangular mesh
        // specified by the arrays 'vertices' and 'indices'

        var n = indices.length / 3,
          result = new Array( n );

        for ( var i = 0, j = 0; i < n; ++ i, j += 3 ) {

          var a = vertices[ indices[ j +2 ] ],
            b = vertices[ indices[ j + 1 ] ],
            c = vertices[ indices[ j + 0 ] ];

          result[ i ] = new THREE.Plane().
            setFromCoplanarPoints( a, b, c );

        }

        return result;

      }

      

  
// these are created during the initialization process and are 
// available throughout

const ourmeshwidth = 10;
const ourmeshheight= 50;

function createGeometries() {

    const sphere = new THREE.SphereBufferGeometry(1,40,40); 

    const cylinder = new THREE.CylinderBufferGeometry(.2,.2, 20, 32);

    // In a somewhat more complicated manner, we'll create a mesh,
    // fixed for now at 10x50.
    var vertArray=[],  faceArray = [];
    
    // First, work out some simple  verts, to be moved about as needed
    var iN = ourmeshwidth, jN = ourmeshheight;
    for(var i = 0; i<iN; i++){
      for(var j=0;j<jN;j++){
        vertArray.push(...[i,j,0]); 
    }}

    // And then the connectivity
    for(var i = 0; i<iN-1; i++){
      for(var j=0;j<jN-1;j++){
      var ip=i+1,jp=j+1 
      faceArray.push(//
        //
        //  ---
        //  |/|  // labeled w inn*Jn+jn in lower left, in usual orientation
        //  ---
        //
        //
            i*jN+j,   i*jN+jp,  ip*jN+jp, //lower right ccwise
        //    i*jN+jp,  i*jN+j,   ip*jN+jp, //lower right cwise
            ip*jN+jp, ip*jN+j,  i*jN+j,  // upper left ccwise
        //    ip*jN+j,  ip*jN+jp, i*jN+j) // upper left cwise
        )}
      }
      // hence there should be 4*iN*jN faces

    var vertices = new Float32Array(vertArray);
    var faces = new Uint16Array(faceArray); //presumes no looping
    var amesh = new THREE.BufferGeometry()
    
    amesh.addAttribute('position', new THREE.BufferAttribute(vertices, 3))
    //geometry.addAttribute('index', new THREE.BufferAttribute(indices, 3));
    //geometry.setIndex(new THREE.BufferAttribute(indices, 4));
    amesh.setIndex(new THREE.BufferAttribute(faces, 1))





    return {
      sphere,cylinder,amesh
    };

  }

 
  
  function makesphereAt(pos=[0,0,0],scale=1,material=materials.mat1){
    var thissphere= new THREE.Mesh(geometries.sphere, material); 
    thissphere.scale.x=scale;
    thissphere.scale.y=scale;
    thissphere.scale.z=scale;
    thissphere.position.set(...pos);
   // thissphere.scale.set(.4);
  return thissphere;
  }

  function moveasphere(asphere,pos = [0,0,0],scale = 1){ 
  // move a known sphere to a position
    asphere.scale.x=scale;
    asphere.scale.y=scale;
    asphere.scale.z=scale;
    asphere.position.set(...pos);
    return asphere;
  }

  
  function createLotsaSpheres(orbitsList){
    // organized as an array of {"ptslist":ptslist, "color":color}'s
    var mesh = [];
    orbitsList.map(x=>{var c = x.color; x.ptslist.map(y=>mesh.push(makesphereAt(y,{"color":c})))})
  }
  




///////////////

function meshFromsurfacefunction(
    surface,// a function inputting i and j returning a quat
    imin=0,imax=1, iN=10, //simply connected with iN, jN verts
    jmin=0,jmax=1,jN=10,
    material = materials.mat0,
    vertexmaterialfunction=1){
  var di = (imax-imin)/(iN-1), dj = (jmax-jmin)/(jN-1)
  var vertArray = [], faceArray = []
  var iNMax, jNMax
  
  //work out the verts in first pass
  for(var inn = 0; inn<iN; inn++)
  {
    for(var jn=0;jn<jN;jn++){
      var ip=inn+1,jp=jn+1
      vertArray.push(surface(inn*di+imin,jn*dj+jmin)) // at in*jN+jn 
    }
  }

   for(var inn = 0; inn<iN-1; inn++)
  {
    for(var jn=0;jn<jN-1;jn++){
      var ip=inn+1,jp=jn+1 
      faceArray.push(//
        //
        //  ---
        //  |/|  // labeled w inn*Jn+jn in lower left, in usual orientation
        //  ---
        //
        //
                inn*jN+jn,inn*jN+jp,ip*jN+jp, //lower right ccwise
                ip*jN+jp,ip*jN+jn,inn*jN+jn,  // upper left ccwise
            )
        }
      }
      // hence there should be 4*iN*jN faces
  
 
 
  {
    var stuff=[vertArray,faceArray]

    var vertices = new Float32Array(vertArray);
   // var indices = new Uint16Array(faceArray); //presumes no looping
    var geometry = new THREE.BufferGeometry()

    // Create vertex colors array with alpha channel (RGBA)
        const colors = [];
        for (let i = 0; i < iN*jN; i++) {
            colors.push(1, 0, 0, 1); // Start with red, full opacity, will be updated in animation
        }
   // stuff.push(vertices,indices)
    geometry.addAttribute('position', new THREE.BufferAttribute(vertices, 3))
    geometry.addAttribute('color', new THREE.Float32BufferAttribute(colors, 4)); // 4 components for RGBA
    geometry.computeVertexNormals();

    

    geometry.setIndex(faceArray);

    var mmesh = new THREE.Mesh(geometry, material)
    scene.add(mmesh)
    meshes.push(mmesh)
/*
    for(i = 0; i< faceArray.length; i++){
      mmesh.faces[ i ].vertexColors[ 0 ].setHSL( Math.random(), 0.5, 0.5 );

    }
    */mmesh.geometry.colorsNeedUpdate = true;
    return mmesh
  }

}



function revisemeshfromsurfacefunction(
    amesh,// a predetermined mesh; the lendth of its vertex array must equal 3*iN*jN
    surface,// a function inputting i and j returning a quat
    imin=0,imax=1, iN=10, //simply connected with iN, jN verts
    jmin=0,jmax=1,jN=10,
    material = materials.mat0,
    vertexmaterialfunction=1){
  var di = (imax-imin)/(iN-1), dj = (jmax-jmin)/(jN-1)
  var vertArray = [], faceArray = []
  var iNMax, jNMax
  var vcounter = 0
  var  positionAttribute = amesh.geometry.getAttribute( 'position' )
  
  //work out the verts in first pass
  for(var inn = 0; inn<iN; inn++)
  {
    for(var jn=0;jn<jN;jn++){
      var ip=inn+1,jp=jn+1
      //vertArray.push(...mapQToWorld(surface(inn*di+imin,jn*dj+jmin))) // at in*jN+jn 

      var p = mapQToWorld(surface(inn*di+imin,jn*dj+jmin))

      positionAttribute.setXYZ(vcounter, p[0],p[1],p[2]);
      vcounter++;
    }
  }
  
  amesh.material=material

  amesh.geometry.colorsNeedUpdate = true;
  return amesh
}



