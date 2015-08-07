/*
 * MakeRefold.cc
 *
 *  Created on: Jul 31, 2015
 *      Author: nmccoll
 */
#include <stdexcept>

#include "TLegend.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TFile.h"

#include "../interface/MakeRefold.h"

namespace QuickRefold {

MakeRefold::MakeRefold(unsigned int nTrueAxes,unsigned int nMeasAxes, BackgroundTreatment treatment, unsigned int nBKGTypes) :
    bkgTreat(treatment),
    tempTrueRefold(new Refold("tempTrueRefold", bkgTreat > SUBTRACT ? nTrueAxes + 1 : nTrueAxes )),
    tempMeasRefold(new Refold("tempMeasRefold", nMeasAxes )),
    input_truth(0),
    input_meas(0),
    input_resp  (0),
    meas  (0),
    bkg   (0),
    catAxis(nTrueAxes),
    nBkgs(nBKGTypes),
    unfoldCache(0),
    respCache(0),
    unfoldTruth(0),
    unfoldReco(0),
    covMatrix(0)
{
  if(bkgTreat  > SUBTRACT){
    tempTrueRefold->addAxis(catAxis, "temp_bkg_type", nBKGTypes + 1, -.5, float(nBKGTypes + 1)-.5);
  }
}

MakeRefold::~MakeRefold() {
  delete tempTrueRefold;
  delete tempMeasRefold;
  delete input_truth;
  delete input_meas;
  delete input_resp;
  delete meas;
  delete bkg;
  delete unfoldCache;
  delete respCache;
  delete unfoldTruth;
  delete unfoldReco;
  delete covMatrix;
}

void MakeRefold::addTruthAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis) {
  tempTrueRefold->addAxis(axis, name, nBins, minAxis, maxAxis);
}
void MakeRefold::addTruthAxis(unsigned int axis, const char *name, unsigned int nBins, float* axisValues) {
  tempTrueRefold->addAxis( axis, name, nBins, axisValues);
}
void MakeRefold::addMeasAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis) {
  tempMeasRefold->addAxis( axis, name, nBins, minAxis, maxAxis);
}
void MakeRefold::addMeasAxis(unsigned int axis, const char *name, unsigned int nBins, float* axisValues) {
  tempMeasRefold->addAxis(axis, name, nBins, axisValues);
}

void MakeRefold::stopSetup(){
  tempTrueRefold->stopSetup(true);
  tempMeasRefold->stopSetup(true);
  const unsigned int nTrueBins = tempTrueRefold->getNumberOfBins();
  const unsigned int nMeasBins = tempMeasRefold->getNumberOfBins();
  input_truth = new TH1F ("input_truth","",nTrueBins, -0.5, float(nTrueBins) -.5);
  input_meas = new TH1F ("input_meas","",nMeasBins, -0.5, float(nMeasBins) -.5);
  input_resp = new TH2F ("input_resp","",nMeasBins, -0.5, float(nMeasBins) -.5,nTrueBins, -0.5, float(nTrueBins) -.5 );
  meas = new TH1F ("tempMeas","",nMeasBins, -0.5, float(nMeasBins) -.5);

  input_truth ->Sumw2();
  input_meas  ->Sumw2();
  input_resp  ->Sumw2();
  meas        ->Sumw2();

  if(bkgTreat  == SUBTRACT){
    bkg= new TH1F ("bkgMeas","",nMeasBins, -0.5, float(nMeasBins) -.5);
    bkg->Sumw2();
  }

}

