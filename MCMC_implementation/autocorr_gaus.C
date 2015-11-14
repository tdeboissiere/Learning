{

#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TRandom3.h"
#include "TRandom2.h"
#include "TRandom.h"
#include "TF1.h"
#include "TF2.h"
#include "TGraph.h"
#include <vector>
#include "TFile.h"

using namespace std;

//Open the graphs fom the gaussian MCMC
//Plot the correlation graphs for the two variables x and y


TFile f("graph_MCMC_gaus.root","read");

TGraph *gr_x = (TGraph*)f.Get("X");
TGraph *gr_y = (TGraph*)f.Get("Y");

//Start at npts (past burn-in)
//Take npts <- start  -> npts

//The following definition of npts and start allows us to compute the autocorrelation with the lowest index=100 >60 = burn-in step
const int npts=40000;
const int lcor=300;
const double npts_double=1000;
const int start=100+npts;

double corr_x[lcor]; double corr_y[lcor];
double tabl[lcor];
for (int k=0; k<lcor; k++) {corr_x[k]=0; corr_y[k]=0; tabl[k]=k;}

double x_1,y_1,x_2,y_2;

for (int k=0; k<lcor; k++) {
   for (int i=start; i<start+npts; i++) {
   gr_x->GetPoint(i,x_1,y_1); gr_x->GetPoint(i-k,x_2,y_2);
   //cout << y_1<< "   " << y_2 << endl;
   corr_x[k]=corr_x[k]+y_1*y_2;
   gr_y->GetPoint(i,x_1,y_1); gr_y->GetPoint(i-k,x_2,y_2);
   corr_y[k]=corr_y[k]+(y_1-30)*(y_2-30)/9.;
   }
   corr_x[k]=(1/npts_double)*corr_x[k];
   corr_y[k]=(1/npts_double)*corr_y[k];
}

TGraph *gr_cx = new TGraph (lcor, tabl, corr_x);
gr_cx->SetLineColor(kBlack);
gr_cx->SetLineWidth(2);
gr_cx->SetName("corr_X");

TGraph *gr_cy = new TGraph (lcor, tabl, corr_y);
gr_cy->SetLineColor(kRed);
gr_cy->SetLineWidth(2);
gr_cy->SetName("corr_Y");

TFile fil("corr_MCMC_gaus.root","recreate");
gr_cx->Write(); gr_cy->Write(); 
fil.Close();


}
