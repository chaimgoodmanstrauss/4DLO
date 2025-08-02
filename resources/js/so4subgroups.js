/*#######################################
#######################################
##
##
##  Hard Coded Geometric Details
##
##
##
##*/

//******** some conversions **********
// 

function quatFromName(name,source=namedQ){
  if(source[name] == undefined)
    return undefined
  else
    return source[name].copy()
}

function actionFromName(name, source = standardgeneratorNames ){
  let actions=source[name].map(x=>{if(x==undefined){return undefined}else return quatFromName(x)});
  let star = (name.charAt(0)=="S") // for star
  return new qAction(...actions,star,name)
}

function getActionsFromNames(names,source = standardgeneratorNames){
    return names.map(x=>actionFromName(x,source))
  }
  


function getGroupGensFromNames(groupname, gpsource = so4subgroups, 
  gensource = standardgeneratorNames){
    let gpgens = gpsource[groupname].gens
    return getActionsFromNames(gpgens,gensource);
  }

groupsElementsList = {};

function makeGroupFromName(groupname,gpElList= groupsElementsList,gpsource = so4subgroups, gensource= standardgeneratorNames){
  if(gpElList[groupname]==undefined){
    if(gpsource[groupname]==undefined){
      gpElList[groupname]=undefined;
    }
    else{
      let gens = getGroupGensFromNames(groupname,gpsource, gensource);
      gpElList[groupname]= makegroup(gens,groupname)
      
    }
  }
  



  return gpElList[groupname]
}

/////////////////////////////
//  adapted from tables in SoT, reproduced from On Quaternions and Octonions; 
// as these are in same order as those tables, the key is easily found.

// some globally defined important quaternions, following Conway's notation
// note that all of these are normalized and lie in S3.


// named quaternions, keyed in namedQ
/*
 qOne  => 1
 qW => \omega
 qI,qJ,qK => I,J,K
 qO, qIco => i_O, i_I
 */
 



const qOne = new quat(1,0,0,0)
const qW = new quat(-.5,.5,.5,.5) // note normalized
const qWp= new quat(.5,.5,.5,.5)
const qWi= new quat(.5,-.5,.5,.5)
const qWj= new quat(.5,.5,-.5,.5)
const qWk= new quat(.5,.5,.5,-.5)

const qI = new quat(0,1,0,0)

const qJ = new quat(0,0,1,0)
const qK = new quat(0,0,0,1)
const qIco = new quat(0,.5,(sqrt(5)-1)/4, (sqrt(5)+1)/4)
const qO = new quat(0,0,sqrt(.5),sqrt(.5))
const q0 = new quat(0,0,0,0)
const qOp = new quat(sqrt(.5),sqrt(.5),0,0)

// named qActions

const qIdentity = new qAction(qOne, qOne);
const qOneOne = qIdentity
const qIOne = new qAction(qI,qOne);
const qWOne = new qAction(qW,qOne);
const qOqO = new qAction(qO,qO);


function qen(n){ 
  let key = "qen"+n.toString();
  if(!(key in namedQ)){
    namedQ[key]=new quat(cos(TPI/n),sin(TPI/n),0,0)
    };
  return namedQ[key]
}




let namedQ={}, renamedQ = {}

