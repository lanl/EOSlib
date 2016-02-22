#include "SesSubs.h"

inline double abs(double x) { return x >= 0 ? x : -x; }

int SESsrch(double x, double *array, int n)
{   // binary search
// return  -1  if x < array[0]
//        i    if array[i] <= x < array[i+1]
//        n-1  if x == array[n-1]
//        n    if x > array[n-1]
    if( x < array[0] )
        return -1;
    if( x == array[n-1] )
        return n-1;
    if( x > array[n-1] )
        return n;
    int top = n-1;
    int bot = 0;
    while( top-bot > 1 )
    {
        int mid = (bot+top)/2;
        if( x < array[mid] )
            top = mid;
        else
            bot = mid;
    }
    return bot;
}

int SESsrchk(double x, double *array, int n, int k)
{   // binary search, arrary[i*k] for i = 0 . . . n-1
// return  -1  if x < array[0]
//        i    if array[i] <= x < array[i+1]
//        n-1  if x == array[n-1]
//        n    if x > array[n-1]
    if( x < array[0] )
        return -1;
    if( x == array[k*(n-1)] )
        return n-1;
    if( x > array[k*(n-1)] )
        return n;
    int top = n-1;
    int bot = 0;
    while( top-bot > 1 )
    {
        int mid = (bot+top)/2;
        if( x < array[mid*k] )
            top = mid;
        else
            bot = mid;
    }
    return bot;
}

// Ref: Rational Function Method of Interpolation
//      G. I. Kerley, LA-6903-MS, Aug. 1977
void SESratfn1(double x, double *xa, int idx, int nx, int kx,
                         double *array, int ky, double *y)
{
    int ix = kx*idx;
    int iy = ky*idx;
    double q = x-xa[ix];
    double d = xa[ix+kx]-xa[ix];
    double r = d-q;
    double s = (array[iy+ky]-array[iy])/d;
    if( idx == 0 )
    {
        double sp = (array[iy+ky+ky]-array[iy+ky])/(xa[ix+kx+kx]-xa[ix+kx]);
        double c2 = (sp-s)/(xa[ix+kx+kx]-xa[ix]);
        if( s*(s-d*c2)<=0. )
            c2 = s/d;
        y[0] = array[iy]+q*(s-r*c2);
        y[1] = s+(q-r)*c2;
    }
    else
    {
        double dm = xa[ix]-xa[ix-kx];
        double sm = (array[iy]-array[iy-ky])/dm;
        double c1 = (s-sm)/(d+dm);
        if( idx >= nx-2 )
        {
            y[0] = array[iy]+q*(s-r*c1);
            y[1] = s+(q-r)*c1;
        }
        else
        {
            if( idx == 1 && sm*(sm-dm*c1) <= 0. )
                c1 = (s-sm-sm)/d;
            double sp = (array[iy+ky+ky]-array[iy+ky])/(xa[ix+kx+kx]-xa[ix+kx]);
            double c2 = (sp-s)/(xa[ix+kx+kx]-xa[ix]);
            double c3 = abs(c2*r);
            double c4 = c3+abs(c1*q);
            if( c4 > 0. )
                c3 = c3/c4;
            c4 = c2+c3*(c1-c2);
            y[0] = array[iy]+q*(s-r*c4);
            y[1] = s+(q-r)*c4+d*(c4-c2)*(1.-c3);
        }
    }
}

