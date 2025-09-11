
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
 
 // let geometricObjects=[]; //NEED handling/removing/disposing of three js objects
  //currently, everything is being dumped into geometricObjects and somethings into
  //special bins, such as qAxes, CayleyGraph


  //Garbage removal is poor
  //BIG MEMORY LEAK!!!
  
  
  // coordinates are 1 = 1 at the equatorial sphere
  
 
  function initthethreejsscene() {


    container = document.querySelector( '#scene-container' );

    scene = new THREE.Scene();
    scene.background = new THREE.Color( 0x170630 );

   
   
    createLights(); // need to create before camera, to attach
    createCamera();
    createControls();
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

      9, // near clipping plane
      40, // far clipping plane
    );

    camera.position.set(1, 4, 10);
    camera.add(mainlight);
    mainlight.position.set( 1,4,2 );
    camera.lookAt(new THREE.Vector3(0,0,0));
    scene.add( camera );
    

  }
  

  function createControls() {

    controls = new THREE.OrbitControls( camera, container ); 
    camera.position.set( 14, .4, 2.3 );
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
  



