// http://www.mcs.anl.gov/~fathom/meshkit-docs/html/circumcenter_8cpp_source.html

#include "stdafx.h"
#include "CircumCenter.h"
#include "GeomPredicates.h"

//
//
//Let a, b, c, d, and m be vectors in R^3.  Let ax, ay, and az be the components
//of a, and likewise for b, c, and d.  Let |a| denote the Euclidean norm of a,
//and let a x b denote the cross product of a and b.  Then
//
//    |                                                                       |
//    | |d-a|^2 [(b-a)x(c-a)] + |c-a|^2 [(d-a)x(b-a)] + |b-a|^2 [(c-a)x(d-a)] |
//    |                                                                       |
//r = -------------------------------------------------------------------------,
//                              | bx-ax  by-ay  bz-az |
//                            2 | cx-ax  cy-ay  cz-az |
//                              | dx-ax  dy-ay  dz-az |
//
//and
//
//        |d-a|^2 [(b-a)x(c-a)] + |c-a|^2 [(d-a)x(b-a)] + |b-a|^2 [(c-a)x(d-a)]
//m = a + ---------------------------------------------------------------------.
//                                | bx-ax  by-ay  bz-az |
//                              2 | cx-ax  cy-ay  cz-az |
//                                | dx-ax  dy-ay  dz-az |
//
//Some notes on stability:
//
//- Note that the expression for r is purely a function of differences between
//  coordinates.  The advantage is that the relative error incurred in the
//  computation of r is also a function of the _differences_ between the
//  vertices, and is not influenced by the _absolute_ coordinates of the
//  vertices.  In most applications, vertices are usually nearer to each other
//  than to the origin, so this property is advantageous.  If someone gives you
//  a formula that doesn't have this property, be wary.
//
//  Similarly, the formula for m incurs roundoff error proportional to the
//  differences between vertices, but does not incur roundoff error proportional
//  to the absolute coordinates of the vertices until the final addition.

//- These expressions are unstable in only one case:  if the denominator is
//  close to zero.  This instability, which arises if the tetrahedron is nearly
//  degenerate, is unavoidable.  Depending on your application, you may want
//  to use exact arithmetic to compute the value of the determinant.
//  Fortunately, this determinant is the basis of the well-studied 3D orientation
//  test, and several fast algorithms for providing accurate approximations to
//  the determinant are available.  Some resources are available from the
//  "Numerical and algebraic computation" page of Nina Amenta's Directory of
//  Computational Geometry Software:

//  http://www.geom.umn.edu/software/cglist/alg.html

//  If you're using floating-point inputs (as opposed to integers), one
//  package that can estimate this determinant somewhat accurately is my own:

//  http://www.cs.cmu.edu/~quake/robust.html

//- If you want to be even more aggressive about stability, you might reorder
//  the vertices of the tetrahedron so that the vertex `a' (which is subtracted
//  from the other vertices) is, roughly speaking, the vertex at the center of
//  the minimum spanning tree of the tetrahedron's four vertices.  For more
//  information about this idea, see Steven Fortune, "Numerical Stability of
//  Algorithms for 2D Delaunay Triangulations," International Journal of
//  Computational Geometry & Applications 5(1-2):193-213, March-June 1995.

//For completeness, here are stable expressions for the circumradius and
//circumcenter of a triangle, in R^2 and in R^3.  Incidentally, the expressions
//given here for R^2 are better behaved in terms of relative error than the
//suggestions currently given in the Geometry Junkyard
//(http://www.ics.uci.edu/~eppstein/junkyard/triangulation.html).

