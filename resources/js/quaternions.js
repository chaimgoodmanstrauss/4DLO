/** 

Quaternions 

For the actions and inverses we are assuming that these are unit quaternions only.
The constructor doesn't do this for you!

**/

// a maximum order for any quaternion; that is, we don't generate cyclic group actions
// after this many steps. 
const QUATMAXORDER = 30


// Our quaternions:


class quat {
  
  // note that the constructed quat is not by default normalized. 
  // follow by .normalize() to normalize.
  constructor(a,b,c,d){
    this.r=a;
    this.i=b;
    this.j=c;
    this.k=d;
    this.order=0; //must explicitly calculate before using
  }
  

  // returns the order of a quaternion, if less than QUATMAXORDER
  orderOf(){
    if(this.order==0){
      var temp = this.copy();
      temp.multby(this);
      var count=1;
      while(!temp.equalTo(this) && count<QUATMAXORDER){
        temp.multby(this);
        count++;
      }
      this.order=count;
    }
    return this.order;
  }
  
  equalTo(q, prec=SHORTEPSILON){
    return (abs(this.r-q.r)<prec && abs(this.i-q.i)<prec && 
      abs(this.j-q.j)<prec && abs(this.k-q.k)<prec)
  }
  
  minusEqualTo(q,prec=SHORTEPSILON){
    return (abs(this.r+q.r)<prec && abs(this.i+q.i)<prec && 
      abs(this.j+q.j)<prec && abs(this.k+q.k)<prec)
  }
  
  // lexigraphic ordering, on quats
  // they are equal if within SHORTEPSILON; so less than if more than SHORTEPSILON less than
  lessThan(q, prec=SHORTEPSILON){
    return( 
      (q.r-this.r >= SHORTEPSILON) || 
      (
        (abs(q.r-this.r)<SHORTEPSILON) &&
        ( 
          (q.i-this.i >=SHORTEPSILON)||
          ((abs(q.i-this.i)<SHORTEPSILON) &&
            ( (q.j-this.j>=SHORTEPSILON) ||
              ((abs(q.j-this.j)<SHORTEPSILON)&&(q.k-this.k>=SHORTEPSILON))
          )
        )
      )
      )
      )
  }
  
  // a string
  toString(forMathematica=false, prec = 4){
    if(forMathematica)
    { return "Quaternion["+this.r.toFixed(prec)+","+this.i.toFixed(prec)+","+
        this.j.toFixed(prec)+","+this.k.toFixed(prec)+"]"}
    else{
      return this.r.toFixed(prec)+" +"+this.i.toFixed(prec)+"i +"+
        this.j.toFixed(prec)+"j +"+this.k.toFixed(prec)+"k";
    }
  }
    
  // the modulus, or absolute value of this; returns a real number
  abs(){
    return Math.sqrt(this.r*this.r+this.i*this.i+this.j*this.j+this.k*this.k);
  }

  // vector inner product; returns a real number
  dot(q){
    return this.r*q.r+this.i*q.i+this.j*q.j+this.k*q.k;
  }
  
  imabs(){
    return Math.sqrt(this.r*this.r+this.i*this.i+this.j*this.j+this.k*this.k);
  }

  im(){
    return new quat(0,this.i,this.j,this.k)
  }


  // returns a new copy
  copy(){
    return new quat(this.r,this.i,this.j,this.k)
  }
  
  // returns a new quaternion
  scalarmult(s){
    return new quat(s*this.r,s*this.i,s*this.j,s*this.k)
  }
  
  // scales this
  scalarmultby(s){
    this.r*=s;
    this.i*=s;
    this.j*=s;
    this.k*=s;
    return this;
    }
  
  //returns a new quaternion, the inverse
  inverse(){
    return this.copy().invert();
  }
  
  // We are assuming all our quaternions are unit
  //inverts this
  invert(){
    this.conj();
    return this;
  }
  
  //returns the normalized quat; as we are in S3, all of our quaternions should be normalized;
  normalize(){
    if (abs(this.abs())<.0000001){return q0}
    this.scalarmultby(1/this.abs());
    return this;
  }
  
