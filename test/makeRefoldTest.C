#include <iostream>

#include "TRandom.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TFile.h"

#if !defined(__CINT__) && !defined(MAKECINT)
#include "AnalysisTools/QuickRefold/interface/MakeRefold.h"


using namespace std;

Double_t smear (Double_t xt)
{
//  Double_t xeff= 0.3 + (1.0-0.3)/20*(xt+10.0);  // efficiency
//  Double_t x= gRandom->Rndm();
//  if (x>xeff) return cutdummy;
  Double_t xsmear= gRandom->Gaus(-2.5,0.2);     // bias and smear
  return xt+xsmear;
}
int nJetTrue(bool two){
  double xt = gRandom->Uniform(1.);
  int nJ = 0;
  if(!two){
    if(xt < .1){
      nJ = 0;
    } else if(xt < .3){
      nJ = 1;
    } else if(xt < .6){
      nJ = 2;
    }else if(xt < .8){
      nJ = 3;
    } else {
      nJ = 4;
    }
  } else {
    if(xt < .2){
      nJ = 0;
    } else if(xt < .5){
      nJ = 1;
    } else if(xt < .7){
      nJ = 2;
    }else if(xt < .9){
      nJ = 3;
    } else {
      nJ = 4;
    }
  }
return nJ;
}

class test {
public:
  test() {

    QuickRefold::MakeRefold * a = new QuickRefold::MakeRefold (2,2);
    a->addTruthAxis(0,"true",10, -10.0, 10.0);
    a->addTruthAxis(1,"true2",5, -.5, 4.5);
    a->addMeasAxis(0,"meas",10, -10.0, 10.0);
    a->addMeasAxis(1,"meas2",5, -.5, 4.5);
    a->stopSetup();

    TH1 * dTest = new TH1D("dt","dt",10,-10,10);
    TH1 * oTest = new TH1D("ot","ot",10,-10,10);
    TH1 * uTest = new TH1D("ut","ut",10,-10,10);
    TH1 * uuTest = new TH1D("uut","uut",10,-10,10);
    TH1 * uITest = new TH1D("uit","uit",10,-10,10);
    TH1 * oITest = new TH1D("oit","oit",10,-10,10);
    dTest->Sumw2();
    oTest->Sumw2();
    uTest->Sumw2();
    uuTest->Sumw2();
    uITest->Sumw2();
    oITest->Sumw2();

    TFile * f = new TFile("outCorr.root","read");
    QuickRefold::Refold * oldA = 0;
    if(f) f->GetObject("testCorr",oldA);

    for (Int_t i=0; i<10000000; i++) {
      Double_t xt= gRandom->Gaus (0.0, 2.0), x= smear (xt);
      a->setMeasBin(0,x);
      a->setTruthBin(0,xt);
      int nJT = nJetTrue(true);
      int nJR = nJT + gRandom->Gaus (0.0, 0.5);
      a->setMeasBin(1,nJR);
      a->setTruthBin(1,nJT);
      a->fillInput(0.01);

//      if(nJR >= .5) continue;

      oTest->Fill(x,.01);
      oITest->Fill(xt,.01);

      if(oldA){
        oldA->setBin(0,xt);
        oldA->setBin(1,nJT);

        uTest->Fill( x, oldA->getValue()*.01);
        uuTest->Fill( x, (oldA->getValue() + oldA->getSqrtError())*.01);
        uITest->Fill(xt,oldA->getValue()*.01);
      }

    }

    for (Int_t i=0; i<100000; i++) {
      Double_t xt= gRandom->Gaus (1.0, 2.0);
      Double_t x= smear (xt);
      a->setMeasBin(0,x);
      int nJT = nJetTrue(true);
      int nJR = nJT + gRandom->Gaus (0.0, 0.5);
      a->setMeasBin(1,nJR);
      a->fillMeasurement();
//      if(nJR >= .5) continue;;
      dTest->Fill(x);
    }

    if(f) f->Close();

    a->unfold(RooUnfold::kBayes,4,0,0);
    a->drawDiag();
    a->writeDiag("outDiag.root","recreate");
    a->writeCorr("outCorr.root","recreate","testCorr",true);

    if(oldA){
      TLegend * legMeas = new TLegend(0.30,0.25,.55,0.45);
      legMeas->SetFillStyle(0);
      legMeas->SetBorderSize(0);
      THStack *hsMeas = new THStack();

      TH1 *inM = (TH1*)oTest->Clone();
      inM->SetLineColor(kBlue);
      inM->SetLineWidth(3);
      legMeas->AddEntry(inM,"Input meas");
      hsMeas->Add(inM);

      TH1 *oM = (TH1*)dTest->Clone();
      oM->SetLineColor(kBlack);
      oM->SetLineWidth(3);
      legMeas->AddEntry(oM,"Meas");
      hsMeas->Add(oM);

      TH1 *uM = (TH1*)uTest->Clone();
      uM->SetLineColor(kRed);
      uM->SetLineWidth(3);
      legMeas->AddEntry(uM,"Unfold meas");
      hsMeas->Add(uM);

      TH1 *uuM = (TH1*)uuTest->Clone();
      uuM->SetLineColor(kGray);
      uuM->SetLineWidth(3);
      legMeas->AddEntry(uuM,"Unfold up meas");
      hsMeas->Add(uuM);




      TCanvas * cMeas = new TCanvas();
      hsMeas->Draw("nostack");
      legMeas->Draw();


      uITest->Divide(oITest);
      new TCanvas();
      uITest->Draw();

    }



  }
};
#endif //!CINT

  void makeRefoldTest(){
    test a;
  }

/* script to test the output
 *
  {
    TFile * f = new TFile("outCorr.root","read");
    QuickRefold::Refold * a;
    f->GetObject("testCorr",a);


    TH1 * h = a.draw("test","test");
    h->Draw();

    new TCanvas();
    TH2 * h2 = a.draw2D("test2","test2");
    h2->Draw("COLZ");

    new TCanvas();
    TH2 * h3 = a.drawCov("cov","cov");
    h3->Draw("COLZ");

    a.printValues();
        // a.printCovariance();


    // f->Close();

}
 *
 */

