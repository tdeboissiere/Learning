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
//#include </home/irfulx204/mnt/tmain/Desktop/My_MCMC/BigEigen/Eigen/Dense>
#include <Eigen/Dense>

using namespace std;



//This is the true density we want to sample
//A simple gaussian, with 3 param: Amp, Mu, S and one observable: E
double density(double *x, double * par){

double Amp=par[0]; double S=par[1]; double Mu=par[2];
double E=x[0];
double pi=TMath::Pi();
return exp(-0.5*pow((E-Mu)/S,2))*Amp/(sqrt(2*pi)*S);

}

double test_like(double * x, double* par) {
vector<double> data;
vector<double> fdata;
double temp_data,temp_fdata;
ifstream file("data.txt");
while (!file.eof()) {
file >> temp_data >> temp_fdata;
data.push_back(temp_data); fdata.push_back(temp_fdata);
}
data.pop_back(); fdata.pop_back();

TF1* f1=new TF1("density",density,0,40,3);
f1->SetParameter(0,14);   f1->SetParameter(1,1);    f1->SetParameter(2,x[0]);
double prod=0;
for (int k=0; k<data.size(); k++) {prod=prod+log(f1->Eval(data[k]));}//+data.size()*log(x[0])-x[0];}
return -prod;
}

int main() {

TRandom3 u;
u.SetSeed(0);

vector <double> A_vec;
vector <double> s_vec;
vector <double> mu_vec;


//Starting values for the param
//----------------------------
double start_A, start_s, start_mu;
start_A=60;
start_s=6;
start_mu=30;
A_vec.push_back(start_A); s_vec.push_back(start_s); mu_vec.push_back(start_mu); 

// Read the data		data are the observables, fdata the density evaluated at data
//-------------
vector<double> data;
vector<double> fdata;
double temp_data,temp_fdata;
ifstream file("data.txt");
while (!file.eof()) {
file >> temp_data >> temp_fdata;
data.push_back(temp_data); fdata.push_back(temp_fdata);
}
data.pop_back(); fdata.pop_back();

//Function for likelihood
//---------------------
TF1* f1 = new TF1("density",density,0,40,3);



const int nMCmax=100000;
//MCMC running
//-----------
for (int nMC=0; nMC<nMCmax; nMC++){
        double width_A=1; double width_S=0.15; double width_mu=0.21;
	A_vec.push_back(A_vec[nMC]+width_A*u.Gaus(0,1));
	s_vec.push_back(s_vec[nMC]+width_S*u.Gaus(0,1));
	mu_vec.push_back(mu_vec[nMC]+width_mu*u.Gaus(0,1));

	double like_i=0; double like_f=0;
	for (int k=0; k<data.size(); k++){
	//initial likelihood
	//------------------ 
	f1->SetParameter(0,A_vec[nMC]);   f1->SetParameter(1,s_vec[nMC]);    f1->SetParameter(2,mu_vec[nMC]);
	like_i=like_i+log(f1->Eval(data[k]));
	like_i=like_i+data.size()*log(A_vec[nMC])-A_vec[nMC];
	//cout << k << "    " << like_i<< "    " <<data[k] << endl;
	//final likelihood
	//------------------ 
	f1->SetParameter(0,A_vec[nMC+1]);   f1->SetParameter(1,s_vec[nMC+1]);    f1->SetParameter(2,mu_vec[nMC+1]);
	like_f=like_f+log(f1->Eval(data[k]));
	like_f=like_f+data.size()*log(A_vec[nMC+1])-A_vec[nMC+1];
	}
	//cout << like_i<<"   "<<like_f<<"   " <<A_vec[nMC]<<"   " << A_vec[nMC+1]<<"   " <<s_vec[nMC]<<"   " << s_vec[nMC+1]<<"   " <<mu_vec[nMC]<<"   " << mu_vec[nMC+1]<< endl;
	//Acceptance probability
	//----------------------
	double prob_ratio=exp(like_f-like_i);
	double accept=TMath::Min(1.,prob_ratio);
	if (u.Binomial(1,accept)==0) {
	A_vec.pop_back(); s_vec.pop_back(); mu_vec.pop_back(); 
	A_vec.push_back(A_vec[nMC]); s_vec.push_back(s_vec[nMC]); mu_vec.push_back(mu_vec[nMC]);
	}
	//cout <<accept << "    " << prob_ratio<<"    " << like_i<<"   "<<like_f<< "   " <<A_vec[nMC]<<"   " << A_vec[nMC+1]<<"   " <<s_vec[nMC]<<"   " << s_vec[nMC+1]<<"   " <<mu_vec[nMC]<<"   " << mu_vec[nMC+1]<< endl;
	//cout << like_i<<"   "<<like_f<<"   " <<A_vec[nMC]<<"   " << A_vec[nMC+1]<<"   " <<s_vec[nMC]<<"   " << s_vec[nMC+1]<<"   " <<mu_vec[nMC]<<"   " << mu_vec[nMC+1]<< endl;
}
//End of MCMC running
//-----------

double A_tabl[nMCmax+1]; double s_tabl[nMCmax+1]; double mu_tabl[nMCmax+1];
double nMC_tabl[nMCmax+1]; 
for (int k=0; k<=nMCmax; k++) {
   nMC_tabl[k]=k;
   A_tabl[k]=A_vec[k]; s_tabl[k]=s_vec[k]; mu_tabl[k]=mu_vec[k];
}

TGraph *gr_A = new TGraph (nMCmax, nMC_tabl, A_tabl);
gr_A->SetLineColor(kBlack);
gr_A->SetLineWidth(2);
gr_A->SetName("Ampl");

TGraph *gr_s = new TGraph (nMCmax, nMC_tabl, s_tabl);
gr_s->SetLineColor(kRed);
gr_s->SetLineWidth(2);
gr_s->SetName("Sigma");

TGraph *gr_mu = new TGraph (nMCmax, nMC_tabl, mu_tabl);
gr_mu->SetLineColor(kBlue);
gr_mu->SetLineWidth(2);
gr_mu->SetName("Mean");


TFile f("graph_MCMC.root","recreate");
gr_A->Write(); gr_s->Write(); gr_mu->Write();
f.Close();

return 0;
}

