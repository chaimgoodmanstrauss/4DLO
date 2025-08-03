//////////////////////
//
//  Quaternionic display
//
// This relies on three.js



// the World is cartesian, with the usual unit framing
// Q coords are in S^3 ~ unit quaternions
// We wish to map back and forth by 
// 1 <-> 000,  i<->100, j<->010, k<->001
// consequently, we are stereographically projecting from the south pole, 000(-1)

/*function mapQToWorld(r,i,j,k){
  return [i/(r+1),j/(r+1),k/(r+1)] 
}*/

function mapQToWorld(q){
  return [q.i/(q.r+1),q.j/(q.r+1),q.k/(q.r+1)] 
}

function mapWorldToQ(x,y,z){
  const D = 2/(x*x+y*y+z*z+1)
  return new quat ( D-1,x*D, y*D, z*D,)
}



//////////////////////
//
//
//  The main mesh function


// surface is a function taking in i and j and returning a quaternion

// mesh returns  a triangular mesh, surface applied to 
// the ij grid; (for now) the mesh is doubled and facets with one vert
// out of bounds (arb set to cube root 2) are ignored. All this should be 
// made controllable.

// checks in R3, not in S3
var BOUNDsqd = 1.11 //(tubes are ? how thick)


function inboundsQ(pt){return true;}// (pt[0]*pt[0]+pt[1]*pt[1]+pt[2]*pt[2]<BOUNDsqd)}
// for the moment, turning off all clipping


//for some reason the one from the box is giving trouble. 
// in time, this can be an octacubical mesh.
const defaultSphereCircumN = 20,
  defaultSphereLatN =20





