/*
 * Refold.h
 *
 *  Created on: Jul 29, 2015
 *      Author: nmccoll
 */

#ifndef QUICKREFOLD_INTERFACE_REFOLD_H_
#define QUICKREFOLD_INTERFACE_REFOLD_H_

#include <vector>
#include <iostream>
#include "TNamed.h"
#include "TMath.h"
#include "BaseCorrector.h"

class TAxis;
class TH1;
class TH2;
class MakeRefold;

namespace QuickRefold {

class Refold : public BaseCorrector<float>{
public:

  Refold(); //required by root, don't use
  Refold(const char *name, int numAxes, bool storeCovMatrix = false);//stadard constructor
  virtual ~Refold();



  //Functions to access values and errors

  //Only use if you have exactly one axis
  float getError(const float& input) const;

  //Only use if you have exactly two axes
  float getError(const float& input0, const float& input1) const;

  //If you want to access n-axis values. Set each axis witht the setBin function
  float getError() const;
  float getSqrtError() const {return TMath::Sqrt(getError());}

  //or you can just do it all yourself for optimization
  float getError(const std::vector<unsigned int>& bins ) const;

  // print all values in a list
  void printValues(std::ostream& output = std::cout) const;
  //print covariance matrix
  void printCovariance(std::ostream& output = std::cout) const;
  //draw all values with the diagonal of the covariance matrix as errors
  // makes a new TH1, so you have to delete it on your own
  TH1* draw(const TString& name, const TString& title) const;
  //draw all values with the diagonal of the covariance matrix as errors
  // makes a new TH1, so you have to delete it on your own
  //only works in the case of exactly two axes
  TH2* draw2D(const TString& name, const TString& title) const;
  TH2* drawCov(const TString& name, const TString& title) const;


  //Functions to fill the object
  //errors are always in the form of cov matrix entries, so even when you
  //do not store the cov matrix you should input sig^2
  void stopSetup(bool setupOnlyAxes = false); //You are done adding axes
  void setError(float in); //fills diagonal, from binCache
  void setError(unsigned int bin1,unsigned int bin2, float in); //fills covariance
  void setErrors(const std::vector<float>& inErrors);


  //helper functions
  unsigned int translateToCovBin(unsigned int bin1,unsigned int bin2) const;

private:
  unsigned int getCovBin(unsigned int bin1,unsigned int bin2) const;



private:
  bool storeFullCov;
  float * covarianceMatrix;

  //Not stored, setup on the fly
  float * errors;


  friend class MakeRefold;

public:
  ClassDef(Refold,1)
};

} /* namespace QuickRefold */

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_REFOLD_H_ */
