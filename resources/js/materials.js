
////////////////////////////////////
///
///
///  Materials
///
///


 const NUMMATS = 36;




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


// further material. 

// Colorable material: 
const colorableMaterial = new THREE.MeshLambertMaterial({ 
            vertexColors: true,
            transparent: true,
            side: THREE.DoubleSide,
            alphaTest: 0.1 // Helps with rendering transparent surfaces

        });






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