void MakeRefold::setTruthBin    (const unsigned int& axis, const float& input) const
{
  tempTrueRefold->setBin(axis,input);
}
void MakeRefold::setMeasBin     (const unsigned int& axis, const float& input) const
{
  tempMeasRefold->setBin(axis,input);
}
void MakeRefold::fillInput      (float weight)
{
  if(bkgTreat > SUBTRACT){
    tempTrueRefold->setBinIntFast(catAxis,1);
  }
  const unsigned int trueBin = tempTrueRefold->getBin() +1;
  const unsigned int measBin = tempMeasRefold->getBin() +1;
  fill(input_truth,trueBin,weight);
  fill(input_meas,measBin,weight);
  fill(input_resp,measBin,trueBin,weight);
}
void MakeRefold::fillBKG        (float weight, unsigned int bkgCategory)
{
  if(bkgTreat > SUBTRACT){
    if(bkgCategory +1 > nBkgs) throw std::invalid_argument(TString::Format("MakeRefold::fillBKG: Want bkg %u, obly have %u",bkgCategory,nBkgs).Data() );
    tempTrueRefold->setBinIntFast(catAxis,bkgCategory+2);
    if(bkgTreat == NORM){
      for(unsigned int iA = 0; iA < tempTrueRefold->getNumberOfAxes() -1; ++iA){
        tempTrueRefold->setBinIntFast(iA,0);
      }
    }
    const unsigned int trueBin = tempTrueRefold->getBin()+1;
    const unsigned int measBin = tempMeasRefold->getBin()+1;
    fill(input_truth,trueBin,weight);
    fill(input_meas,measBin,weight);
    fill(input_resp,measBin,trueBin,weight);
  } else if(bkgTreat ==SUBTRACT){
    const unsigned int measBin = tempMeasRefold->getBin()+1;
    fill(bkg,measBin,weight);
  }
}
void MakeRefold::fillMeasurement(float weight)
{
  const unsigned int measBin = tempMeasRefold->getBin()+1;
  fill(meas,measBin,weight);
}
void MakeRefold::unfold(RooUnfold::Algorithm alg, double regparm, double minMeas, double  minResp) const
{
  if(unfoldCache){
    delete unfoldCache;
    delete respCache;
    delete unfoldTruth;
    delete unfoldReco;
    delete covMatrix;
  }

  TH1 * actMeas = 0;
  if(bkgTreat == SUBTRACT){
    actMeas = (TH1*)meas->Clone();
    actMeas->Add(bkg,-1);
  } else{
    actMeas = meas;
  }

  for(unsigned int iM = 0; iM < tempMeasRefold->getNumberOfBins(); ++iM){
    if(actMeas->GetBinContent(iM + 1) < minMeas)
      throw std::invalid_argument(TString::Format("MakeRefold::unfold: Meas. bin %s, only has %f but you require at least %f",
          tempMeasRefold->getBinTitle(iM).Data(),actMeas->GetBinContent(iM + 1), minMeas   ).Data() );
    for(unsigned int iT = 0; iT < tempTrueRefold->getNumberOfBins(); ++iT){
      const unsigned int rbin = input_resp->GetBin(iM +1 ,iT +1);
      if(input_resp->GetBinContent(rbin) < minResp)
        throw std::invalid_argument(TString::Format("MakeRefold::unfold: Resp: meas bin %s and true bin %s only has %f but you require at least %f",
            tempTrueRefold->getBinTitle(iT).Data(),tempMeasRefold->getBinTitle(iM).Data(),input_resp->GetBinContent(rbin), minResp   ).Data() );
    }
  }

  respCache = new RooUnfoldResponse(0,0,input_resp);
  unfoldCache = RooUnfold::New(alg,respCache,actMeas,regparm);
  unfoldCache->SetVerbose(5);
  unfoldTruth = unfoldCache->Hreco(RooUnfold::kCovariance);

  //Now get the undfolded reco;
  unfoldReco = (TH1*)meas->Clone();
  unfoldReco->Reset();

  const unsigned int nTrue = unfoldTruth->GetNbinsX();
  const unsigned int nReco = unfoldReco->GetNbinsX();
  float * totTrues = new float[nTrue];

  for(unsigned int iT = 1; iT <= nTrue; ++iT ){
    totTrues[iT - 1] = 0;
    for(unsigned int iR = 1; iR <= nReco; ++iR){
      const unsigned int bin = input_resp->GetBin(iR,iT);
      totTrues[iT -1] += input_resp->GetBinContent(bin);
    }
    if(totTrues[iT -1] == 0) continue;
    for(unsigned int iR = 1; iR <= nReco; ++iR){
      const unsigned int bin = input_resp->GetBin(iR,iT);
      unfoldReco->AddBinContent(iR,input_resp->GetBinContent(bin) * unfoldTruth->GetBinContent(iT) / totTrues[iT -1] );
    }
  }

  //Now for errors
  TArrayD& sumw2 = *unfoldReco->GetSumw2();
  covMatrix = new TMatrixD(unfoldCache->Ereco(RooUnfold::kCovariance));
  for(unsigned int iR = 1; iR <= nReco; ++iR){
    sumw2[iR] = 0;
    for(unsigned int iT1 = 1; iT1 <= nTrue; ++iT1 ){
      for(unsigned int iT2 = iT1; iT2 <= nTrue; ++iT2 ){
        if(totTrues[iT1 -1] == 0) continue;
        if(totTrues[iT2 -1] == 0) continue;
        const unsigned int bin1 = input_resp->GetBin(iR,iT1);
        const unsigned int bin2 = input_resp->GetBin(iR,iT2);
        sumw2[iR] += (input_resp->GetBinContent(bin1) / totTrues[iT1 -1])
            *(input_resp->GetBinContent(bin2) / totTrues[iT2 -1])
            *(*covMatrix)(iT1 -1, iT2 -1);
      }
    }
  }

  delete [] totTrues;
  if(bkgTreat == SUBTRACT){
    delete actMeas;
  }
}
void MakeRefold::drawDiag() const {
  if(unfoldCache == 0)
    throw std::invalid_argument("MakeRefold::drawDiag : trying to draw, but you have not unfolded yet.");

  TLegend * legTruth = new TLegend(0.30,0.25,.55,0.45);
  legTruth->SetFillStyle(0);
  legTruth->SetBorderSize(0);
  THStack *hsTruth = new THStack();

  TH1 *inT = (TH1*)input_truth->Clone();
  inT->SetLineColor(kBlue);
  inT->SetLineWidth(3);
  legTruth->AddEntry(inT,"Input truth");
  hsTruth->Add(inT);

  TH1 *unT = (TH1*)unfoldTruth->Clone();
  unT->SetLineColor(kBlack);
  unT->SetLineWidth(3);
  legTruth->AddEntry(unT,"Unfold truth");
  hsTruth->Add(unT);

  TCanvas * cTruth = new TCanvas();
  hsTruth->Draw("nostack");
  legTruth->Draw();

  for(unsigned int iB = 1; int(iB) <= hsTruth->GetXaxis()->GetNbins(); ++iB){
    if((iB - 1) % 5 == 0)
    hsTruth->GetXaxis()->SetBinLabel(iB, tempTrueRefold->getBinTitle(iB -1));
  }
  cTruth->Update();
  cTruth->RedrawAxis();

  TH1 *unT2 = (TH1*)unfoldTruth->Clone();
  unT2->Divide(unT2,inT,1,1,"s");
  new TCanvas();
  unT2->Draw();

  TLegend * legMeas = new TLegend(0.30,0.25,.55,0.45);
  legMeas->SetFillStyle(0);
  legMeas->SetBorderSize(0);
  THStack *hsMeas = new THStack();

  TH1 *inM = (TH1*)input_meas->Clone();
  inM->SetLineColor(kBlue);
  inM->SetLineWidth(3);
  legMeas->AddEntry(inM,"Input meas");
  hsMeas->Add(inM);

  TH1 *oM = (TH1*)meas->Clone();
  oM->SetLineColor(kBlack);
  oM->SetLineWidth(3);
  legMeas->AddEntry(oM,"Meas");
  hsMeas->Add(oM);

  TH1 *uM = (TH1*)unfoldReco->Clone();
  uM->SetLineColor(kRed);
  uM->SetLineWidth(3);
  legMeas->AddEntry(uM,"Unfold meas");
  hsMeas->Add(uM);


  TCanvas * cMeas = new TCanvas();
  hsMeas->Draw("nostack");
  legMeas->Draw();

  for(unsigned int iB = 1; int(iB) <= hsMeas->GetXaxis()->GetNbins(); ++iB){
    if((iB - 1) % 5 == 0)
      hsMeas->GetXaxis()->SetBinLabel(iB, tempMeasRefold->getBinTitle(iB -1));
  }
  cMeas->Update();
  cMeas->RedrawAxis();


  TCanvas * cRes = new TCanvas();
  TH2 *resp = (TH2*)input_resp->Clone();
  resp->Draw("COLZ");
  resp->GetXaxis()->SetTitle("Measured");
  resp->GetYaxis()->SetTitle("Truth");
  cRes->Update();
  cRes->RedrawAxis();
}
void MakeRefold::writeDiag(TString fileName, TString option){
  TFile * outFile = new TFile(fileName,option);
  outFile->cd();
  input_truth->Write("input_truth");
  unfoldTruth->Write("unfold_truth");
  input_meas->Write("input_measure");
  unfoldReco->Write("unfold_measure");
  meas->Write("measure");
  input_resp->Write("input_response");
  outFile->Close();
  delete outFile;
}

