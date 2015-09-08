/*
 * Refold.h
 *
 *  Created on: Jul 29, 2015
 *      Author: nmccoll
 */

#ifndef QUICKREFOLD_INTERFACE_BASECORRECTOR_H_
#define QUICKREFOLD_INTERFACE_BASECORRECTOR_H_

#include <vector>
#include <iostream>
#include "TNamed.h"
#include "TMath.h"

class TAxis;
class TH1;
class TH2;
class MakeRefold;

namespace QuickRefold {

template<typename data>
class BaseCorrector: public TNamed{
public:

  //status of the object
  enum Status {NONE, SETUP, FILLING, READING};

  BaseCorrector(); //required by root, don't use
  BaseCorrector(int numAxes);//stadard constructor
  virtual ~BaseCorrector();

  //Functions to access values and errors

  //Only use if you have exactly one axis
  const data& getValue(const float& input) const;

  //Only use if you have exactly two axes
  const data& getValue(const float& input0, const float& input1) const;

  //If you want to access n-axis values. Set each axis witht the setBin function
  void setBin(const unsigned int& axis, const float& input) const;
  const data& getValue() const;

  //or you can just do it all yourself for optimization
  const data& getValue(const std::vector<unsigned int>& bins ) const;

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

  //Functions to fill the object
  void addAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis);
  void addAxis(unsigned int axis, const char *name, unsigned int nBins, float* axisValues);
  void addAxis(unsigned int axis, const TAxis * inAxis);
  virtual void stopSetup(bool setupOnlyAxes = false); //You are done adding axes
  void setValue(const data& in); //from binCache
  void setValue(unsigned int bin, const data& in); //fils direct
  void setValues(const std::vector<data>& inValues);


  //helper functions
  unsigned int translateToBin(const std::vector<unsigned int>& inBins) const;

protected:
  void  setBinIntFast(unsigned int axis,unsigned int bin) const {binCache->at(axis) = bin;};
  void  setBinValFast(unsigned int axis,float input) const {binCache->at(axis) = findAxisBin(axis,input);};
  unsigned int getBin(const std::vector<unsigned int>& inBins) const;
  unsigned int getBin() const {return getBin(*binCache);}
  void fillBinCache(unsigned int inBin) const;

protected:
  //Stored values
  unsigned int nAxes;
  TAxis * axes;
  data * values;

  //Not stored, setup on the fly
  Status status;
  unsigned int nBins;
  unsigned int   * metrics;
  mutable std::vector<unsigned int> * binCache;
public:
  ClassDef(BaseCorrector,0)

};

} /* namespace QuickRefold */

#include "../src/BaseCorrector.icc"

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_REFOLD_H_ */
