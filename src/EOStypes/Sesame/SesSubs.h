#ifndef EOSLIB_SES_SUBS_H
#define EOSLIB_SES_SUBS_H

int SESsrch(double x, double *array, int n);
int SESsrchk(double x, double *array, int n, int k);


void SESratfn1(double x, double *xa, int idx, int nx, int kx,
                         double *array, int ky, double *y);

void SESratfn2(double x, double y,
               double *xa, int ix, int nx,
               double *ya, int iy, int ny,
               double *tbls, int ifn, double *z);

void SESbilinear(double x, double y,
                 double *xa, int i, int nx,
                 double *ya, int j, int ny,
                 double *tbls, double *z);

void SESinterp2(double x, double y,
                double *xa, int i, int nx,
                double *ya, int j, int ny,
                double *tbls, double *z);

#endif // EOSLIB_SES_SUBS_H
