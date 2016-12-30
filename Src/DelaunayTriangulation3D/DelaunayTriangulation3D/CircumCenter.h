// http://www.mcs.anl.gov/~fathom/meshkit-docs/html/circumcenter_8cpp_source.html

#pragma once


//#include "GeomPredicates.hpp"
//#include "basic_math.hpp"

//using namespace QM;
void TetCircumCenter( float *a, float *b, float *c, float *d, float *r, float *p);
void TriCircumCenter2D(float *a, float *b, float *c, float *r, float *p);
void TriCircumCenter3D(float *a, float *b, float *c, float *r, float *p);
void TriCircumCenter3D(float *a, float *b, float *c, float *r);


//namespace UnitTest
//{
//   int test_tet_circumcenter();
//   int test_tri2d_circumcenter();
//   int test_tri3d_circumcenter();
//}
//   
