#pragma once

namespace QM{

typedef float real;

void     exactinit();

real   orient2d(real *pa, real *pb, real *pc);
real   orient2dfast(real *pa, real *pb, real *pc);
real   orient3d(real *pa, real *pb, real *pc, real *pd);
real   orient3dfast(real *pa, real *pb, real *pc, real *pd);

real   incircle(real *pa, real *pb, real *pc, real *pd);
real   incirclefast(real *pa, real *pb, real *pc, real *pd);

real   insphere(real *pa, real *pb, real *pc, real *pd, real *pe);
real   inspherefast(real *pa, real *pb, real *pc, real *pd, real *pe);

}//end namespace QM                                                            
    