  // put in a standard form for actions
  positivize(){
    let out;
    if((this.r<0)||(this.r==0 && 
      ((this.i<0)||((this.i==0)&&((this.j<0)||((this.j==0)&&this.k<0)))))){
      out = this.scalarmult(-1);
    }
    else out=this.copy();
    return out;
  }
  
  
  // returns this plus q
  addby(q){
    this.r+=q.r;
    this.i+=q.i;
    this.j+=q.j;
    this.k+=q.k;
    return this;
  }
  
  // returns a new quaternion, this+q
  add(q){
    return this.copy().addby(q)
  }
  
  // conjugates this
  conj(){
    this.i=-this.i;
    this.j=-this.j;
    this.k=-this.k;
    return this;
  }
  
  // returns a new conjugate
  conjugate(){
    return this.copy().conj();
  }
  
  // returns this * q on the right (note: not commutative)
  multby(q){
    var r=this.r, i = this.i, j=this.j,k=this.k;
    this.r=-q.i* i - q.j* j - q.k* k + q.r* r;
    this.i= q.r* i + q.k* j - q.j* k + q.i* r;
    this.j=-q.k* i + q.r* j + q.i* k + q.j* r;
    this.k= q.j* i - q.i* j + q.r* k + q.k* r;
    return this;
  }

  
  // returns a new product, this * q 
  mult(q){
    return new quat(
      -q.i*this.i - q.j*this.j - q.k*this.k + q.r*this.r, 
      q.r*this.i + q.k*this.j - q.j*this.k + q.i*this.r, 
      -q.k*this.i + q.r*this.j + q.i*this.k + q.j*this.r, 
      q.j*this.i - q.i*this.j + q.r*this.k + q.k*this.r)
  }
  
  // returns q* this (note: not commutative)
  lmultby(q){
    var r=this.r, i = this.i, j=this.j,k=this.k;
    this.r=-i* q.i - j* q.j - k* q.k + r* q.r;
    this.i= r* q.i + k* q.j - j* q.k + i* q.r;
    this.j=-k* q.i + r* q.j + i* q.k + j* q.r;
    this.k= j* q.i - i* q.j + r* q.k + k* q.r;
    return this;
  }
  
  
  
  // returns a new quat, acted upon by a quaternion action
  actby(a){
    if(!a.star){
      let t1 = a.l.inverse() //inverses on the left!
    return t1.multby(this).multby(a.r)}
    else
    {
      let out = a.l.inverse().multby(this.invert()).multby(a.r);
      this.invert();
      return this}
  }

}





// actions are of the form [l,r] or *[l,r], acting on a quaternion q by
// q-> l_ . q . r  or  l_. q_ . r

class qAction{
  constructor(l,r,star=false,name=""){
    this.l=l;
    this.r=r;
    this.star = star;
    this.name = name;
  }
  
  copy(){
    return new qAction(this.l,this.r,this.star, this.name)
  }
  
  standardize(){
    // make positive first non-zero of l.r, l.i, l.j,l.k ... 
    // presume l and r are normalized
    
    if(
      (this.l.r<-SHORTEPSILON ||
        ( abs(this.l.r)<SHORTEPSILON &&
          ( this.l.i<-SHORTEPSILON ||
            ( abs(this.l.i)<SHORTEPSILON &&
              ( this.l.j<-SHORTEPSILON ||
                ( abs(this.l.j)<SHORTEPSILON &&
                  this.l.k<-SHORTEPSILON
                  )
                )
              )
            )
          ))){
      this.l.scalarmultby(-1);
      this.r.scalarmultby(-1);

    }
    return this;
  }
  
  // returns a new quat, acted upon by this
  acton(q){
    return q.actby(this);
  }
  
