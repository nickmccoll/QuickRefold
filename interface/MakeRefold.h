/*
 * MakeRefold.h
 *
 *  Created on: Jul 31, 2015
 *      Author: nmccoll
 */

#ifndef QUICKREFOLD_INTERFACE_MAKEREFOLD_H_
#define QUICKREFOLD_INTERFACE_MAKEREFOLD_H_
#include "TH2F.h"
#include "AnalysisTools/RooUnfold/src/RooUnfold.h"

#include "Refold.h"



namespace QuickRefold {

//How to deal with background
// IGNORE = just ignore all backgrounds, filling it wont do anything
// Subtract = Subtract off of data
// NORM = allow the normalization of each background type to be adjusted
// FREE = allow the background counts in each bin and type to unfolded fully

enum BackgroundTreatment { IGNORE, SUBTRACT, NORM, FREE};

class MakeRefold {
public:
  // nTrueAxes = How many truth parameterization axes you want to use when unfolding
  // nMeasAxes = How many measured parameterization axes you want to use when unfolding
  // treatment = How to treat background
  // nBKGTypes = How many background types you want to individually keep track of
  MakeRefold(unsigned int nTrueAxes,unsigned int nMeasAxes, BackgroundTreatment treatment = IGNORE, unsigned int nBKGTypes = 1);
  virtual ~MakeRefold();

  //Setup functions
  //Add a new truth axis with fixed width bins
  void addTruthAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis);
  //Add a new truth axis with variable width bins
  void addTruthAxis(unsigned int axis, const char *name, unsigned int nBins, float* axisValues);

  //Repeated for measured axes
  void addMeasAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis);
  void addMeasAxis(unsigned int axis, const char *name, unsigned int nBins, float* axisValues);

  //Call when setup is complete
  void stopSetup();


  //Filling options, we have to set the the values in each axis
  void setTruthBin    (const unsigned int& axis, const float& input) const;
  void setMeasBin     (const unsigned int& axis, const float& input) const;
  void fillInput      (float weight = 1);                                   // fill truth (with corresponding measurement) event
  void fillBKG        (float weight = 1, unsigned int bkgCategory = 0);     // fill background event with bkgCategory 0 - (nBKGTypes -1)
  void fillMeasurement(float weight = 1);                                   //fill measured, what you want to unfold against


  //Processing
  //Do the unfolding, can be repeated multiple times
  //alg = What algorithm to unfold with
  //regparm = regularization param. for bayes it is the number of iterations
  //min yield per bin of the unfold to measurements
  //min yield per bin of the input response matrix
  void unfold(RooUnfold::Algorithm alg, double regparm= -1e30, double minMeas = 5, double  minResp = 5) const;
  //Draw before and after histograms
  void drawDiag() const;

  //Output
  //Write out the correction
  //Warning! this is destructive and you will not be able
  //to unfold again!
  //storeCov= store full cov matrix or just the diagonals
  void writeCorr(TString fileName, TString openOpt, TString corrName, bool storeCov);


private:

  void fill(TH1* hist, unsigned int bin, float weight){
    hist->AddBinContent(bin,weight);
    (*hist->GetSumw2())[bin] += weight*weight;
    hist   ->SetEntries    (hist->GetEntries() + 1);
  }
  void fill(TH2* hist,  unsigned int binx,unsigned int biny,float weight){
    const unsigned int bin = hist->GetBin(binx,biny);
    hist->AddBinContent(bin,weight);
    (*hist->GetSumw2())[bin] += weight*weight;
    hist   ->SetEntries    (hist->GetEntries() + 1);
  }

  const BackgroundTreatment bkgTreat;

private:
  Refold * tempTrueRefold;
  Refold * tempMeasRefold;
  TH1 * input_truth;
  TH1 * input_meas;
  TH2 * input_resp ;
  TH1 * meas ;
  TH1 * bkg  ;

  const int catAxis;
  const unsigned int nBkgs;

  mutable RooUnfold * unfoldCache;
  mutable RooUnfoldResponse * respCache;

  mutable TH1 * unfoldTruth;
  mutable TH1 * unfoldReco;
  mutable TMatrixD * covMatrix;






//private:
//  RooUnfold * rooUnfold;
};

} /* namespace QuickRefold */

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_MAKEREFOLD_H_ */
