/***
 Sets up and manages the UI for the full list of groups
 
**/


var printNames = Object.keys(so4subgroups).map(x=>so4subgroups[x].print)
var printNamesToGroup = {};
Object.keys(so4subgroups).map(x=>printNamesToGroup[so4subgroups[x].print]=x);

 var guiParams = {
   "group":"+/24 [O x O]",
   "en":3,
   "em":2,
   "reset":false,
   "basepoint r":1,
   "basepoint i":.03,
   "basepoint j":.02,
   "basepoint k":.01,
   "showRandomSphereOrbitsQ":true,
   "showGeneratorsAndBasePointsQ":true,
   "showQAxesQ":true,
   "basepointName":"1100",
 }

 var gp={}; //a place to put data
 
function groupChanged(){
  ourGroupName = guiParams.group
  // TO DO: Check to see if .isDefined() 
  ourGroup = makeGroupFromName(printNamesToGroup[guiParams.group]);
  ourGroupElts = ourGroup.groupElements;
  clearOrbits();
  createOrbits(printNamesToGroup[guiParams.group]);
  removeCayleyGraph();
  if(guiParams.showGeneratorsAndBasePointsQ) {makeCayleyGraph()};
 }

 function randomSpheresOnOff(){
  
 }

 function generatorsOnOff(){
  removeCayleyGraph()
  if(guiParams.showGeneratorsAndBasePointsQ)
  {
    makeCayleyGraph()
  }
 }


 function qAxesOnOff(){
 	if(guiParams.showQAxesQ){
 		addQAxes()}
 	else{removeQAxes()}
 }

var basepoint = new quat(1,.1,.2,.3).normalize()
var basesphere =0

function baseptMoved(){
	basepoint.r=guiParams["basepoint r"]
	basepoint.i=guiParams["basepoint i"]
	basepoint.j=guiParams["basepoint j"]
	basepoint.k=guiParams["basepoint k"]
 basepoint.normalize()
	if(basesphere!=0){
		scene.remove(basesphere)
	}
	basesphere = qSphereInWorld(basepoint,.1,materials.mat0)
	scene.add(basesphere)
	geometricObjects.push(basesphere)
  clearOrbits();
  createOrbits(printNamesToGroup[guiParams.group]);
  removeCayleyGraph();
  if(guiParams["showGeneratorsAndBasePointsQ"]) makeCayleyGraph();
}
 
 function baseptSelectionMoved(){
  var qq = namedQ[Object.keys(renamedQ).find(key => renamedQ[key] === guiParams.basepointName)]; 
  gp.br.setValue(qq.r);
  gp.bi.setValue(qq.i);
  gp.bj.setValue(qq.j);
  gp.bk.setValue(qq.k)
  ;

  
 }
// set up and initialization
function initUI(){
  
  gui.add(guiParams,'group',printNames).onChange(groupChanged);
  gui.add(guiParams,'basepointName',Object.keys(renamedQ).map(x=>renamedQ[x])).onChange(baseptSelectionMoved)
  gp.br = gui.add(guiParams,'basepoint r',0,1).onChange(baseptMoved)
  gp.bi = gui.add(guiParams,'basepoint i',0,1).onChange(baseptMoved)
  gp.bj = gui.add(guiParams,'basepoint j',0,1).onChange(baseptMoved)
  gp.bk = gui.add(guiParams,'basepoint k',0,1).onChange(baseptMoved)
  gui.add(guiParams,"showRandomSphereOrbitsQ").onChange(randomSpheresOnOff)
  gui.add(guiParams,"showGeneratorsAndBasePointsQ").onChange(generatorsOnOff)
  gui.add(guiParams,"showQAxesQ").onChange(qAxesOnOff)

//***** ADD THIS TO discreteSO4.js
  //setUpQAxes()
  //addQAxes()
  //baseptMoved()
  //qAxesOnOff()
  //groupChanged()
}