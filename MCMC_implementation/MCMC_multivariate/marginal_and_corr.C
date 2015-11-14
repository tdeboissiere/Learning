

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
#include "TH1F.h"
#include "TH2F.h"

using namespace std;

int main(){

TFile f_M("graph_MCMC_multivariate.root","read");

TGraph *gr_A = (TGraph*)f_M.Get("Ampl");
TGraph *gr_S = (TGraph*)f_M.Get("Sigma");
TGraph *gr_M = (TGraph*)f_M.Get("Mean");

//Start at npts (past burn-in)
//Take npts <- start  -> npts

//The following definition of npts and start allows us to compute the autocorrelation with the lowest index=1000 >600 = burn-in step
const int npts=40000;
const int lcor=300;
const double npts_double=40000;
const int start=1000+npts;
const int npts_mean_std=99000;
const int start_mean_std=1000;
const double start_mean_std_double=99000.;

double corr_A[lcor]; double corr_S[lcor]; double corr_M[lcor];
double tabl[lcor];
for (int k=0; k<lcor; k++) {corr_A[k]=0; corr_S[k]=0; corr_M[k]=0; tabl[k]=k;}

//Standardize samples to 0 mean and unit standard deviation
double mean_A,mean_S,mean_M; mean_A=0; mean_S=0; mean_M=0;
double std_A,std_S,std_M; std_A=0; std_S=0; std_M=0;

//Temporary stuff to store values of interest
double x_1,y_1,x_2,y_2,x_3,y_3;

//Marginal distributions histo
TH1F* hA = new TH1F("hA","hA",100,90,120);
TH1F* hS = new TH1F("hS","hS",100,0,6);
TH1F* hM = new TH1F("hM","hM",100,15,25);
hA->SetLineColor(kBlack);
hA->SetLineWidth(2);
hS->SetLineColor(kRed);
hS->SetLineWidth(2);
hM->SetLineColor(kBlue);
hM->SetLineWidth(2);

//Correlation histo
TH2F* hcAS = new TH2F("hcAS","hcAS",100,90,120,100,0,6);
TH2F* hcAM = new TH2F("hcAM","hcAM",100,90,120,100,15,25);
TH2F* hcSM = new TH2F("hcSM","hcSM",100,0,6,100,15,25);
hcAS->SetMarkerColor(kBlack);
hcAS->SetLineWidth(2);
hcAM->SetMarkerColor(kRed);
hcAM->SetLineWidth(2);
hcSM->SetMarkerColor(kBlue);
hcSM->SetLineWidth(2);

//Fill mean 
for (int i=start_mean_std; i<start_mean_std+npts_mean_std; i++) {
   gr_A->GetPoint(i,x_1,y_1); 
   mean_A+=(1/start_mean_std_double)*y_1;
   
   gr_S->GetPoint(i,x_2,y_2);
   mean_S+=(1/start_mean_std_double)*y_2;
   
   gr_M->GetPoint(i,x_3,y_3); 
   mean_M+=(1/start_mean_std_double)*y_3;
   
   hA->Fill(y_1);
   hS->Fill(y_2);
   hM->Fill(y_3);
   
   hcAS->Fill(y_1,y_2);
   hcAM->Fill(y_1,y_3);
   hcSM->Fill(y_2,y_3);
   }

//Fill std
for (int i=start_mean_std; i<start_mean_std+npts_mean_std; i++) {
   gr_A->GetPoint(i,x_1,y_1); 
   std_A+=(1/start_mean_std_double)*pow(y_1-mean_A,2);
   gr_S->GetPoint(i,x_1,y_1); 
   std_S+=(1/start_mean_std_double)*pow(y_1-mean_S,2);
   gr_M->GetPoint(i,x_1,y_1); 
   std_M+=(1/start_mean_std_double)*pow(y_1-mean_M,2);
   }

cout << mean_A<< "   " << mean_S<< "    " << mean_M << endl;
cout << sqrt(std_A)<< "   " << sqrt(std_S)<< "    " << sqrt(std_M) << endl;


//Compute Correlation length brute force
for (int k=0; k<lcor; k++) {
   for (int i=start; i<start+npts; i++) {
   gr_A->GetPoint(i,x_1,y_1); gr_A->GetPoint(i-k,x_2,y_2);
   //cout << y_1<< "   " << y_2 << endl;
   corr_A[k]=corr_A[k]+(y_1-mean_A)*(y_2-mean_A)/std_A;
   
   gr_S->GetPoint(i,x_1,y_1); gr_S->GetPoint(i-k,x_2,y_2);
   corr_S[k]=corr_S[k]+(y_1-mean_S)*(y_2-mean_S)/std_S;
   
   gr_M->GetPoint(i,x_1,y_1); gr_M->GetPoint(i-k,x_2,y_2);
   corr_M[k]=corr_M[k]+(y_1-mean_M)*(y_2-mean_M)/std_M;
   }
   corr_A[k]=(1/npts_double)*corr_A[k];
   corr_S[k]=(1/npts_double)*corr_S[k];
   corr_M[k]=(1/npts_double)*corr_M[k];
}


//Correlation length graphs
TGraph *gr_cA = new TGraph (lcor, tabl, corr_A);
gr_cA->SetLineColor(kBlack);
gr_cA->SetLineWidth(2);
gr_cA->SetName("corr_A");

TGraph *gr_cS = new TGraph (lcor, tabl, corr_S);
gr_cS->SetLineColor(kRed);
gr_cS->SetLineWidth(2);
gr_cS->SetName("corr_S");

TGraph *gr_cM = new TGraph (lcor, tabl, corr_M);
gr_cM->SetLineColor(kBlue);
gr_cM->SetLineWidth(2);
gr_cM->SetName("corr_M");

TFile fil("marginal_and_corr_MCMC_multivariate.root","recreate");
gr_cA->Write(); gr_cS->Write(); gr_cM->Write();
hA->Write(); hS->Write(); hM->Write();
hcAS->Write(); hcAM->Write(); hcSM->Write();
fil.Close();

}

