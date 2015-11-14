
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TRandom3.h"
#include "TRandom2.h"
#include "TRandom.h"

using namespace std;

//This is the proposal density we want to sample
//A simple gaussian, with 3 param: Amp, Mu, S and one observable: E
double density(double *x, double * par){

double Amp=par[0]; double S=par[1]; double Mu=par[2];
double E=x[0];
double pi=TMath::Pi();
return exp(-0.5*pow((E-Mu)/S,2))*Amp/(sqrt(2*pi)*S);

}

int main() {
//Fill the vector "data", with samples from the proposal density
//The proposal density has {Amp,S,Mu}={1,2,5}

TF1* f1 = new TF1("density",density,0,40,3);
f1->SetParameter(0,100);
f1->SetParameter(1,2);
f1->SetParameter(2,20);
TRandom3 u;
u.SetSeed(0);
const int Nexp=u.Poisson(f1->Integral(0,40));
cout << Nexp << endl;
vector <double> data;
for (int k=0; k<Nexp; k++) {
data.push_back(f1->GetRandom(0,40));
}


//Write this to a file

ofstream file("data.txt");
for (int k=0; k<data.size(); k++) {
file << data[k]<< "\t\t" << f1->Eval(data[k]) << endl;
}
file.close();

}

int main2() {
TH1F* h1 = new TH1F("h","h",100,0,40);
double truc, val;
ifstream file("data.txt");
while (!file.eof()) {
file >> val >> truc;
h1->Fill(val);
}

h1->Draw();

}
