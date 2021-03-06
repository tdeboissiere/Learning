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
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

//Run several chain in parallel to get an estimate of the covariance matrix
//Update the proposal function on the fly

//This is the true density we want to sample
//A simple gaussian, with 3 param: Amp, Mu, S and one observable: E
double density(double *x, double * par){

double Amp=par[0]; double S=par[1]; double Mu=par[2];
double E=x[0];
double pi=TMath::Pi();
return exp(-0.5*pow((E-Mu)/S,2))*Amp/(sqrt(2*pi)*S);

}



int main() {

TRandom3 u;
u.SetSeed(0);

const int nMCmax=100000;
const int nMCchain=10;
const double nMCchain_d=10.;

vector< vector<double> > A_vec(nMCmax);		
for(int i=0; i<nMCmax; ++i){A_vec[i].resize(nMCchain);}
vector< vector<double> > s_vec(nMCmax);		
for(int i=0; i<nMCmax; ++i){s_vec[i].resize(nMCchain);}
vector< vector<double> > mu_vec(nMCmax);		
for(int i=0; i<nMCmax; ++i){mu_vec[i].resize(nMCchain);}


//Starting values for the param
//----------------------------
double start_A, start_s, start_mu;
//start_A=60;start_s=6;start_mu=30;
start_A=100;start_s=5;start_mu=2;

//Mean over the chain of A, s and mu
vector <double> A_mean(nMCmax);
vector <double> s_mean(nMCmax);
vector <double> mu_mean(nMCmax);
A_mean[0]=start_A; s_mean[0]=start_s; mu_mean[0]=start_mu;



for (int i=0;i<nMCchain; i++) {
A_vec[0][i]=start_A; s_vec[0][i]=start_s; mu_vec[0][i]=start_mu;
}


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
file.close();


//Function for likelihood
//---------------------
TF1* f1 = new TF1("density",density,0,40,3);



//MCMC running
//-----------

//1st step with the standard gaussian proposal
double width_A=1; double width_s=0.15; double width_mu=0.21;
//double width_A=10; double width_s=0.0015; double width_mu=0.0021;		//Various checks performed: insensitive to variations in initial param
//double width_A=4; double width_s=1; double width_mu=1;
for (int i=0; i<nMCchain; i++) {
A_vec[1][i]=A_vec[0][i]+width_A*u.Gaus(0,1);
s_vec[1][i]=s_vec[0][i]+width_s*u.Gaus(0,1);
mu_vec[1][i]=mu_vec[0][i]+width_mu*u.Gaus(0,1);


A_mean[1]+=(1/nMCchain_d)*A_vec[1][i]; 
s_mean[1]+=(1/nMCchain_d)*s_vec[1][i];
mu_mean[1]+=(1/nMCchain_d)*mu_vec[1][i];
} 



//2nd step with multivariate gausian
for (int nMC=1; nMC<nMCmax-1; nMC++){
        // Build covariance matrix , diagonalize , get the coordinate change matrix and update
        Matrix3d Cov;
	Cov=Matrix3d::Zero();
        for (int i=0; i<nMCchain; i++) {
        VectorXd v(3);
        v(0)=A_vec[nMC][i]-A_mean[nMC]; v(1)=s_vec[nMC][i]-s_mean[nMC];v(2)=mu_vec[nMC][i]-mu_mean[nMC];
        Cov=Cov+(1/(nMCchain_d-1))*v*v.transpose();
        				//Cov=Matrix3d::Zero();				Test if the former gaussian update works: it does
        				//Cov(0,0)=1; Cov(1,1)=.15; Cov(2,2)=0.21;	Test if the former gaussian update works: it does
        //cout << nMC<< "   " << i << "    " << A_vec[nMC][i] << "   "  << A_mean[nMC] <<"   " << sqrt(Cov(0,0))<< endl;
        }
        //cout << Cov <<"-----"<< endl;
        //Diagonalize the matrix, stock the eigenvalues to diagonal matrix D, V is the coo change matrix such that: Cov=V*D*V.transpose
        SelfAdjointEigenSolver<Matrix3d> es(Cov);
	if (es.info() != Success) abort();
        Matrix3d D = es.eigenvalues().asDiagonal();
	Matrix3d V = es.eigenvectors();
	
        D(0,0)=sqrt(D(0,0)); D(1,1)=sqrt(D(1,1)); D(2,2)=sqrt(D(2,2));
        //D(0,0)=sqrt(D(2,2)); D(1,1)=sqrt(D(0,0)); D(2,2)=sqrt(D(1,1));
        //D=0.5*D;
        /*D(0,0)=sqrt(Cov(0,0)); D(1,1)=sqrt(Cov(1,1)); D(2,2)=sqrt(Cov(2,2));
        cout << "D   " << endl;
        cout << D*V.transpose() << endl;
        cout << "    " << endl;*/
        
        //Now update the param: theta_new= theta+ V*D*x
        for (int i=0; i<nMCchain; i++) {
        VectorXd Gaus_RandX(3);
        Gaus_RandX(0)=u.Gaus(0,1); Gaus_RandX(1)=u.Gaus(0,1); Gaus_RandX(2)=u.Gaus(0,1);
        VectorXd Par_update(3);
        Par_update=V*D*Gaus_RandX;
        //Par_update=D*Gaus_RandX;
        				//Par_update=Cov*Gaus_RandX;			Test if the former gaussian update works: it does
        //cout << Par_update <<"-----"<< endl;
        A_vec[nMC+1][i]=A_vec[nMC][i]+Par_update(0);
        s_vec[nMC+1][i]=s_vec[nMC][i]+Par_update(1);
        mu_vec[nMC+1][i]=mu_vec[nMC][i]+Par_update(2);
        //cout <<nMC<<"  "<< A_vec[nMC+1][i] << endl;
        }
        
double accept,like_fprint,like_iprint,prob_print;
        for (int i=0; i<nMCchain; i++) {
		double like_i=0; double like_f=0;
		for (int k=0; k<data.size(); k++){
		//initial likelihood
		//------------------ 
		f1->SetParameter(0,A_vec[nMC][i]);   f1->SetParameter(1,s_vec[nMC][i]);    f1->SetParameter(2,mu_vec[nMC][i]);
		like_i=like_i+log(f1->Eval(data[k]));
		like_i=like_i+data.size()*log(A_vec[nMC][i])-A_vec[nMC][i];
		//final likelihood
		//------------------ 
		f1->SetParameter(0,A_vec[nMC+1][i]);   f1->SetParameter(1,s_vec[nMC+1][i]);    f1->SetParameter(2,mu_vec[nMC+1][i]);
		like_f=like_f+log(f1->Eval(data[k]));
		like_f=like_f+data.size()*log(A_vec[nMC+1][i])-A_vec[nMC+1][i];
		}
		//Acceptance probability
		//----------------------
		if (A_vec[nMC+1][i]<=0 || s_vec[nMC+1][i]<=0 || mu_vec[nMC+1][i]<=0) {
		A_vec[nMC+1][i]=A_vec[nMC][i]; s_vec[nMC+1][i]=s_vec[nMC][i]; mu_vec[nMC+1][i]=mu_vec[nMC][i];
		}
		else if (A_vec[nMC+1][i]>0 && s_vec[nMC+1][i]>0 && mu_vec[nMC+1][i]>0) {
			double prob_ratio=exp(like_f-like_i);
			accept=TMath::Min(1.,prob_ratio);
			/*double prob_ratio=(like_f-like_i);
			accept=TMath::Min(0.,prob_ratio);
			if (accept>=0) {accept=1;}
			else if (accept<1) {accept=exp(accept);}*/
			//cout << nMC<< "   " << i << "    " <<accept<<"   " <<A_vec[nMC+1][i]<<"   "<< A_vec[nMC][i] <<"   "<< s_vec[nMC+1][i] <<"   "<< s_vec[nMC][i] <<"   "<< mu_vec[nMC+1][i] <<"   "<< mu_vec[nMC][i]<<"   "<< like_f<<"    "<< like_i<<endl;
			if (u.Binomial(1,accept)==0) {
			A_vec[nMC+1][i]=A_vec[nMC][i]; s_vec[nMC+1][i]=s_vec[nMC][i]; mu_vec[nMC+1][i]=mu_vec[nMC][i];
			}
		}
		A_mean[nMC+1]+=(1/nMCchain_d)*A_vec[nMC+1][i];
		s_mean[nMC+1]+=(1/nMCchain_d)*s_vec[nMC+1][i];
		mu_mean[nMC+1]+=(1/nMCchain_d)*mu_vec[nMC+1][i];
	}
	//cout <<A_vec[nMC+1][0]<<"   "<< A_vec[nMC][0] <<"   "<<accept<<"   "<<like_fprint<<"    "<<like_iprint<<"    "<<prob_print<< endl;
	//cout <<A_vec[nMC+1][0]<<"   "<< A_vec[nMC][0] <<"   "<< s_vec[nMC+1][0] <<"   "<< s_vec[nMC][0] <<"   "<< mu_vec[nMC+1][0] <<"   "<< mu_vec[nMC][0]<<"   "<<accept<<"   "<<like_fprint<<"    "<<like_iprint<<"    "<<prob_print<< endl;
	//cout <<nMC<<"  "<< A_mean[nMC] << endl;
	//cout << "  " << endl;
}
//End of MCMC running
//-----------

vector <double> A_vtemp; vector <double> s_vtemp; vector <double> mu_vtemp;

 
for (int k=0; k<nMCmax; k++) {
   for (int i=0; i<1; i++) {
   A_vtemp.push_back(A_vec[k][i]); s_vtemp.push_back(s_vec[k][i]); mu_vtemp.push_back(mu_vec[k][i]);
   //cout << A_tabl[k] << endl;
   }
   
}
const int sizevec= A_vtemp.size();
double A_tabl[sizevec]; double s_tabl[sizevec]; double mu_tabl[sizevec]; double nMC_tabl[sizevec];
for (int k=0; k<sizevec; k++) {
nMC_tabl[k]=k;
A_tabl[k]=A_vtemp[k]; s_tabl[k]=s_vtemp[k]; mu_tabl[k]=mu_vtemp[k];
}

TGraph *gr_A = new TGraph (sizevec, nMC_tabl, A_tabl);
gr_A->SetLineColor(kBlack);
gr_A->SetLineWidth(2);
gr_A->SetName("Ampl");

TGraph *gr_s = new TGraph (sizevec, nMC_tabl, s_tabl);
gr_s->SetLineColor(kRed);
gr_s->SetLineWidth(2);
gr_s->SetName("Sigma");

TGraph *gr_mu = new TGraph (sizevec, nMC_tabl, mu_tabl);
gr_mu->SetLineColor(kBlue);
gr_mu->SetLineWidth(2);
gr_mu->SetName("Mean");


TFile f("graph_MCMC_multivariate_for_comparison.root","recreate");
gr_A->Write(); gr_s->Write(); gr_mu->Write();
f.Close();

return 0;
}