//Triangle in R^2:
//
//     |b-a| |c-a| |b-c|            < Note: You only want to compute one sqrt, so
//r = ------------------,             use sqrt{ |b-a|^2 |c-a|^2 |b-c|^2 }
//      | bx-ax  by-ay |
//    2 | cx-ax  cy-ay |
//
//          | by-ay  |b-a|^2 |
//          | cy-ay  |c-a|^2 |
//mx = ax - ------------------,
//            | bx-ax  by-ay |
//          2 | cx-ax  cy-ay |
//
//          | bx-ax  |b-a|^2 |
//          | cx-ax  |c-a|^2 |
//my = ay + ------------------.
//            | bx-ax  by-ay |
//          2 | cx-ax  cy-ay |
//
//Triangle in R^3:
//
//    |                                                           |
//    | |c-a|^2 [(b-a)x(c-a)]x(b-a) + |b-a|^2 (c-a)x[(b-a)x(c-a)] |
//    |                                                           |
//r = -------------------------------------------------------------,
//                         2 | (b-a)x(c-a) |^2
//
//        |c-a|^2 [(b-a)x(c-a)]x(b-a) + |b-a|^2 (c-a)x[(b-a)x(c-a)]
//m = a + ---------------------------------------------------------.
//                           2 | (b-a)x(c-a) |^2
//
//Finally, here's some C code that computes the vector m-a (whose norm is r) in
//each of these three cases.  Notice the #ifdef statements, which allow you to
//choose whether or not to use my aforementioned package to approximate the
//determinant.  (No attempt is made here to reorder the vertices to improve
//stability.)

/*****************************************************************************/
/*                                                                           */
/*  tetcircumcenter()   Find the circumcenter of a tetrahedron.              */
/*                                                                           */
/*  The result is returned both in terms of xyz coordinates and xi-eta-zeta  */
/*  coordinates, relative to the tetrahedron's point `a' (that is, `a' is    */
/*  the origin of both coordinate systems).  Hence, the xyz coordinates      */
/*  returned are NOT absolute; one must add the coordinates of `a' to        */
/*  find the absolute coordinates of the circumcircle.  However, this means  */
/*  that the result is frequently more accurate than would be possible if    */
/*  absolute coordinates were returned, due to limited floating-point        */
/*  precision.  In general, the circumradius can be computed much more       */
/*  accurately.                                                              */
/*                                                                           */
/*  The xi-eta-zeta coordinate system is defined in terms of the             */
/*  tetrahedron.  Point `a' is the origin of the coordinate system.          */
/*  The edge `ab' extends one unit along the xi axis.  The edge `ac'         */
/*  extends one unit along the eta axis.  The edge `ad' extends one unit     */
/*  along the zeta axis.  These coordinate values are useful for linear      */
/*  interpolation.                                                           */
/*                                                                           */
/*  If `xi' is NULL on input, the xi-eta-zeta coordinates will not be        */
/*  computed.                                                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
void tetcircumcenter(float a[3], float b[3], float c[3], float d[3],
	float circumcenter[3], float *xi, float *eta, float *zeta)
{
	float xba, yba, zba, xca, yca, zca, xda, yda, zda;
	float balength, calength, dalength;
	float xcrosscd, ycrosscd, zcrosscd;
	float xcrossdb, ycrossdb, zcrossdb;
	float xcrossbc, ycrossbc, zcrossbc;
	float denominator;
	float xcirca, ycirca, zcirca;

	/* Use coordinates relative to point `a' of the tetrahedron. */
	xba = b[0] - a[0];
	yba = b[1] - a[1];
	zba = b[2] - a[2];
	xca = c[0] - a[0];
	yca = c[1] - a[1];
	zca = c[2] - a[2];
	xda = d[0] - a[0];
	yda = d[1] - a[1];
	zda = d[2] - a[2];
	/* Squares of lengths of the edges incident to `a'. */
	balength = xba * xba + yba * yba + zba * zba;
	calength = xca * xca + yca * yca + zca * zca;
	dalength = xda * xda + yda * yda + zda * zda;
	/* Cross products of these edges. */
	xcrosscd = yca * zda - yda * zca;
	ycrosscd = zca * xda - zda * xca;
	zcrosscd = xca * yda - xda * yca;
	xcrossdb = yda * zba - yba * zda;
	ycrossdb = zda * xba - zba * xda;
	zcrossdb = xda * yba - xba * yda;
	xcrossbc = yba * zca - yca * zba;
	ycrossbc = zba * xca - zca * xba;
	zcrossbc = xba * yca - xca * yba;

	/* Calculate the denominator of the formulae. */