void MakeRefold::writeCorr(TString fileName, TString openOpt, TString corrName, bool storeCov) {
  if(unfoldCache == 0)
    throw std::invalid_argument("MakeRefold::writeCorr : trying to write, but you have not unfolded yet.");


  //Reduce memory load;
  delete tempMeasRefold;
  delete input_meas;
  delete input_resp ;
  delete meas ;
  delete bkg  ;
  delete unfoldCache;
  delete respCache;
  delete unfoldReco;




  Refold * outputRefold = new Refold(corrName, ( bkgTreat > SUBTRACT ? tempTrueRefold->getNumberOfAxes() - 1 : tempTrueRefold->getNumberOfAxes()),storeCov);
  for(unsigned int iA = 0; iA < outputRefold->getNumberOfAxes(); ++iA){
    outputRefold->addAxis(iA,tempTrueRefold->getAxis(iA));
  }
  outputRefold->stopSetup();

  const unsigned int numOBi = outputRefold->getNumberOfBins();
  const unsigned int numOAx = outputRefold->getNumberOfAxes();
  std::vector<unsigned int> inputBins(tempTrueRefold->getNumberOfAxes(),1); //set to one so the default is w/o bkg
  for(unsigned int iB = 0; iB < numOBi; ++iB){
    outputRefold->fillBinCache(iB);
    for(unsigned int iA = 0; iA < numOAx; ++iA)
      tempTrueRefold->setBinIntFast(iA,outputRefold->binCache->at(iA));
    const unsigned int inputBin = tempTrueRefold->getBin();
    if(input_truth->GetBinContent(inputBin + 1) <= 0) continue;
    outputRefold->setValue(iB, unfoldTruth->GetBinContent(inputBin + 1) / input_truth->GetBinContent(inputBin + 1) );

    for(unsigned int iB2 = iB; iB2 < numOBi; ++iB2){
      outputRefold->fillBinCache(iB2);
      for(unsigned int iA2 = 0; iA2 < numOAx; ++iA2)
        tempTrueRefold->setBinIntFast(iA2,outputRefold->binCache->at(iA2));
      const unsigned int inputBin2 = tempTrueRefold->getBin();
      if(input_truth->GetBinContent(inputBin2 + 1) <= 0) continue;
      double covValue = (*covMatrix)(inputBin,inputBin2) / (input_truth->GetBinContent(inputBin + 1) * input_truth->GetBinContent(inputBin2 + 1));

      outputRefold->setError(iB,iB2, covValue );
      outputRefold->setError(iB2,iB, covValue );
    }
  }

  TFile * outFile = new TFile(fileName,openOpt);
  outputRefold->Write();
  outFile->Close();
  delete outputRefold;
  delete outFile;

  delete tempTrueRefold;
  delete input_truth;
  delete unfoldTruth;
  delete covMatrix;
}

} /* namespace QuickRefold */