  //  returns a new action, this followed by action a (a on outside)
  mult(a){
    if(!this.star && !a.star){
      return new qAction(this.l.mult(a.l), this.r.mult(a.r), false,this.name+"."+a.name);
    }
    else if(this.star && !a.star){
      return new qAction(this.l.mult(a.l), this.r.mult(a.r), true,this.name+"."+a.name);
      
    }
    else if(!this.star && a.star){
      return new qAction(this.r.mult(a.l), this.l.mult(a.r), true,this.name+"."+a.name);
      
    }
    else //(this.star && a.star)
    {
      return new qAction(this.r.mult(a.l), this.l.mult(a.r), false,this.name+"."+a.name);
    }
  }
  
  //projectively equivalent in  S04, double covered by  S3 x S3 
  equalTo(a){
    return (
      (this.star==a.star) && 
      (
        (this.l.equalTo(a.l) && this.r.equalTo(a.r))
        ||
        (this.l.minusEqualTo(a.l) && this.r.minusEqualTo(a.r))
      )
    ) 
  }
  
  lessThan(a){
    let tlp = this.standardize().copy();
    let alp = a.standardize().copy();
    let ltQ = tlp.l.lessThan(alp.l);
    if(!ltQ && tlp.l.equalTo(alp.l))
    { ltQ = tlp.r.lessThan(alp.r);
      if(!ltQ && tlp.r.equalTo(alp.r))
      {
        ltQ=(this.star<a.star)
      }
    }
    return(ltQ)
  }
  
  lessThanEqualTo(a){
    return(this.lessThan(a) || this.equalTo(a))
  }
  
  toString(forMathematica=false, prec = 4){
    let tf;
    if(forMathematica){
      if(this.star){
        tf = "True";
      }
      else tf="False";
      return "{"+this.l.toString(forMathematica,prec)+", "+this.r.toString(forMathematica,prec)+", "+tf+"}"
    }
    else{
      if(this.star){
        tf = "true";
      }
      else tf="false";
      return "["+this.l.toString(forMathematica,prec)+", "+this.r.toString(forMathematica,prec)+", "+tf+"]"
    }
  }
}


class group{
  constructor(ugens,name="",noneedtogenerateQ=false)
  {
    let gpinfo = makegroup(ugens,name,noneedtogenerateQ);
    this.groupElements = gpinfo.groupElements;
    this.name = gpinfo.name;
  }
  
  toString(forMathematica=false,prec=4){
    let out="{";
    this.groupElements.map(x=>{out=out+x.toString(forMathematica,prec)+",\n"; return out});
    out = out.substring(0, out.length - 2);
    out=out+"}";
    return out;
  }
  
  actOn(q){
    let list=[];
    let nextQuat,newQ,i;
    for(i=0;i<this.groupElements.length;i++){
      let nextQuat = this.groupElements[i].acton(q);
      let newQ = true; // so far g.q seems new...
      let j=0; // lets walk through the list and see
      while(j<list.length && newQ){
        newQ = !(nextQuat.equalTo(list[j]));
        j++
      }
      if(newQ){list.push(nextQuat)}
    }
    return list;
    
  }
}

/////////////////////////////
//
//
//