#ifdef EXACT
	/* Use orient3d() from http://www.cs.cmu.edu/~quake/robust.html     */
	/*   to ensure a correctly signed (and reasonably accurate) result, */
	/*   avoiding any possibility of division by zero.                  */
	denominator = 0.5 / orient3d(b, c, d, a);
#else
	/* Take your chances with floating-point roundoff. */
	//printf( " Warning: IEEE floating points used: Define -DEXACT in makefile \n");
	denominator = 0.5f / (xba * xcrosscd + yba * ycrosscd + zba * zcrosscd);
#endif

	/* Calculate offset (from `a') of circumcenter. */
	xcirca = (balength * xcrosscd + calength * xcrossdb + dalength * xcrossbc) * denominator;
	ycirca = (balength * ycrosscd + calength * ycrossdb + dalength * ycrossbc) * denominator;
	zcirca = (balength * zcrosscd + calength * zcrossdb + dalength * zcrossbc) * denominator;
	circumcenter[0] = xcirca;
	circumcenter[1] = ycirca;
	circumcenter[2] = zcirca;

	if (xi != (float *)NULL)
	{
		/* To interpolate a linear function at the circumcenter, define a    */
		/*   coordinate system with a xi-axis directed from `a' to `b',      */
		/*   an eta-axis directed from `a' to `c', and a zeta-axis directed  */
		/*   from `a' to `d'.  The values for xi, eta, and zeta are computed */
		/*   by Cramer's Rule for solving systems of linear equations.       */
		*xi = (xcirca * xcrosscd + ycirca * ycrosscd + zcirca * zcrosscd) * (2.0f * denominator);
		*eta = (xcirca * xcrossdb + ycirca * ycrossdb + zcirca * zcrossdb) * (2.0f * denominator);
		*zeta = (xcirca * xcrossbc + ycirca * ycrossbc + zcirca * zcrossbc) * (2.0f * denominator);
	}
}



/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*  tricircumcenter()   Find the circumcenter of a triangle.                 */
/*                                                                           */
/*  The result is returned both in terms of x-y coordinates and xi-eta       */
/*  coordinates, relative to the triangle's point `a' (that is, `a' is       */
/*  the origin of both coordinate systems).  Hence, the x-y coordinates      */
/*  returned are NOT absolute; one must add the coordinates of `a' to        */
/*  find the absolute coordinates of the circumcircle.  However, this means  */
/*  that the result is frequently more accurate than would be possible if    */
/*  absolute coordinates were returned, due to limited floating-point        */
/*  precision.  In general, the circumradius can be computed much more       */
/*  accurately.                                                              */
/*                                                                           */
/*  The xi-eta coordinate system is defined in terms of the triangle.        */
/*  Point `a' is the origin of the coordinate system.  The edge `ab' extends */
/*  one unit along the xi axis.  The edge `ac' extends one unit along the    */
/*  eta axis.  These coordinate values are useful for linear interpolation.  */
/*                                                                           */
/*  If `xi' is NULL on input, the xi-eta coordinates will not be computed.   */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
void tricircumcenter(float a[2], float b[2], float c[2], float circumcenter[2], 
                     float *xi, float *eta)
{
  float xba, yba, xca, yca;
  float balength, calength;
  float denominator;
  float xcirca, ycirca;

  /* Use coordinates relative to point `a' of the triangle. */
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba;
  calength = xca * xca + yca * yca;

  /* Calculate the denominator of the formulae. */
#ifdef EXACT
  /* Use orient2d() from http://www.cs.cmu.edu/~quake/robust.html     */
  /*   to ensure a correctly signed (and reasonably accurate) result, */
  /*   avoiding any possibility of division by zero.                  */
  denominator = 0.5f / orient2d(b, c, a);
#else
  /* Take your chances with floating-point roundoff. */
  denominator = 0.5f / (xba * yca - yba * xca);
#endif

  /* Calculate offset (from `a') of circumcenter. */
  xcirca = (yca * balength - yba * calength) * denominator;
  ycirca = (xba * calength - xca * balength) * denominator;
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;

  if (xi != (float *) NULL) {
    /* To interpolate a linear function at the circumcenter, define a     */
    /*   coordinate system with a xi-axis directed from `a' to `b' and    */
    /*   an eta-axis directed from `a' to `c'.  The values for xi and eta */
    /*   are computed by Cramer's Rule for solving systems of linear      */
    /*   equations.                                                       */
    *xi = (xcirca * yca - ycirca * xca) * (2.0f * denominator);
    *eta = (ycirca * xba - xcirca * yba) * (2.0f * denominator);
  }
}