void SESratfn2(double x, double y,
               double *xa, int ix, int nx,
               double *ya, int iy, int ny,
               double *tbls, int ifn, double *z)
{
    if( ifn == 1 )
    {
        SESbilinear(x,y,xa,ix,nx,ya,iy,ny,tbls,z);
        return;
    }
    double fx[2], fpx[2], fy[2], fpy[2];
    int k;
    int iz = ix+nx*iy;
    double qx = x-xa[ix];
    double dx = xa[ix+1]-xa[ix];
    double rx = xa[ix+1]-x;
    for( k=0; k<2; k++ )
    {
       double s = (tbls[iz+1]-tbls[iz])/dx;
       fx[k]  = 0.5*(tbls[iz]+qx*s);
       fpx[k] = 0.5*s;
       if( ix == 0 )
       {
          double sp = (tbls[iz+2]-tbls[iz+1])/(xa[ix+2]-xa[ix+1]);
          double c2 = (sp-s)/(xa[ix+2]-xa[ix]);
          if( s*(s-dx*c2) <= 0. )
              c2 = s/dx;
          fx[k]  -= qx*rx*c2;
          fpx[k] += (qx-rx)*c2;
       }
       else
       {
          double dm = xa[ix]-xa[ix-1];
          double sm = (tbls[iz]-tbls[iz-1])/dm;
          double c1 = (s-sm)/(dx+dm);
          if( ix >= nx-2 )
          {
             fx[k]  -= qx*rx*c1;
             fpx[k] += (qx-rx)*c1;
          }
          else
          {
             if( ix==1 && sm*(sm-dm*c1)<=0. )
                 c1=(s-sm-sm)/dx;
             double sp = (tbls[iz+2]-tbls[iz+1])/(xa[ix+2]-xa[ix+1]);
             double c2 = (sp-s)/(xa[ix+2]-xa[ix]);
             double c3 = abs(c2*rx);
             double c4 = c3+abs(c1*qx);
             if( c4>0. )
                 c3=c3/c4;
             c4 = c2+c3*(c1-c2);
             fx[k]  -= qx*rx*c4;
             fpx[k] += (qx-rx)*c4+dx*(c4-c2)*(1.-c3);
          }
       } 
       iz = iz+nx;
    }
    iz =ix+nx*iy;
    double qy = y-ya[iy];
    double dy = ya[iy+1]-ya[iy];
    double ry = ya[iy+1]-y;
    for( k=0; k<2; k++ )
    {
       double s = (tbls[iz+nx]-tbls[iz])/dy;
       fy[k]  = 0.5*(tbls[iz]+qy*s);
       fpy[k] = 0.5*s;
       if( iy == 0 )
       {
          double sp = (tbls[iz+2*nx]-tbls[iz+nx])/(ya[iy+2]-ya[iy+1]);
          double c2 = (sp-s)/(ya[iy+2]-ya[iy]);
          if( s*(s-dy*c2) <= 0. )
              c2=s/dy;
          fy[k]  -= qy*ry*c2;
          fpy[k] += (qy-ry)*c2;
       }
       else
       {
          double dm = ya[iy]-ya[iy-1];
          double sm = (tbls[iz]-tbls[iz-nx])/dm;
          double c1 = (s-sm)/(dy+dm);
          if( iy >= ny-2 )
          {
             fy[k]  -= qy*ry*c1;
             fpy[k] += (qy-ry)*c1;
          }
          else
          {
             if( iy==1 && sm*(sm-dm*c1)<=0. )
                 c1=(s-sm-sm)/dy;
             double sp = (tbls[iz+2*nx]-tbls[iz+nx])/(ya[iy+2]-ya[iy+1]);
             double c2 = (sp-s)/(ya[iy+2]-ya[iy]);
             double c3 = abs(c2*ry);
             double c4 = c3+abs(c1*qy);
             if( c4 > 0. )
                 c3=c3/c4;
             c4 = c2+c3*(c1-c2);
             fy[k]  -= qy*ry*c4;
             fpy[k] += (qy-ry)*c4+dy*(c4-c2)*(1.-c3);
          }
       }
       iz = iz+1;
    }
    double sy = (fx[1] - fx[0])/dy;
    double sx = (fy[1] - fy[0])/dx;
    z[0] = (fx[0]+sy*qy) + (fy[0]+sx*qx);
    z[1] = sx + (qy*fpx[1]+ry*fpx[0])/dy;
    z[2] = sy + (qx*fpy[1]+rx*fpy[0])/dx;
}
      
