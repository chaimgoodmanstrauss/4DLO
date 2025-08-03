
///////////
///  Set up the three.js environment

  // these need to be accessed inside more than one function so we'll declare them first
  let container;
  let camera;
  let renderer;
  let scene;
  let mesh;
  let meshes = [];
  let controls;
  let mainLight;
  let materials;
 // let geometricObjects=[]; //NEED handling/removing/disposing of three js objects
  //currently, everything is being dumped into geometricObjects and somethings into
  //special bins, such as qAxes, CayleyGraph


  //Garbage removal is poor
  //BIG MEMORY LEAK!!!
  
  
  // coordinates are 1 = 1 at the equatorial sphere
  
  const NUMMATS = 36;

  function init() {


    container = document.querySelector( '#scene-container' );

    scene = new THREE.Scene();
    scene.background = new THREE.Color( 0x8FBCD4 );

   
   
    createLights(); // need to create before camera, to attach
    createCamera();
    createControls();
    materials = createMaterials()
    geometries = createGeometries();// this includes the basic mesh and
    // basic sphere.

   
   
   
    createRenderer();

//TO DO: HOW DOES STEREO WORK??!

    //effect = new THREE.StereoEffect( renderer );
   // effect.setSize( window.innerWidth, window.innerHeight );
    
  //  makeClippingSphere();
    
   renderer.setAnimationLoop( () => {

      update();
      render();

    } );
    
   // render();
    
    onWindowResize();

  }

  function createCamera() {

    camera = new THREE.PerspectiveCamera(
      15, // FOV
      container.clientWidth / container.clientHeight, // aspect

      .001, // near clipping plane
      100, // far clipping plane
    );

    camera.position.set(1, 4, 10);
    camera.add(mainlight);
    mainlight.position.set( 1,4,2 );
    camera.lookAt(new THREE.Vector3(0,0,0));
    scene.add( camera );
    

  }
  

  function createControls() {

    controls = new THREE.OrbitControls( camera, container ); 
    camera.position.set( 11, 2, 4 );
    controls.update();
    
    //controls.addEventListener( 'change', render );
     }
    // these will have to be modified: how to get other behavior from the mouse?

  function createLights() {

     // const ambientLight = new THREE.AmbientLight( 0xffffff, .2 );
     //   scene.add( ambientLight );
        
      const ambientLight = new THREE.HemisphereLight(
        0xddeeff, // sky color
        0x202020, // ground color
        5, // intensity
      );
        
        // Create a directional light
           mainlight = new THREE.DirectionalLight( 0xffffff, 2.0 );

           mainlight.position.set( 0,0,0 );
    // this will be attached to the camera, and then moved
  }



  
  
  function createRenderer() {

    renderer = new THREE.WebGLRenderer( { antialias: true } );
    renderer.setSize( container.clientWidth, container.clientHeight );

    renderer.setPixelRatio( window.devicePixelRatio );

    renderer.gammaFactor = 2.2;
    renderer.gammaOutput = true;

    container.appendChild( renderer.domElement );

  }
  
  // perform any updates to the scene, called once per frame
  // avoid heavy computation here
  function update() {
    
   // meshes[0].rotation.x += 0.001; 
   // meshes[0].rotation.y += 0.001;
   

  }
  
 /* function animate(){
    update();
    render();
  }*/
  function render(){
      // render, or 'create a still image', of the scene
      renderer.render( scene, camera );
  }
  
  // a function that will be called every time the window gets resized.
  // It can get called a lot, so don't put any heavy computation in here!
  function onWindowResize() {

    // set the aspect ratio to match the new browser window aspect ratio
    camera.aspect = container.clientWidth / container.clientHeight;


    // update the camera's frustum
    camera.updateProjectionMatrix();

    // update the size of the renderer AND the canvas
    renderer.setSize( container.clientWidth, container.clientHeight );

  }


 


  window.addEventListener( 'resize', onWindowResize );
  



  // then call the animate function to render the scene
 // animate();





 
////////////////////////////////////
///
///
///  Geometry and materials
///
///






  function createMaterials() {


    mats = {}
// need more refined!
  mats["specialMat2"] = new THREE.MeshStandardMaterial({
    color:0xddddddee,
    flatShading:true,
    name:'special mat 2'
  })


mats["specialMat1"] = new THREE.MeshPhongMaterial({
    color:0xffffff,
    opacity: 0.1,
    transparent: true,
    side: THREE.DoubleSide,
    name:'special mat 1'
  });


mats["grayMat"] = new THREE.MeshPhongMaterial({
    color:0xaaaaaa,
    name:"grayMat"});

mats["specialvertexcolor"] = new THREE.MeshStandardMaterial({
  color:0xabc125,
  name:"specialvertexcolor",
  vertexColors: THREE.VertexColors
})



//fix this up
    const mat1 = new THREE.MeshStandardMaterial( {
      color: 0xff3333, // red
      flatShading: true,
      name:'red'
    } );

    // just as with textures, we need to put colors into linear color space
    mat1.color.convertSRGBToLinear();

    const mat2 = new THREE.MeshStandardMaterial( {
      color: 0x808000, 
      flatShading: true,
      name:'something'
    } );

    mat2.color.convertSRGBToLinear();
    
  
//NUMMATS is set to 36 above
    let i = 0;
    for(i=0;i<NUMMATS; i++){
      let name = "mat"+i.toString();
      mats[name]= new THREE.MeshStandardMaterial( {
        color: 'hsl('+(((360/NUMMATS)*i)%360).toString()+',100%,50%)',
        flatShading: true,
      } );
    }

  
    return mats;
  }