/****************************************************************************/

/*****************************************************************************/
 /*                                                                           */
 /*  tricircumcenter3d()   Find the circumcenter of a triangle in 3D.         */
 /*                                                                           */
 /*  The result is returned both in terms of xyz coordinates and xi-eta       */
 /*  coordinates, relative to the triangle's point `a' (that is, `a' is       */
 /*  the origin of both coordinate systems).  Hence, the xyz coordinates      */
 /*  returned are NOT absolute; one must add the coordinates of `a' to        */
 /*  find the absolute coordinates of the circumcircle.  However, this means  */
 /*  that the result is frequently more accurate than would be possible if    */
 /*  absolute coordinates were returned, due to limited floating-point        */
 /*  precision.  In general, the circumradius can be computed much more       */
 /*  accurately.                                                              */
 /*                                                                           */
 /*  The xi-eta coordinate system is defined in terms of the triangle.        */
 /*  Point `a' is the origin of the coordinate system.  The edge `ab' extends */
 /*  one unit along the xi axis.  The edge `ac' extends one unit along the    */
 /*  eta axis.  These coordinate values are useful for linear interpolation.  */
 /*                                                                           */
 /*  If `xi' is NULL on input, the xi-eta coordinates will not be computed.   */
 /*                                                                           */
 /*****************************************************************************/
 /*****************************************************************************/
 void tricircumcenter3d(float a[3], float b[3], float c[3], float circumcenter[3], 
                        float *xi, float *eta)
 {
   float xba, yba, zba, xca, yca, zca;
   float balength, calength;
   float xcrossbc, ycrossbc, zcrossbc;
   float denominator;
   float xcirca, ycirca, zcirca;
 
   /* Use coordinates relative to point `a' of the triangle. */
   xba = b[0] - a[0];
   yba = b[1] - a[1];
   zba = b[2] - a[2];
   xca = c[0] - a[0];
   yca = c[1] - a[1];
   zca = c[2] - a[2];
   /* Squares of lengths of the edges incident to `a'. */
   balength = xba * xba + yba * yba + zba * zba;
   calength = xca * xca + yca * yca + zca * zca;
 
   /* Cross product of these edges. */
 #ifdef EXACT
   /* Use orient2d() from http://www.cs.cmu.edu/~quake/robust.html     */
   /*   to ensure a correctly signed (and reasonably accurate) result, */
   /*   avoiding any possibility of division by zero.                  */
 
   A[0] = b[1]; A[1] = b[2];
   B[0] = c[1]; B[1] = c[2];
   C[0] = a[1]; C[1] = a[2];
   xcrossbc = orient2d(A, B, C);
 
   A[0] = c[0]; A[1] = c[2];
   B[0] = b[0]; B[1] = b[2];
   C[0] = a[0]; C[1] = a[2];
   ycrossbc = orient2d(A, B, C);
 
   A[0] = b[0]; A[1] = b[1];
   B[0] = c[0]; B[1] = c[1];
   C[0] = a[0]; C[1] = a[1];
   zcrossbc = orient2d(A, B, C);
 
   /*
   xcrossbc = orient2d(b[1], b[2], c[1], c[2], a[1], a[2]);
   ycrossbc = orient2d(b[2], b[0], c[2], c[0], a[2], a[0]);
   zcrossbc = orient2d(b[0], b[1], c[0], c[1], a[0], a[1]);
   */
 #else
   printf( " Warning: IEEE floating points used: Define -DEXACT in makefile \n");
   /* Take your chances with floating-point roundoff. */
   xcrossbc = yba * zca - yca * zba;
   ycrossbc = zba * xca - zca * xba;
   zcrossbc = xba * yca - xca * yba;
 #endif
 
   /* Calculate the denominator of the formulae. */
   denominator = 0.5f / (xcrossbc * xcrossbc + ycrossbc * ycrossbc +
                        zcrossbc * zcrossbc);
 
   /* Calculate offset (from `a') of circumcenter. */
   xcirca = ((balength * yca - calength * yba) * zcrossbc -
             (balength * zca - calength * zba) * ycrossbc) * denominator;
   ycirca = ((balength * zca - calength * zba) * xcrossbc -
             (balength * xca - calength * xba) * zcrossbc) * denominator;
   zcirca = ((balength * xca - calength * xba) * ycrossbc -
             (balength * yca - calength * yba) * xcrossbc) * denominator;
   circumcenter[0] = xcirca;
   circumcenter[1] = ycirca;
   circumcenter[2] = zcirca;
 
   if (xi != (float *) NULL) {
     /* To interpolate a linear function at the circumcenter, define a     */
     /*   coordinate system with a xi-axis directed from `a' to `b' and    */
     /*   an eta-axis directed from `a' to `c'.  The values for xi and eta */
     /*   are computed by Cramer's Rule for solving systems of linear      */
     /*   equations.                                                       */
 
     /* There are three ways to do this calculation - using xcrossbc, */
     /*   ycrossbc, or zcrossbc.  Choose whichever has the largest    */
     /*   magnitude, to improve stability and avoid division by zero. */
     if (((xcrossbc >= ycrossbc) ^ (-xcrossbc > ycrossbc)) &&
         ((xcrossbc >= zcrossbc) ^ (-xcrossbc > zcrossbc))) {
       *xi = (ycirca * zca - zcirca * yca) / xcrossbc;
       *eta = (zcirca * yba - ycirca * zba) / xcrossbc;
     } else if ((ycrossbc >= zcrossbc) ^ (-ycrossbc > zcrossbc)) {
       *xi = (zcirca * xca - xcirca * zca) / ycrossbc;
       *eta = (xcirca * zba - zcirca * xba) / ycrossbc;
     } else {
       *xi = (xcirca * yca - ycirca * xca) / zcrossbc;
       *eta = (ycirca * xba - xcirca * yba) / zcrossbc;
     }
   }
 }

 /****************************************************************************/
 void TriCircumCenter2D( float *a, float *b, float *c, float *result, 
                         float *param)
 {
    tricircumcenter(a, b, c, result, &param[0], &param[1]);
 
    result[0] += a[0];
    result[1] += a[1];
 }

 /****************************************************************************/
 void TriCircumCenter3D( float *a, float *b, float *c, float *result, 
                          float *param)
 {
    tricircumcenter3d(a, b, c, result, &param[0], &param[1]);
    result[0] += a[0];
    result[1] += a[1];
    result[2] += a[2];
 }
 
 /****************************************************************************/
 void TriCircumCenter3D( float *a, float *b, float *c, float *result)
 {
    float xi, eta;
    tricircumcenter3d(a, b, c, result, &xi, & eta);
    result[0] += a[0];
    result[1] += a[1];
    result[2] += a[2];
 }

 /****************************************************************************/
 void TetCircumCenter( float *a, float *b, float *c, float *d, float *result, 
                       float *param)
 {
    float orient = QM::orient3d(a, b, c, d);
 
    if(orient < 0.0) 
       tetcircumcenter(a, c, b, d, result, &param[0], &param[1], &param[2]);
    else
       tetcircumcenter(a, b, c, d, result, &param[0], &param[1], &param[2]);
 
    result[0] += a[0];
    result[1] += a[1];
    result[2] += a[2];
 }