function resetNamedQ(en=3,em=2){// for 
  namedQ = {
    "q4th":qen(4),
    "q2nd":qen(2),
    "qe2n":qen(2*en),  
    "qe2m":qen(2*em),
    "qe3n":qen(3*en),
    "qen":qen(en), 
    "qem":qen(em),
    "qI":qI, 
    "qIco":qIco, 
    "qJ":qJ, 
    "qO":qO, 
    "qOne":qOne, 
    "qW":qW,
    "qoW":qW.conjugate(),
    "mqe2n":qen(2*en).scalarmult(-1),  
    "mqe2m":qen(2*em).scalarmult(-1),
    "mqe3n":qen(3*en).scalarmult(-1),
    "mqen":qen(en).scalarmult(-1), 
    "mqem":qen(em).scalarmult(-1),
    "mqI":qI.scalarmult(-1), 
    "mqIco":qIco.scalarmult(-1), 
    "mqJ":qJ.scalarmult(-1), 
    "mqO":qO.scalarmult(-1), 
    "mqOne":qOne.scalarmult(-1), 
    "mqW":qW.scalarmult(-1),
    
    
    // for stabilizers
    "qJmK":new quat(0,0,1,-1),
    "qmJK":new quat(0,0,-1,1)
  }


  renamedQ =
  { "qOne":"1111",
    "qW":"1100",
    "q4th":"3100",
    "q2nd":"1100",
    
  }


}

resetNamedQ();


// standard generators names; if they are starred, the first letter is S
// the names are keyed in standardgeneratorNames
// as qActions, they are listed in 

/*
The code is 
i,j,k-> qI, qJ, qK -> I, J, K
o -> qOne -> 1
O, Ico -> qO, qIco -> i_O, i_I
W -> \omega
M -> qminus -> \minus
e2n, 3en, etc. 

*/
/*{cmfcnfs, e2me2n, e2mj, em1, emfesnf, emo, IcoIco, IcoIcoprime, Icoo, \
II, Io, IoI, je2n, JJ, Jo, mem1, memo, Memo, moen, Moen, mOO, Oe2n, \
oen, oI, oIco, oJ, OJ, oO, Oo, OO, oW, pcmfcnfs, pe2mj, pemfesnf, \
pIcoIco, pIcoIcoprime, pIoI, pje2n, pJJ, pOO, pWoW, pWW, We3n, Wo, \
WoW, WW}*/

const standardgeneratorNames= 
{"oo":["qOne","qOne"],
  "pm":["qOne","mqOne"],
 "e2me2n":["qe2m","qe2m"],
 "emo":["qem","qOne"],
 "Icoo":["qIco","qOne"], 
 "Io":["qI","qOne"],
 "je2n":["qJ","qen"],
 "Jo":["qJ","qOne"],
 "memo":["qminusEm","qOne"],
 "moen":["qminusOne","qen"],
 "Oe2n":["qO","qe2n"],
 "oen":["qOne","qen"],
 "oI":["qOne","qI"],
 "II":["qI","qI"],
 "IoI":["qI","mqI"],
 "oJ":["qOne","qJ"],
 "OJ":["qO","qJ"],
 "oO":["qOne","qO"],
 "Oo":["qO","qOne"],
 "OO":["qO","qO"],
 "oW":["qOne","qW"],
 "We3n":["qW","qe3n"],
 "Wo":["qW","qOne"],
 "WW":["qW","qW"],
 "WoW":["qW","qoW"],
 "mWoW":["mqW","qoW"],
 "oIco":["qOne","qIco"],
 "IcoIco":["qIco","qIco"],
 "IcoIcoprime":["qIco","qIco"],// WRONG, but what is right? Need to pass star along

 }
 

/*
// the code is: 

p,pm -> \plus,\plusminus
2ve, 3rd, etc -> \frac{1}{2}, \frac{1}{3}, etc
I,O,T-> I,O,T
C,D-> C_{ , D_{
x -> \times
n, m, 2n, 3m, etc -> n, m, 2n, 3m 

add braces as implied

*/
// print is a close approximation of the latex form; 
// special are representative points with non-trivial stabilizers, also given