function meshFromS3surface(
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
      vertArray.push(...mapQToWorld(surface(inn*di+imin,jn*dj+jmin))) // at in*jN+jn 
    }
  }

   for(var inn = 0; inn<iN-1; inn++)
  {
    for(var jn=0;jn<jN-1;jn++){
      var ip=inn+1,jp=jn+1 
      faceArray.push(//note both orientations; Really do need both
        //
        //  ---
        //  |/|  // labeled w inn*Jn+jn in lower left, in usual orientation
        //  ---
        //
        //
                inn*jN+jn,inn*jN+jp,ip*jN+jp, //lower right ccwise
              // inn*jN+jp,inn*jN+jn,ip*jN+jp, //lower right cwise
                ip*jN+jp,ip*jN+jn,inn*jN+jn,  // upper left ccwise
              // ip*jN+jn,ip*jN+jp, inn*jN+jn, // upper left cwise
            )
        }
      }
      // hence there should be 4*iN*jN faces
  
 
 if(faceArray.length>0) //TO DO: Check that there are some faces!
  {
    var stuff=[vertArray,faceArray]

    var vertices = new Float32Array(vertArray);
    var indices = new Uint16Array(faceArray); //presumes no looping
    var geometry = new THREE.BufferGeometry()
  
    stuff.push(vertices,indices)
    geometry.addAttribute('position', new THREE.BufferAttribute(vertices, 3))
    //geometry.addAttribute('index', new THREE.BufferAttribute(indices, 3));
    //geometry.setIndex(new THREE.BufferAttribute(indices, 4));
    geometry.setIndex(new THREE.BufferAttribute(indices, 1))


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



function revisemeshFromS3surface(
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







//////////////////////
//
//  spheres


function sphereOnS3ToWorld(d,a,b,c,e,threespaceradius=false){
  // following http://www.geom.uiuc.edu/docs/doyle/mpls/handouts/node33.html
  // XYZW on the unit sphere satisfy X^2+Y^2+Z^2+W^2 = 1
  // and some a X + b Y + c Z + d W + e = 0.
  // (in quats, XYZW = XI+YJ+ZK+W1)
  // Stereographically projecting from 000(-1)
  // we will obtain x,y,z; as there is a line through 000- XYZW and xyz0, we interpolate, writing
  // we have XYZW = t(xyz0)+(1-t)(000(-1))
  // and so 
  // a(tx)+b(ty)+c(tz)+d(t-1)=0
  
  // Taking Q = x^2+y^2+z^2, we have
  // 1+Q = 2/t and Q = (1-W)/(W+1), W = (1-Q)/(Q+1)
  // Consequently
  
  // 2ax + 2by+2cz+d(1-Q)+e(1+Q)=0 or
  // (x^2+y^2+z^2)(e-d)+2ax+2by+2cz+d+e=0
  
  // Completing the squares and scaling we have, with F=d+e
  // (x+2a/F)^2+(y+2b/F)^2+(z+2c/F)^2=(a^2+b^2+c^2+d^2-e^2)/F^2
  
  let F = e-d; 
  //console.log(F)
  if(abs(F)<SHORTEPSILON){
    return{"radius":NaN,"center":NaN}
  }
  else{ //console.log(sqrt(a*a+b*b+c*c+d*d-e*e)/abs(F))
    if(threespaceradius){return{"radius":threespaceradius,"center":[-a/F, -b/F, -c/F]}}
      else{
    return{"radius":sqrt(a*a+b*b+c*c+d*d-e*e)/abs(F),"center":[-a/F, -b/F, -c/F]}}}
 }

function qSphereToWorld(q,r,threespaceradius=false){
  // given a sphere with center at q and radius r, obtain the center and radius of a sphere 
  // this assumes that q is normalized and lies on S3
  return sphereOnS3ToWorld(q.r,q.i,q.j,q.k,-cos(r),threespaceradius)
}

function qSphereInWorld(q,r=.1,mat=materials.mat3,threespaceradius=false){
  var dat=qSphereToWorld(q,r,threespaceradius)
  var sph=makesphereAt(dat.center,dat.radius,mat)
  scene.add(sph)
 // geometricObjects.push(sph)
  return sph
}


function copyqSphereInWorld(asphere, oldradius, q,r=.1,mat=materials.mat3,threespaceradius=false){
  var dat=qSphereToWorld(q,r,threespaceradius)
  
  asphere.geometry.attributes.position = dat.center
  asphere.geometry.scale(dat.radius/oldradius);
  

  asphere.visible = true;

  return dat.radius
}







function qSphereMoveInWorld(q,r=.1,threespaceradius = false){
  var dat=qSphereToWorld(q,r,threespaceradius)
  var sph=moveasphere(dat.center,dat.radius)
  return sph
}


function getOrbitListOf(q,groupname){
  let gp = makeGroupFromName(groupname);
  return gp.groupElements.map(x=>mapQToWorld(q.actby(x)));
}


/*function createOrbitOf(q,groupname,size=.2,material=materials.mat1){
  let gp = makeGroupFromName(groupname);
  let orbit = gp.groupElements.map(x=>mapQToWorld(q.actby(x)));
  orbit.map(x=>{let sp = makesphereAt(x,size,material);scene.add(sp);})

  return orbit
}*/

function makeSphereData(basept=qOne,numSph=3,randomOffset = .2, startMatIndex=2, matStepIndex=4){
  let sphereData = [];
  let n = 0;
  let i = basept.i, j = basept.j,k=basept.k,r=basept.r;
  for(n=0;n<numSph;n++){
    let pos = new quat(
      r+(n/numSph)*randomOffset*(.5-random()),i+(n/numSph)*randomOffset*(.5-random()),
      j+(n/numSph)*randomOffset*(.5-random()),k+(n/numSph)*randomOffset*(.5-random())).normalize()
    sphereData.push({"size":.6/180*PI*((numSph-n-1)%numSph)+1/180*PI,"mat":materials["mat"+((n*matStepIndex+startMatIndex)%NUMMATS)],
    "pos":pos})
  }
  return sphereData}

// creates spheres at all pts in orbit of q, within 2 of origin in projection
function createOrbitOf(q,groupname,size=.2,material=materials.mat1){
  var gp = ourGroup
  let spheres =[];
  let orbit = gp.groupElements.map(x=>(q.actby(x)));
  orbit.map(x=>{
    let spdata = qSphereToWorld(x,size);
    if(spdata.center!=NaN && spdata.radius!=NaN){
      if(spdata.center[0]*spdata.center[0]+
        spdata.center[1]*spdata.center[1]+
        spdata.center[2]*spdata.center[2]<4 && spdata.radius<1){
      let sp = makesphereAt(spdata.center,spdata.radius,material);
      spheres.push(sp);
      scene.add(sp);}}})
  return spheres
}


///// Called from basicUI.js 
//    The spheres are placed into the scene at makesphereAt
sphereLists={};

function createOrbits(groupname,basept=basepoint, sphData=[1,0],newSphereData=false,){
  //sphData is options for makeSphereData
  var sdat;
  sdat = makeSphereData(basept,...sphData)
 var orbitSpheres = sdat.map(x=>createOrbitOf(x.pos,groupname,x.size,x.mat));
  if(sphereLists[basept]==undefined)
  {
    sphereLists[basept]=orbitSpheres;
  }
  else sphereLists[basept]=[...sphereLists[basept],...orbitSpheres]
}

function clearOrbits(){
  Object.keys(sphereLists).map(x=>(sphereLists[x].forEach(function(z){z.map(y=>{scene.remove(y)})})))
}

function addAxes(){
  var axesHelper = new THREE.AxesHelper( 5 );
  scene.add( axesHelper );
  return axesHelper}

var qAxes=[];
/*


function addQAxes(){
  if(addQAxes.children.length=0){ 
    
  }
  // else do nothing
  
}*/


/////
// Axes

function setUpQAxes(){
  qAxes.push(qSphereInWorld(qOne,.4,materials.mat7))
  qAxes.push(qSphereInWorld(qI,.4,materials.mat0))
  qAxes.push(qSphereInWorld(qJ,.4,materials.mat13))
  qAxes.push(qSphereInWorld(qK,.4,materials.mat26))

}

function addQAxes(){
  qAxes.map(x=>{
    scene.add(x)})
  }

function removeQAxes(){
  qAxes.map(x=>{
    scene.remove(qAxes)})
}

/*function addQAxes(){
  qSphereInWorld(qOne,.2,materials.mat7)
  qSphereInWorld(qI,.13,materials.mat0)
  qSphereInWorld(qJ,.13,materials.mat13)
  qSphereInWorld(qK,.13,materials.mat26)
}*/


///////////////////////////




///////////////////////////////////
//////////////////////////////////
//
// Framing S3
//
// Given p and q in S3, given coords uvw, 
// locate the point w between 
// the point o on pq measured v from the closest point to 1
// the point r on pq which is perp to o, measured in the p to q direction
// the point s on o1 perp to pq, measured in the o to 1 direction
// the point t perp to o, r, s
// The coords are then 

function orstuvwCoords(o,r,s,t,u,v,w){
  return ((o.scalarmult(cos(v)).add(r.scalarmult(sin(v)))).scalarmult(cos(w))).add(
    (s.scalarmult(cos(u)).add(t.scalarmult(sin(u)))).scalarmult(sin(w)))
}

//v is along the p,q,o,r great circle, with o at v=0 and p at v = -d ; r is from o,r to s,t 
// and u is along s,t starting at s.
function orstCoords(frame){
  var o = frame.o, r= frame.r, s=frame.s, t=frame.t,d=frame.d
  //var uu = 1, vv=.2, ww = .3
//  console.log(frame, o,r,s,t,d,o.scalarmult(cos(vv-d)))
 // console.log(((o.mult(cos(vv-d)).add(r.mult(sin(vv-d)))).mult(
 //   cos(ww))).add(
 //    (s.mult(cos(uu)).add(t.mult(cos(uu)))).mult(sin(ww))))

  var g=function(u,v,w){
    return ((o.scalarmult(cos(v)).add(r.scalarmult(sin(v)))).scalarmult(cos(w))).add(
     (s.scalarmult(cos(u)).add(t.scalarmult(sin(u)))).scalarmult(sin(w)))
  }
  return g
}

//the points o,r are on the geodesic pq: o is closest to 1 (*this is not stable, and hence wrong*), 
// r is pi/2 around 
//the point s is pi/2 from o on the geodesic o1 and t is pi/2 from each of o,r,s
//oriented consistently (but not known which way that is!)
//d is the offset, the cosine of angle from p to o;  orientation is always in this order


// the right way to do this is to pull the point back to the origin; handedness shouldn't ?? matter
// From the origin, the pts are qOne, qI, qJ and qK.
// Hence: let m = the motion on the left (say?) that brings  (p.inverse()) q onto the 1 - I axis. 
// Then the points are then  m.inverse applied to the left of qOne, qI, qJ and qK. 

// So only need to work this out. 
function framingFrom(p,q){ // assume p and q are unit length quats
  qq = (p.inverse()).mult(q)
  var a = qq.i, b = qq.j, c = qq.k 
  var nn = sqrt(a*a+b*b+c*c);
  a = a/nn; b = b/nn; c = c/nn;
  var aa = sqrt(1+a)
  var sq2 = sqrt(2)
  var vv= new quat(aa/sq2,0,c/aa/sq2,-b/aa/sq2)
  var vc = new quat(aa/sq2,0,-c/aa/sq2,b/aa/sq2)
  var o,r,s,t,l // measuring from o; offset to p is included in return
  o = p.copy()
  r = p.mult(vc).mult(qI).mult(vv)
  s = p.mult(vc).mult(qJ).mult(vv)
  t = p.mult(vc).mult(qK).mult(vv)
  l = Math.acos(p.dot(q))
 return {"o":o,"r":r,"s":s,"t":t,"d":0,"l":l}//d is some legacy something
}








//////
//  Arrows


function arrowTubeFunction(pp,qq,radP,radQ,jmin,jmax){
  var frame = framingFrom(pp,qq)
  var orsts = orstCoords(frame)
  return function(i,j){
    var jj = (j*jmax+(1-j)*jmin)
    return orsts(i*2*PI,
    jj*frame.l-frame.d,
    j*radQ+(1-j)*radP)}
}

function arrowTube(pp,qq,mat = materials.mat17, radP =.02, radQ = .003,  iN=10,jN=30,
    jmin=.00,jmax=1.00){
  //TO DO: adjust jN for distance from 1
    return meshFromS3surface(
      arrowTubeFunction(pp,qq,radP,radQ,jmin,jmax),
      0,1,iN,jmin,jmax,jN,mat)
}

/////
// Cayley Graph

var ourCayleyGraph=[]

function removeCayleyGraph(){
  ourCayleyGraph.map(x=>{scene.remove(x);x.geometry.dispose();})
  // MEMORY ISSUE 
  ourCayleyGraph=[]

}

function makeCayleyGraph(basept=basepoint,
  groupelts=ourGroupElts,
  groupgens=getGroupGensFromNames(ourGroup.name),usecolorsQ=true){

  removeCayleyGraph();
  
  i=0
  groupgens.map(y=>
    {
    if(!y.l.equalTo(qOne))
    {var matt = "mat"+((7*(i++))%30).toString()
    ourGroupElts.map(x=>{
      var bb = x.acton(basept), cc = x.acton(y.acton(basept));
      var tube = arrowTube(bb,cc,materials[matt])
      ourCayleyGraph.push(tube)
   //   geometricObjects.push(tube)
          })}})
}


function sphereFunction(rad = .2){
  return function(u,v){return new quat(rad,
        Math.cos(6.28312*u)*Math.sin(3.14159*v),
        Math.sin(6.28312*u)*Math.sin(3.14159*v),
        Math.cos(3.14159*v)).normalize()}
}

function basicsphere(rad =.2,material = mats['grayMat']){
  return meshFromS3surface(sphereFunction(rad),0,1,defaultSphereCircumN, 
      0,1,defaultSphereLatN,
      material,1)
}

function movebasicsphere(asphere, position, radius){
  positions = asphere.geometry.attributes.positions.array
  
}

function tubeFunctionFrom(p,q,rad=.2, fullTorus=false){
  var frame = framingFrom(p,q)
 // console.log("ho", frame)
  var orsts = orstCoords(frame)
  if(!fullTorus)
    return function(i,j){return orsts(i*2*PI,j*(frame.l)+frame.d,rad)}
  else
    return function(i,j){return orsts(i*2*PI,j*2*PI,rad)}
}

function tubeArc(pp,qq,rad =.08, fullQ= false ,material=materials.mat0, imin=0,imax=1, iN=10, jmin=0,jmax=1,jN=50,
  clipQ=false,clippingbound = 1,vertexmaterialfunction = 1){
    var ff= tubeFunctionFrom(pp,qq,rad,fullQ)
    return meshFromS3surface(tubeFunctionFrom(pp,qq,rad,fullQ),imin,imax,iN,jmin,jmax,jN,material,
      vertexmaterialfunction)
}


function rejiggertubeArc(amesh, pp,qq,rad =.08, fullQ= false ,material=0, imin=0,imax=1, iN=10, jmin=0,jmax=1,jN=50,
  clipQ=false,clippingbound = 1,vertexmaterialfunction = 1){
    var ff= tubeFunctionFrom(pp,qq,rad,fullQ)
    var thematerial=amesh.material;
    if(material !=0){thematerial=material;}
    var amesh =  revisemeshFromS3surface(amesh, tubeFunctionFrom(pp,qq,rad,fullQ),imin,imax,iN,jmin,jmax,jN,thematerial,
      vertexmaterialfunction)
    amesh.visible = true;
    return amesh
}




function initQuatDisplay(){
  setUpQAxes()

}