void SESbilinear(double x, double y,
                 double *xa, int i, int nx,
                 double *ya, int j, int ny,
                 double *tbls, double *z)
{
    double qx = x       - xa[i];
    double rx = xa[i+1] - x;
    double dx = xa[i+1] - xa[i];
 
    double qy = y       - ya[j];
    double ry = ya[j+1] - y;
    double dy = ya[j+1] - ya[j];

    int k = i+nx*j;
    double fx1 = (qx*tbls[k+1]    + rx*tbls[k]   )/dx;
    double fx2 = (qx*tbls[k+nx+1] + rx*tbls[k+nx])/dx;
    double fy1 = (qy*tbls[k+nx]   + ry*tbls[k]   )/dy;
    double fy2 = (qy*tbls[k+1+nx] + ry*tbls[k+1] )/dy;

    z[0] = (qy*fx2+ry*fx1)/dy;
    z[1] = (fy2-fy1)/dx;
    z[2] = (fx2-fx1)/dy;
}

// Equivalent to SESratfn2 in terms of SESratfn1 and SESbilinear
void SESinterp2(double x, double y,
                double *xa, int i, int nx,
                double *ya, int j, int ny,
                double *tbls, int ifn, double *z)
{
    SESbilinear(x, y, xa, i, nx, ya, j, ny, tbls, z);
    if( ifn == 1 )
        return;
    
    int one = 1;
    double fx1[2], fx2[2];
    double fy1[2], fy2[2];
    
    SESratfn1(x, xa,i,nx,one, tbls+nx*j,one, fx1);
    SESratfn1(x, xa,i,nx,one, tbls+nx*(j+1),one, fx2);
    SESratfn1(y, ya,j,ny,one, tbls+i,nx, fy1);
    SESratfn1(y, ya,j,ny,one, tbls+i+1,nx, fy2);
    
    double qx = x-xa[i];
    double dx = xa[i+1]-xa[i];
    double qy = y-ya[j];
    double dy = ya[j+1]-ya[j];
    double sy = (fx2[0] - fx1[0])/dy;
    double sx = (fy2[0] - fy1[0])/dx;
    z[0] = -z[0] + (fx1[0]+sy*qy) + (fy1[0]+sx*qx);
    z[1] = -z[1] + sx + (fx1[1]+(fx2[1]-fx1[1])*qy/dy);
    z[2] = -z[2] + sy + (fy1[1]+(fy2[1]-fy1[1])*qx/dx);
}


/****************************************************
void SESinterp2(double x, double y,
                double *xa, int i, int nx,
                double *ya, int j, int ny,
                double *tbls, double *z)
{
    double s;
    int one = 1;
    int k, ki;
    double fx1[2], fx2[2];
    double fy1[2], fy2[2];
    
    double qx = x-xa[i];
    double dx = xa[i+1]-xa[i];
    double qy = y-ya[j];
    double dy = ya[j+1]-ya[j];

    k  = nx*j;
    ki = i + nx*j;
    SESratfn1(x, xa,i,nx,one, tbls+k,one, fx1);
    s = (tbls[ki+1] -tbls[ki])/dx;
    fx1[0] -= 0.5*(tbls[ki]+qx*s);
    fx1[1] -= 0.5*s;
    
    ki = ki + nx;
    SESratfn1(x, xa,i,nx,one, tbls+k+nx,one, fx2);
    s = (tbls[ki+1] -tbls[ki])/dx;
    fx2[0] -= 0.5*(tbls[ki]+qx*s);
    fx2[1] -= 0.5*s;
    
    ki = i + nx*j;
    SESratfn1(y, ya,j,ny,one, tbls+i,nx, fy1);
    s = (tbls[ki+nx] -tbls[ki])/dy;
    fy1[0] -= 0.5*(tbls[ki]+qy*s);
    fy1[1] -= 0.5*s;

    ki = ki+1;
    SESratfn1(y, ya,j,ny,one, tbls+i+1,nx, fy2);
    s = (tbls[ki+nx] -tbls[ki])/dy;
    fy2[0] -= 0.5*(tbls[ki]+qy*s);
    fy2[1] -= 0.5*s;
    
    double sy = (fx2[0] - fx1[0])/dy;
    double sx = (fy2[0] - fy1[0])/dx;
    z[0] = (fx1[0]+sy*qy) + (fy1[0]+sx*qx);
    z[1] = sx + (fx1[1]+(fx2[1]-fx1[1])*qy/dy);
    z[2] = sy + (fy1[1]+(fy2[1]-fy1[1])*qx/dx);
}
**************************************************************/