function makegroup(ugens, name ="", noneedtogenerategeroupQ=false)
{
    const gens = ugens.map(x=>x.standardize()).slice(0);
   // makeSortedGroupFrom(gens,name);
    const MAXDEPTH = 30; // this may not be enough for IxI;
    let groupElements = arraySort(gens.slice(0)) ;// make a copy of the generators

    if(noneedtogenerategeroupQ){
      console.log('tada')
      return {"groupElements":ugens,"name":name};

    }
    //otherwise, keep on trucking

    let nextgroupElements = groupElements.slice(0);
    
  // we'll keep the groupElements list sorted;
  // we begin with a copy of the generators
  
  // at each stage, we have these to work with:
  let newElements = groupElements.slice(0); // make another copy of the generators
  // producing a list of 
  let nextElements = [];
  let nextElement;
  
  let i = 0, j=0,k=0,l=0;
  let newQ;
  let start=0, end=0, mid=0, doneQ;
  
  // in the outer loop, as long as we still have some newElements to work with ...
//2
  while (i<MAXDEPTH && newElements.length>0){
    i++; // a safety counter
    
    let nextElements = [];
 
  let newElt, gen;
  for (k=0;k<newElements.length;k++)
  {   newElt=newElements[k];
      for(l=0;l<gens.length;l++)
      {   gen=gens[l];
      
        // to obtain
        nextElement = newElt.mult(gen).copy().standardize();   //prob some overkill here from debugging
       
         // let test1 = new quat(0., 0., 0.5, - 0.866);
         // let test2 = new quat(0., 0., -0.5, 0.866);
        //  let test1 = new quat(0.,0.,1.,0.);
         /* let test2 = new quat(.5,-.866,.0,.0);
          let test3 = new quat(-.5,.866,.0,.0);
          let test4 = new quat(-.5,-.866,.0,.0);
          let test5 = new quat(.5,.866,.0,.0);
          */
        let test1 = new quat(0.,  -1.,0.,0.)
          if(
            // nextElement.l.equalTo(test4,.01)||
            // nextElement.l.equalTo(test3,.01)||
             // nextElement.l.equalTo(test2,.01)||
              nextElement.l.equalTo(test1,.01)
            )
          
        
            {  // then do this for debugging:
               console.log("groupElements")
            }
        
        
        
        newQ=true;
        j=0;
        
        // now check to see if nextElement should be kept;
        // we apply a binary search for efficiency:
        doneQ=false;
        start = 0; end = groupElements.length - 1;
        // are we already less than start? If so, add now and done
        if(nextElement.lessThan(groupElements[start])){
 //5
          nextElements.push(nextElement);
          groupElements.unshift(nextElement);
          doneQ = true;
        }
  //4      // or are we already greater than end? If so add now and done
        else if(groupElements[end].lessThan(nextElement)){
   //5
          nextElements.push(nextElement);
          groupElements.push(nextElement);
          doneQ=true;
        }
//4  
      // or can we already ignore these? done
        else 
  
      {
  /*5*/  let t1=(groupElements[end].equalTo(nextElement));
          let t2 = (groupElements[start].equalTo(nextElement));
          doneQ=t1||t2;
        }
       
 //4         
          //otherwise, we have start < nextElement < end, and we can proceed.
     
        while(!doneQ){
 //5         
          // for start + 1 < end, we have start < mid < end 
          
          mid =  (end+start-((end+start)%2))/2;// floor (e+s)/2
          
          // however, for start+1>=end, we have mid = start.
          if(start+1==end)// we can only go in the middle:
    //6
          { doneQ=true;
            nextElements.push(nextElement);
            groupElements.splice(end,0,nextElement)
          }
    //5      
          // otherwise start<mid<end and start < nextElement < end
          // check if "next = mid"
          else if (nextElement.equalTo(groupElements[mid])){
            doneQ=true;
          }
          // so mid < next or next < mid
          else if (groupElements[mid].lessThan(nextElement)){
            start = mid;  //end stays the same
          }
          else // if (nextElement.lessThan(groupElements[mid]))
          {
            end = mid; // start stays the same
          }
     
        }//end while! doneQ
     //4      
       // return nextElements;
    
  /*  }) // end w the generators
    //3
      //return nextElements
  }) // end w the newElements
  //2*/
}}
  
  // whatd'we got for the new go around?
  
  newElements=nextElements;
}
//1
  return {"groupElements":groupElements,"name":name};
}
//0 and done





function toString(forMathematica=false, prec = 4){
  let tf;
  if(forMathematica){
    if(this.star){
      tf = "True";
    }
    else tf="False";
    return "{"+this.l.toString(forMathematica,prec)+", "+this.r.toString(forMathematica,prec)+", "+tf+"}"
  }
  else{
    if(this.star){
      tf = "true";
    }
    else tf="false";
    return "["+this.l.toString(forMathematica,prec)+", "+this.r.toString(forMathematica,prec)+", "+tf+"]"
  }
}


