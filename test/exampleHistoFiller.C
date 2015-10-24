
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TFile.h"

#if !defined(__CINT__) && !defined(MAKECINT)
#include "AnalysisTools/QuickRefold/interface/TObjectContainer.h"


using namespace std;



class test {
public:
  enum axes {FLAVOR, ETA};
  test() {
    //setup the object for filling
    QuickRefold::TH1FContainer * a = new QuickRefold::TH1FContainer ("eff",2);
    a->addAxis(FLAVOR,"flavor",3,-.5,2.5);
    a->addAxis(ETA,"eta",2,0,2.5);
    a->stopSetup();

    //whatever input you want...for this example just some random histos
    for(unsigned int iF = 0; iF < 3; ++iF)
      for(unsigned int iE = 0; iE < 2; ++iE){
        TH1F * h = new TH1F(TString::Format("eff_%u_%u",iF,iE),";p_{T}",10,0,200);
        for(unsigned int iB = 1; iB <= 10; ++iB ){
          h->SetBinContent(iB,gRandom->Uniform (0.0, 1.0));
        }

        //fill the container with your histogram
        a->setBin(FLAVOR,iF);
        a->setBin(ETA,iE);
        a->setValue(*h);
      }

    //write it out
    TFile * outFile = new TFile("testOutput.root","recreate");
    a->Write();
    outFile->Close();
    delete outFile;
  }

  void getOutput(){
    TFile * inFile = new TFile("testOutput.root","read");
    QuickRefold::TH1FContainer * a = 0;
    inFile->GetObject("eff",a);

    //get flavor bin 2, etat bin 1
    a->setBin(FLAVOR,2);
    a->setBin(ETA,1);

    const TH1F& effHist = a->getValue();
    cout << effHist.GetBinContent(effHist.FindFixBin(190)) << endl;

    //drawing requires a copy...it is a non-const function
    TH1 * copyHist = (TH1*)effHist.Clone();
    copyHist->Draw();
  }
};



#endif //!CINT

void exampleHistoFiller(){
  //test creating a contianer
  test a;
  a.getOutput();
  //test getting the output
}