// Also, work out and hash good base points
//PLAYING W GENERATORS, etc
//NICE
//CANONICAL
so4subgroups ={
  //the fixed point free ones NOT listed in SoT!
  "Ixone":{"print":"I x 1","name":"Ixone", "gens":["Icoo", "Wo"],"class":"fixedpointfree"},
  "Oxone":{"print":"O x 1","name":"Oxone", "gens":["Io", "Wo"],"class":"fixedpointfree"},
  "tester":{"print":"tester","name":"tester","gens":["IoI","WoW","oO"],"class":"chiral"},
  "tester2":{"print":"tester2","name":"tester2","gens":["Icoo","oO","WW","WoW"],"class":"chiral"},
  "pmIxO":{"print":"± [I x O]", "name":"pmIxO", "gens":["Icoo", "Wo", "oO", "oW"],"class":"chiral"}, 
  "pmIxT":{"print":"± [I x T]","name":"pmIxT", "gens":["pm","Icoo", "Wo", "oI", "oW" ],"class":"chiral"}, 
  "pmIxd2n":{"print":"± [I x D2n]","name":"pmIxd2n", "gens":["pm","Icoo", "Wo", "oen", "oJ" ],"class":"chiral"}, 
  "pmIxcn":{"print":"± [I x Cn]","name":"pmIxcn", "gens":["pm","Icoo", "Wo", "oen" ],"class":"chiral"}, 
  "pmOxT":{"print":"± [O x T]","name":"pmOxT", "gens":["pm","Oo", "Wo", "oI", "oW" ],"class":"chiral"}, 
  "pmOxd2n":{"print":"± [O x D2n]","name":"pmOxd2n", "gens":["pm","Oo", "Wo", "oen", "oJ" ],"class":"chiral"}, 
  "pm2veOxd2n":{"print":"±/2 [O x D2n]","name":"pm2veOxd2n", "gens":["pm","Io", "Wo", "oen" , "OJ"],"class":"chiral"}, 
  "pm2veOxod4n":{"print":"±/2 [O x D4n.]","name":"pm2veOxod4n", "gens":["pm","Io", "Wo", "oen", "oJ" , "Oe2n"],"class":"chiral"}, 
  "pm6thOxd6n":{"print":"±/6 [O x D6n]","name":"pm6thOxd6n", "gens":["pm","Io", "Jo", "oen" , "OJ", "We3n"],"class":"chiral"}, 
  "pmOxcn":{"print":"± [O x Cn]","name":"pmOxcn", "gens":["pm","Oo", "Wo", "oen" ],"class":"chiral"}, 
  "pm2veOxc2n":{"print":"±/2 [O x C2n]","name":"pm2veOxc2n", "gens":["pm","Io", "Wo", "oen" , "Oe2n"],"class":"chiral"}, 
  "pmTxd2n":{"print":"± [T x D2n]","name":"pmTxd2n", "gens":["pm","Io", "Wo", "oen", "oJ" ],"class":"chiral"}, 
  "pmTxcn":{"print":"± [T x Cn]","name":"pmTxcn", "gens":["pm","Io", "Wo", "oen" ],"class":"chiral"}, 
  "pm3rdTxc3n":{"print":"±/3 [T x C3n]","name":"pm3rdTxc3n", "gens":["pm","Io", "oen" , "We3n"],"class":"chiral"}, 
  "pm2ved2mxod4n":{"print":"±/2 [D2m x D4n]","name":"pm2ved2mxod4n", "gens":["pm","emo", "oen", "oJ" , "je2n"],"class":"chiral"}, 
  "pmd2mxcn":{"print":"± [D2m x Cn]","name":"pmd2mxcn", "gens":["pm","emo", "Jo", "oen" ],"class":"chiral"}, 
  "pm2ved2mxc2n":{"print":"±/2 [D2m x C2n]","name":"pm2ved2mxc2n", "gens":["pm","emo", "oen" , "je2n"],"class":"chiral"}, 
  "p2ved2mxc2n":{"print":"+/2 [D2m x C2n]","name":"p2ved2mxc2n", "gens":["Memo","Moen", "je2n"],"class":"chiral"}, //******CHECK
  "pm2veod4mxc2n":{"print":"±/2 [D4m. x C2n]","name":"pm2veod4mxc2n", "gens":["pm","emo", "Jo", "oen" , "e2me2n" ],"class":"chiral"},
  // chiral2
  "pmIxI":{"print":"± [I x I]","name":"pmIxI", "gens":["pm","Icoo", "Wo", "oIco", "oW"],"class":"chiral2"},
  "pm60thIxI":{"print":"±/60 [I x I]","name":"pm60thIxI", "gens":["pm","WW","IcoIco"],"class":"chiral2"},
  "p60thIxI":{"print":"+/60 [I x I]","name":"p60thIxI", "gens":["WW","IcoIco"],"class":"chiral2"}, 
  "pm60thIxoI":{"print":"±/60 [I x I.]","name":"pm60thIxoI", "gens":["pm","WW","IcoIcoprime"],"class":"chiral2"},
  "p60thIxoI":{"print":"+/60 [I x I.]","name":"p60thIxoI" , "gens":["WW","IcoIcoprime"],"class":"chiral2"},
  "pmOxO":{"print":"± [O x O]","name":"pmOxO", "gens":["pm","Oo", "Wo", "oO", "oW"],"class":"chiral2",
      "special":[["qOne","O",48]],"notes":"There are 48 images of qOne, each with full 24 O symmetry"},
  "pm2veOxO":{"print":"±/2 [O x O]","name":"pm2veOxO", "gens":["pm","Io", "Wo", "oI","oW","OO"],"class":"chiral2",
      "special":[["qW","O",24],["qOne","O",24]],"notes":"cleaves the orbit of OxO into the verts of the 24 cell and its dual"},
  "pm6thOxO":{"print":"±/6 [O x O]","name":"pm6thOxO", "gens":["pm","Io", "Jo", "oI", "oJ","WW","OO"],"class":"chiral2",
      "notes":"O symmetry at each (±1000); T symmetry at each ±1±1+1±1",
      "special":[["qW","T",16],["qOne","O",8]]},
  "pm24thOxO":{"print":"±/24 [O x O]","name":"pm24thOxO", "gens":["pm","WW","OO" ],"class":"chiral2",
      "special":[[]],"notes":""},
  
  "p24thOxO":{"print":"+/24 [O x O]","name":"p24thOxO", "gens":["WW","OO" ],"class":"chiral2"},
  "p24thOxoO":{"print":"+/24 [O x O.]","name":"p24thOxoO", "gens":["WW","mOO" ],"class":"chiral2"},
  "pmTxT":{"print":"± [T x T]","name":"pmTxT", "gens":["pm","Io", "Wo", "oI", "oW"],"class":"chiral2"},
  "pm3rdTxT":{"print":"±/3 [T x T]","name":"pm3rdTxT", "gens":["pm","Io", "Jo", "oI", "oJ", "WW"],"class":"chiral2"}, // NOTE: congruent to the next one
  "pm3rdTxoT":{"print":"±/3 [T x T.]","name":"pm3rdTxoT", "gens":["pm","Io", "Jo","oI", "oJ" , "WoW" ]},
  "pm12thTxT":{"print":"±/12 [T x T]","name":"pm12thTxT", "gens":["pm","WW", "II" ],"class":"chiral2"},// NOTE: Congruent to next one 
  "pm12thTxoT":{"print":"±/12 [T x T.]","name":"pm12thTxoT", "gens":["pm","WoW", "IoI"]},
  //"p12thTxT":{"print":"+/12 [T x T]","name":"p12thTxT", "gens":["WoW", "IoI"],"class":"chiral2",
  "p12thTxT":{"print":"+/12 [T x T]","name":"p12thTxT", "gens":["WW", "II"],"class":"chiral2",
      "notes":"fixes j-k and k-j, with T symmetry on level spheres around these; exact same? as p12thTxoT",
       "special":["qJmK","qmJK"] }, // NOTE: Congruent to next one 
  //"p12thTxoT":{"print":"+/12 [T x T.]","name":"p12thTxoT", "gens":["mWoW", "IoI"],"class":"chiral2"}, // CHECK // ?? So why include?
  "p12thTxoT":{"print":"+/12 [T x T.]","name":"p12thTxoT", "gens":["WoW", "IoI"],"class":"chiral2"}, // CHECK // ?? So why include?
  "pmd2mxd2n":{"print":"± [D2m x D2n]","name":"pmd2mxd2n", "gens":["pm","em1", "Jo", "oen", "oJ" ],"class":"chiral2"},
  "pm2veod4mxod4n":{"print":"±/2 [D4m. x D4n.]","name":"pm2veod4mxod4n", "gens":["pm","em1", "Jo", "oen", "oJ" , "e2me2n" ],"class":"chiral2"},
  "pm4thd4mxod4n":{"print":"±/4 D[4m x D4n.]","name":"pm4thd4mxod4n", "gens":["pm","em1","oen" , "e2mj", "je2n" ],"class":"chiral2"},
  "p4thd4mxod4n":{"print":"+/4 [D4m x D4n.]","name":"p4thd4mxod4n", "gens":["mem1","moen" , "e2mj", "je2n" ],"class":"chiral2"},  //Require m, n odd 
  "pm2fthd2mfd2nfs":{"print":"±/2f [D2mf x D2nf(s)]","name":"pm2fthd2mfd2nfs", "gens":["pm","emo", "oen","emfesnf","JJ"],"class":"chiral2"}, //Require (s, f) = 1 
  "p2fthd2mfd2nfs":{"print":"+/2f [D2mf x D2nf(s)]","name": "p2fthd2mfd2nfs", "gens":["memo", "moen","emfesnf","JJ"],"class":"chiral2"}, // m, n odd (s, 2f) = 1 
  "pmfthcmfxcnfs":{"print":"±/f [Cmf x Cnf(s)]","name":"pmfthcmfxcnfs", "gens":["pm","emo","oen","cmfcnfs"],"class":"chiral2"}, // (s, f) = 1 \\
  "pfthcmfxcnfs":{"print":"+/f [Cmf x Cnf(s)]","name": "pfthcmfxcnfs", "gens":["memo","moen","cmfcnfs"],"class":"chiral2"}, //m, n \mbox{ odd, } (s, 2f) = 1 
}

