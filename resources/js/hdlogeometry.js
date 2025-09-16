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




const qOne = new quat(1,0,0,0).positivize()
const qmone = new quat(-1,0,0,0)
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

