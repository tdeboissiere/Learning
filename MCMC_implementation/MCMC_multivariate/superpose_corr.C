{

//Plots the two methods' correlation length
//Used 10 chains for MCMC multivariate

using namespace std;

TFile fm("marginal_and_corr_MCMC_multivariate_for_comparison.root");
TGraph * grm= (TGraph*)fm.Get("corr_M");

grm->SetLineColor(kRed);


TFile f("/home/irfulx204/mnt/tmain/Desktop/My_MCMC/marginal_and_corr_MCMC.root");
TGraph * gr= (TGraph*)f.Get("corr_M");

gr->SetLineColor(kBlue);


grm->Draw("AC");
gr->Draw("Csame");

}
