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

//Left over from the code MCMC.C  No tocar!
double density2(double *x, double * par){
double Amp=par[0]; double S=par[1]; double Mu=par[2];
double E=x[0];
return exp(-0.5*pow((E-Mu)/S,2))*Amp;
}

//True pdf 
double density(double *var, double * par){
double x=var[0]; double y=var[1]; 

return exp(-0.5*pow(x,2))*exp(-0.5*pow((y-30)/3,2));
}

int main() {

TRandom3 u;
u.SetSeed(0);

vector <double> x_vec;
vector <double> y_vec;


//Starting values for the param
//----------------------------
double start_x, start_y;
start_x=10;
start_y=10;
x_vec.push_back(start_x); y_vec.push_back(start_y); 



//Function for likelihood
//---------------------
TF2* f2 = new TF2("density",density,-200,200,200,200,0);



const int nMCmax=100000;
//MCMC running
//-----------
for (int nMC=0; nMC<nMCmax; nMC++){
        double width_x=2; double width_y=6;
	x_vec.push_back(x_vec[nMC]+width_x*u.Gaus(0,1));
	y_vec.push_back(y_vec[nMC]+width_y*u.Gaus(0,1));
	
	//initial likelihood
	//------------------ 
	double p_ini=f2->Eval(x_vec[nMC],y_vec[nMC]);
	//final likelihood
	//------------------ 
	double p_fin=f2->Eval(x_vec[nMC+1],y_vec[nMC+1]);
	
	
	//Acceptance probability
	//----------------------
	double prob_ratio=p_fin/p_ini;
	double accept=TMath::Min(1.,prob_ratio);
	if (u.Binomial(1,accept)==0) {
	x_vec.pop_back(); y_vec.pop_back(); 
	x_vec.push_back(x_vec[nMC]); y_vec.push_back(y_vec[nMC]); 
	}
	//cout <<accept << "    " << prob_ratio<<"    " << like_i<<"   "<<like_f<< "   " <<A_vec[0]<<"   " << A_vec[1]<< endl;
	//cout << like_i<<"   "<<like_f<<"   " <<A_vec[nMC]<<"   " << A_vec[nMC+1]<<"   " <<s_vec[nMC]<<"   " << s_vec[nMC+1]<<"   " <<mu_vec[nMC]<<"   " << mu_vec[nMC+1]<< endl;
}
//End of MCMC running
//-----------

double x_tabl[nMCmax+1]; double y_tabl[nMCmax+1]; 
double nMC_tabl[nMCmax+1]; 
for (int k=0; k<=nMCmax; k++) {
   nMC_tabl[k]=k;
   x_tabl[k]=x_vec[k]; y_tabl[k]=y_vec[k]; 
}

TGraph *gr_x = new TGraph (nMCmax, nMC_tabl, x_tabl);
gr_x->SetLineColor(kBlack);
gr_x->SetLineWidth(2);
gr_x->SetName("X");

TGraph *gr_y = new TGraph (nMCmax, nMC_tabl, y_tabl);
gr_y->SetLineColor(kRed);
gr_y->SetLineWidth(2);
gr_y->SetName("Y");


TFile f("graph_MCMC_gaus.root","recreate");
gr_x->Write(); gr_y->Write(); 
f.Close();

return 0;
}
