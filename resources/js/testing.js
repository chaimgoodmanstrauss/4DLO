
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// testing



function runtest(agroup,basepoints){

  let out="";

  out+="gp="+agroup.toString(true)+";\n\n";

  basepoints.forEach(function(basept){
    let name = basept[0];
    let quat = basept[1];
    out = out+name+"={";
    let orbit = agroup.actOn(quat)
    orbit.map(x=>{out=out+x.toString(true)+",\n"});
    out= out.substring(0,out.length-2);
    out = out+"};\n\n" ;
    return out
  })

  out+="Graphics3D[i=1;{{Red,Green,Blue,Green}[[i++]],(If[True,Sphere[Drop[#,-1]/(#[[-1]]+1),.04]]&@(List@@#&@#))&/@#}&/@"
  +"{";
  basepoints.forEach(function(pt){return out = out + pt[0]+","});
  out = out.substring(0,out.length-1);
  out = out +"}]\n\n";
 
  console.log(out)

 
}
const z = new quat(1,2,3,4);
const a = new qAction(qOne,qW);
const b =  new qAction(qW,qOne);
const  gens0 = [a,b];

   namedQ["gens0"]=gens0
   namedQ["group0"]=new group(gens0)



     
//runtest(namedQ.group0,[ ["pts1",new quat(.85,.13,.08,.1)],   ["pts2", new quat(.9,.08,.1,.14)]])



/*const  gensOne = [
 new qAction(qOne,qW),
 new qAction(qW,qOne),
 new qAction(qO,qOne),
 new qAction(qOne,qO)
 ]*/

//const groupOne = generateGroup(gensOne);

//namedQ["groupOne"]=groupOne
//namedQ["gensOne"]=gensOne




function testgp(ourgpname="pm12thTxT")
{
  const ourgennames = so4subgroups[ourgpname].gens;
  string = "(* our generators, ";
  ourgennames.map(x=>(string=string+x+" "));
  string = string+":\n"
  let ourgens = getActionsFromNames(ourgennames);
  ourgens.map(x=>(string=string+x.toString()+"\n"))
  string += "*)\n\n";
  console.log( string )


  let gp = new tempSortedGroup(ourgens)

  string = string + " (*"+gp.groupElements.length.toString()+"*) sortedGroupElements = {";
  gp.groupElements.map(x=>(string=string+x.toString(true)+",\n"))
  string=string.slice(0,string.length-2)+"};"
  console.log(
    string+"\n\n")
  
    let gp2 = new group(ourgens)

    string =  string + " (*"+gp2.groupElements.length.toString()+"*) groupElements = {";
    gp2.groupElements.map(x=>(string=string+x.toString(true)+",\n"))
    string=string.slice(0,string.length-2)+"};"
    console.log(
      string+"\n\n")
}
  
  
function testgp2(ourgpname="pm12thTxT")
{
  
  const ourgennames = so4subgroups[ourgpname].gens;
  string = /*"(*  "+*/ourgpname+" : ";
  ourgennames.map(x=>(string=string+x+" "));
  //string = string+":\n"
  let ourgens = getActionsFromNames(ourgennames);
  //ourgens.map(x=>(string=string+x.toString()+" "))
 
  let gp = new group(ourgens)

  string = string + " : " + gp.groupElements.length.toString();
  return string;
  //  string = "";
  
  /*  let gp2 = new group(ourgens)

    string =  string + " (*"+gp2.groupElements.length.toString()+"*) ";
    
    console.log(
      string+"\n\n")*/
  //if(ourgpname == "pm2veod4mxc2n"){   console.log("hold it")  }
}




//Object.values(standardgenerators).forEach(function(n){return n.forEach(m){return namedQ[m]}})


function testAllGroups(groups=so4subgroups){
  string = "";
  Object.keys(groups).forEach(function(x){string = string + testgp2(x)+"\n"})
  console.log(string)
  
}