so4subgroupClasses = {
  "chiral":
    {"name":"Chiral groups, I. These are most of the metachiral groups",
      "groups":["pmIxO","pmIxT","pmIxd2n",
      "pmIxcn","pmOxT","pmOxd2n","pm2veOxd2n","pm2veOxod4n",
      "pm6thOxd6n","pmOxcn","pm2veOxc2n","pmTxd2n","pmTxcn","pm3rdTxc3n",
      "pm2ved2mxod4n","pmd2mxcn","pm2ved2mxc2n","p2ved2mxc2n","pm2veod4mxc2n"]},
    "chiral2":
      {"name":"Chiral groups, II.  These groups are mostly orthochiral, with a few  parachiral groups in the last few lines.   The generators should be taken with both signs except in the haploid cases,  for which we just indicate the proper choice of sign.",
          "groups":[
              "pmIxI", "pm60thIxI", "p60thIxI", "pm60thIxoI", 
              "p60thIxoI" , "pmOxO", "pm2veOxO", "pm6thOxO", "pm24thOxO", "p24thOxO", "p24thOxoO",
               "pmTxT", "pm3rdTxT", "pm3rdTxoT", "pm12thTxT", "pm12thTxoT", "p12thTxT", 
              "p12thTxoT", "pmd2mxd2n", "pm2veod4mxod4n", "pm4thd4mxod4n", "p4thd4mxod4n" ,
               "pm2fthd2mfd2nfs", "p2fthd2mfd2nfs", "pmfthcmfxcnfs", "pfthcmfxcnfs", 
           ]},
           
      // achiral groups still to go
}
  

    