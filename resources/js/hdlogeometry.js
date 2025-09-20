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




const qOne = new quat(1.0,0,0,0).positivize()

const qW = new quat(-.5,.5,.5,.5) // note normalized
const qWp= new quat(.5,.5,.5,.5)
const qWi= new quat(.5,-.5,.5,.5)
const qWj= new quat(.5,.5,-.5,.5)
const qWk= new quat(.5,.5,.5,-.5)

const qI = new quat(0,1,0,0)

const qJ = new quat(0,0,1,0)
const qK = new quat(0,0,0,1)

const qmone = new quat(-1.0,0,0,0)
const qmOne = new quat(-1.0,0,0,0)
const qmI = new quat(0,-1.0,0,0)
const qmJ = new quat(0,0,-1.0,0)
const qmK = new quat(0,0,0,-1.0)


// named qActions

const qIdentity = new qAction(qOne, qOne)
const qOneOne = qIdentity
const qIOne = new qAction(qI,qOne)
const qWOne = new qAction(qW,qOne)

const qMOneOne = new qAction(qmOne,qOne)
const qmIOneOne = new qAction(qmI,qOne)

//const rots4I[qOneOne,qIone,qmOneONe,qmIOneOne]
//const rots4J[qOneOne,qJone,qmOneONe,qmJOneOne]