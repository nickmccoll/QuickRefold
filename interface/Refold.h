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

class TAxis;
class TH1;
class TH2;
class MakeRefold;

namespace QuickRefold {

class Refold : public TNamed{
public:

  //status of the object
  enum Status {NONE, SETUP, FILLING, READING};

  Refold(); //required by root, don't use
  Refold(const char *name, int numAxes, bool storeCovMatrix = false);//stadard constructor
  virtual ~Refold();



  //Functions to access values and errors

  //Only use if you have exactly one axis
  float getValue(const float& input) const;
  float getError(const float& input) const;

  //Only use if you have exactly two axes
  float getValue(const float& input0, const float& input1) const;
  float getError(const float& input0, const float& input1) const;

  //If you want to access n-axis values. Set each axis witht the setBin function
  void setBin(const unsigned int& axis, const float& input) const;
  float getValue() const;
  float getError() const;
  float getSqrtError() const {return TMath::Sqrt(getError());}

  //or you can just do it all yourself for optimization
  float getValue(const std::vector<unsigned int>& bins ) const;
  float getError(const std::vector<unsigned int>& bins ) const;

  //Functions to access the axes
  unsigned int getNumberOfAxes() const {return nAxes;};
  const TAxis * getAxis(unsigned int axis) const;
  //negative one means the axis was not found
  int findAxis(const TString& axisName) const;
  unsigned int findAxisBin(unsigned int axis, double input) const;
  unsigned int findAxisBin(const TAxis& axis, double input) const; //modification of FindFixBin to ignore under/overflow
  TString getAxisBinTitle(unsigned int axis, float input) const;
  TString getAxisBinTitle(unsigned int axis, unsigned int bin) const;
  unsigned int getNumberOfBins() const {return nBins;};


  //Functions to print or draw the stored values
  TString getBinTitle(unsigned int iB) const;
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
  void addAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis);
  void addAxis(unsigned int axis, const char *name, unsigned int nBins, float* axisValues);
  void addAxis(unsigned int axis, const TAxis * inAxis);
  void stopSetup(bool setupOnlyAxes = false); //You are done adding axes
  void setError(float in); //fills diagonal, from binCache
  void setValue(float in); //from binCache
  void setError(unsigned int bin1,unsigned int bin2, float in); //fills covariance
  void setValue(unsigned int bin, float in); //fils direct
  void setErrors(const std::vector<float>& inErrors);
  void setValues(const std::vector<float>& inValues);


  //helper functions
  unsigned int translateToBin(const std::vector<unsigned int>& inBins) const;
  unsigned int translateToCovBin(unsigned int bin1,unsigned int bin2) const;

private:
  void  setBinIntFast(unsigned int axis,unsigned int bin) const {binCache->at(axis) = bin;};
  void  setBinValFast(unsigned int axis,float input) const {binCache->at(axis) = findAxisBin(axis,input);};
  unsigned int getBin(const std::vector<unsigned int>& inBins) const;
  unsigned int getBin() const {return getBin(*binCache);}
  unsigned int getCovBin(unsigned int bin1,unsigned int bin2) const;
  void fillBinCache(unsigned int inBin) const;



private:
  //Stored values
  unsigned int nAxes;
  TAxis * axes;
  float * values;
  bool storeFullCov;
  float * covarianceMatrix;

  //Not stored, setup on the fly
  Status status;
  unsigned int nBins;
  float * errors;
  unsigned int   * metrics;
  mutable std::vector<unsigned int> * binCache;


  friend class MakeRefold;

public:
  ClassDef(Refold,1)
};

} /* namespace QuickRefold */

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_REFOLD_H_ */