createMaterials()


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

      function createPlanes( n ) {

        // creates an array of n uninitialized plane objects

        var result = new Array( n );

        for ( var i = 0; i !== n; ++ i )
          result[ i ] = new THREE.Plane();

        return result;

      }

      function assignTransformedPlanes( planesOut, planesIn, matrix ) {

        // sets an array of existing planes to transformed 'planesIn'

        for ( var i = 0, n = planesIn.length; i !== n; ++ i )
          planesOut[ i ].copy( planesIn[ i ] ).applyMatrix4( matrix );

      }

      function cylindricalPlanes( n, innerRadius ) {

        var result = createPlanes( n );

        for ( var i = 0; i !== n; ++ i ) {

          var plane = result[ i ],
            angle = i * Math.PI * 2 / n;

          plane.normal.set(
            Math.cos( angle ), 0, Math.sin( angle ) );

          plane.constant = innerRadius;

        }

        return result;

      }

      var planeToMatrix = ( function () {

        // creates a matrix that aligns X/Y to a given plane

        // temporaries:
        var xAxis = new THREE.Vector3(),
          yAxis = new THREE.Vector3(),
          trans = new THREE.Vector3();

        return function planeToMatrix( plane ) {

          var zAxis = plane.normal,
            matrix = new THREE.Matrix4();

          // Hughes & Moeller '99
          // "Building an Orthonormal Basis from a Unit Vector."

          if ( Math.abs( zAxis.x ) > Math.abs( zAxis.z ) ) {

            yAxis.set( - zAxis.y, zAxis.x, 0 );

          } else {

            yAxis.set( 0, - zAxis.z, zAxis.y );

          }

          xAxis.crossVectors( yAxis.normalize(), zAxis );

          plane.coplanarPoint( trans );
          return matrix.set(
            xAxis.x, yAxis.x, zAxis.x, trans.x,
            xAxis.y, yAxis.y, zAxis.y, trans.y,
            xAxis.z, yAxis.z, zAxis.z, trans.z,
            0,   0, 0, 1 );

        };

      } )();

function makeClippingSphere(){
  /*const geo = new THREE.SphereBufferGeometry(1.4,10,10);
  
  let positions  = geo.attributes.position.array;
  let vertices=[];
  let indices = geo.index.array;
  let ptN = positions.length / 3;
      for (let i = 0; i<ptN ; i++)
      {
          let p = new THREE.Vector3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
          vertices.push(p) 
      }
   */
  let vertices = [new THREE.Vector3(-1,-1,-1),new THREE.Vector3(1,1,-1),new THREE.Vector3(1,-1,1),new THREE.Vector3(-1,1,1),]
  let indices = [0,1,2,1,0,3,2,1,3,0,2,3]
  //indices = [3,2,0,3,1,2,3,0,1,2,1,0]
     
  let globalClippingPlanes = planesFromMesh(vertices,indices)
 // let PlaneMatrices = Planes.map( planeToMatrix ),

  //GlobalClippingPlanes = cylindricalPlanes( 5, 3.5 ), Empty = Object.freeze( [] );

  
  /*clipMaterial = new THREE.MeshPhongMaterial( {
    color: 0xee0a10,
    shininess: 100,
    side: THREE.DoubleSide,
    // Clipping setup:
    clippingPlanes: createPlanes( Planes.length ),
    clipShadows: true
    } );*/

   // globalClippingPlanes = createPlanes( GlobalClippingPlanes.length );
   
   // renderer.clippingPlanes = globalClippingPlanes;
    
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

 /* function createMeshes() {

    // our model
    const symmetricalObject = new THREE.Group();
    scene.add( symmetricalObject );

    
    const asphere = new THREE.Mesh( geometries.sphere, materials.mat1 );
    const acylinder = new THREE.Mesh( geometries.cylinder, materials.mat2 );
    
    const newsp = makesphereAt([0,.4,.5],.3, materials.mat1);
    const newsp2 = makesphereAt([0,0,0], .4, materials.mat2);
  //  symmetricalObject.add(newsp,newsp2
     //acylinder,
    //  asphere,

   // );
    
  //  meshes.push(symmetricalObject);
 // scene.add(symmetricalObject)

  }*/
  
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






// Somewhere, there needs to be an init();

init()
 
