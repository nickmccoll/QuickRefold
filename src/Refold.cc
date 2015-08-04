/*
 * Refold.cc
 *
 *  Created on: Jul 29, 2015
 *      Author: nmccoll
 */

#include <stdexcept>

#include "TAxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TClass.h"
#include "../interface/Refold.h"

namespace QuickRefold {
Refold::Refold() : nAxes(0), axes(0), values(0),storeFullCov(false), covarianceMatrix(0),
     status(NONE),
     nBins(0),errors(0), metrics(0),binCache(0)
 {};

Refold::Refold(const char *name, int numAxes, bool storeCovMatrix) : nAxes(numAxes), axes(new TAxis [nAxes]),  values(0),storeFullCov(storeCovMatrix), covarianceMatrix(0),
    status(SETUP), nBins(0),errors(0),metrics(new unsigned int [nAxes]),binCache(new std::vector<unsigned int>(nAxes,1u))
{ SetName(name);};

Refold::~Refold(){
    delete [] errors;
    delete [] metrics;
    delete [] axes;
    delete [] values;
    delete [] covarianceMatrix;
    delete binCache;
  };

  //Functions to access values and errors
  float Refold::getValue(const float& input) const {
    if(nAxes != 1) throw std::invalid_argument("This version of Refold::getValue assumes only one axis");
    return values[findAxisBin(0,input)];
  }
  float Refold::getError(const float& input) const {
    if(nAxes != 1) throw std::invalid_argument("This version of Refold::getError assumes only one axis");
    return errors[findAxisBin(0,input)];
  }
  float Refold::getValue(const float& input0, const float& input1) const {
    if(nAxes != 2) throw std::invalid_argument("This version of Refold::getValue assumes two axes");
    binCache->at(0) = findAxisBin(0,input0);
    binCache->at(1) = findAxisBin(1,input1);
    return values[getBin(*binCache)];
  }
  float Refold::getError(const float& input0, const float& input1) const {
    if(nAxes != 2) throw std::invalid_argument("This version of Refold::getError assumes two axes");
    binCache->at(0) = findAxisBin(0,input0);
    binCache->at(1) = findAxisBin(1,input1);
    return errors[getBin(*binCache)];
  }
  void Refold::setBin(const unsigned int& axis, const float& input) const{
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::setBin: tried to access axis %u but only %u exist.",axis,nAxes).Data());
    setBinValFast(axis,input);
  }
  float Refold::getValue() const {
    return values[getBin(*binCache)];
  }
  float Refold::getError() const {
    return errors[getBin(*binCache)];
  }
  float Refold::getValue(const std::vector<unsigned int>& bins ) const {
    if(bins.size() != nAxes) throw std::invalid_argument(TString::Format("Refold::getValue: tried to access %lu axes but the structure contains %u",bins.size(),nAxes).Data());
    return values[getBin(bins)];
  }
  float Refold::getError(const std::vector<unsigned int>& bins ) const{
    if(bins.size() != nAxes) throw std::invalid_argument(TString::Format("Refold::getError: tried to access %lu axes but the structure contains %u",bins.size(),nAxes).Data());
    return errors[getBin(bins)];
  }

  //Functions to access the axes
  const TAxis * Refold::getAxis(unsigned int axis) const {
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::getAxis: tried to access axis %u but only %u exist.",axis,nAxes).Data());
    return &axes[axis];
  }
  int Refold::findAxis(const TString& axisName) const{
    for(unsigned int iA = 0; iA < nAxes; ++iA){
      if(!axisName.EqualTo(axes[iA].GetName())) continue;
      return iA;
    }
    return -1;
  }
  unsigned int Refold::findAxisBin(unsigned int axis, double input) const {
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::findAxisBin: tried to access axis %u but only %u exist.",axis,nAxes).Data());
    return findAxisBin(axes[axis],input);
  }
  unsigned int Refold::findAxisBin(const TAxis& axis, double input) const {
    unsigned int bin;
    double xMin = axis.GetXmin();
    double xMax = axis.GetXmax();
    unsigned int nBins = axis.GetNbins();
    if(input < xMin){
      bin = 1;
    } else if( !(input < xMax)){
      bin = nBins;
    } else {
      if (!axis.IsVariableBinSize()) {
        bin = 1 + int (nBins*(input-xMin)/(xMax-xMin) );
      } else {
        bin = 1 + TMath::BinarySearch(axis.GetXbins()->fN,axis.GetXbins()->fArray,input);
      }
    }
    return bin;
  }
  TString Refold::getAxisBinTitle(unsigned int axis, float input) const {
    return getAxisBinTitle(axis,findAxisBin(axis,input));
  }
  TString Refold::getAxisBinTitle(unsigned int axis, unsigned int bin) const {
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::getAxisBinTitle: tried to access axis %u but only %u exist.",axis,nAxes).Data());
    if (bin <= 1)             return TString::Format("< %.4g", axes[axis].GetBinUpEdge(1));
    if (int(bin) >= axes[axis].GetNbins())  return TString::Format("%.4g #leq" , axes[axis].GetBinLowEdge(axes[axis].GetNbins()) );
    return TString::Format("%.4g - %.4g", axes[axis].GetBinLowEdge(bin), axes[axis].GetBinLowEdge(bin));
  }


  //Functions to print or draw the stored values
  TString Refold::getBinTitle(unsigned int iB) const{
    TString title;
    fillBinCache(iB);
    for(unsigned int iA = 0; iA < nAxes; ++iA){
      title += "(";
      title +=  getAxisBinTitle(iA,binCache->at(iA)) ;
      title +=")";
    }
    return title;
  }
  void Refold::printValues(std::ostream& output) const {
    output          << TString::Format("%u Axes:",nAxes) << std::endl;
    for(unsigned int iA = 0; iA < nAxes; ++iA){
      output          << axes[iA].GetName() << "\t";
    }
    output << std::endl;
    for(unsigned int iB = 0; iB < nBins; ++iB){
      output << getBinTitle(iB);
      output <<"\t"<< values[iB] <<"\t+\\-\t"<<TMath::Sqrt(errors[iB]) << std::endl;
    }
  }
  void Refold::printCovariance(std::ostream& output) const {
    if(!storeFullCov) throw std::invalid_argument("Refold::printCovariance: The covariance matrix was not stored." );

    for(unsigned int iA = 0; iA < nAxes; ++iA){
      output          << axes[iA].GetName() << "\t";
    }
    output << std::endl;

    for(unsigned int iB = 0; iB < nBins; ++iB){
      output <<"\t\t";
      output << getBinTitle(iB);
      output <<"\t";
    }
    output << std::endl;

    for(unsigned int iB1 = 0; iB1 < nBins; ++iB1){
      output << getBinTitle(iB1);
      output <<"\t";
      for(unsigned int iB2 = 0; iB2 < nBins; ++iB2){
        output << TString::Format("%.2f",covarianceMatrix[getCovBin(iB1,iB2)]) <<"\t";
      }
      output << std::endl;
    }
  }
  TH1* Refold::draw(const TString& name, const TString& title) const {

    TH1*            histo         = nAxes == 1 ? (axes[0].GetXbins()->GetSize() ? new TH1F(name,title,nBins, axes[0].GetXbins()->GetArray())
                                                    : new TH1F(name,title,nBins, axes[0].GetXmin(),axes[0].GetXmax()))
                                                    : new TH1F(name,title,nBins, -.5, float(nBins) -.5);

    histo->Sumw2();
    for(unsigned int iB = 0; iB < nBins; ++iB){
      histo->SetBinContent(iB+1,values[iB]);
      histo->SetBinError(iB+1,TMath::Sqrt(errors[iB]));
    }
    histo->SetEntries(nBins);
    return histo;
  }
  TH2* Refold::draw2D(const TString& name, const TString& title) const {
    if(nAxes != 2) throw std::invalid_argument("Refold::draw2D: Not two dimensions." );

    bool v1 = axes[0].GetXbins()->GetSize();
    bool v2 = axes[1].GetXbins()->GetSize();

    TH2*            histo;
    if(v1 && v2){
      histo         =  new TH2F(name,title,axes[0].GetNbins(), axes[0].GetXbins()->GetArray(),axes[1].GetNbins(), axes[1].GetXbins()->GetArray());
    } else if (v1){
      histo         =  new TH2F(name,title,axes[0].GetNbins(), axes[0].GetXbins()->GetArray(),axes[1].GetNbins(), axes[1].GetXmin(), axes[1].GetXmax());
    } else if(v2){
      histo         =  new TH2F(name,title,axes[0].GetNbins(), axes[0].GetXmin(), axes[0].GetXmax(),axes[1].GetNbins(), axes[1].GetXbins()->GetArray());
    } else {
      histo         =  new TH2F(name,title,axes[0].GetNbins(), axes[0].GetXmin(), axes[0].GetXmax(),axes[1].GetNbins(), axes[1].GetXmin(), axes[1].GetXmax());
    }

    histo->Sumw2();
    for(unsigned int iB = 0; iB < nBins; ++iB){
      fillBinCache(iB);
      histo->SetBinContent(binCache->at(0),binCache->at(1),values[iB]);
      histo->SetBinError(binCache->at(0),binCache->at(1),TMath::Sqrt(errors[iB]));
    }
    histo->SetEntries(nBins);
    return histo;
  }
  TH2* Refold::drawCov(const TString& name, const TString& title) const {
    if(!storeFullCov) throw std::invalid_argument("Refold::drawCov: The covariance matrix was not stored." );
    TH2 * histo = new TH2F(name,title,nBins,-.5, float(nBins) -.5,nBins,-.5, float(nBins) -.5);
    histo->Sumw2();
    for(unsigned int iB1 = 0; iB1 < nBins; ++iB1){
      for(unsigned int iB2 = 0; iB2 < nBins; ++iB2){
      histo->SetBinContent(iB1 + 1, iB2 + 1, covarianceMatrix[getCovBin(iB1,iB2)]);
    }
    }
    return histo;
  }


  //Functions to fill the object
  void Refold::addAxis(unsigned int axis, const char *name, unsigned int nBins, float minAxis, float maxAxis) {
    if(status != SETUP) throw std::invalid_argument("Refold::addAxis: Not in SETUP mode." );
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::addAxis: tried to setup axis %u but only %u exist.",axis,nAxes).Data());
    axes[axis].Set(nBins,minAxis,maxAxis);
    axes[axis].SetName(name);
  }
  void Refold::addAxis(unsigned int axis,const char *name, unsigned int nBins, float* axisValues){
    if(status != SETUP) throw std::invalid_argument("Refold::addAxis: Not in SETUP mode." );
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::addAxis: tried to setup axis %u but only %u exist.",axis,nAxes).Data());
    axes[axis].Set(nBins,axisValues);
    axes[axis].SetName(name);
  }
  void Refold::addAxis(unsigned int axis, const TAxis * inAxis){
    if(status != SETUP) throw std::invalid_argument("Refold::addAxis: Not in SETUP mode." );
    if(axis >= nAxes) throw std::invalid_argument(TString::Format("Refold::addAxis: tried to setup axis %u but only %u exist.",axis,nAxes).Data());
    axes[axis] = *inAxis;
  }

  void Refold::stopSetup(bool setupOnlyAxes) {
    if(status != SETUP) throw std::invalid_argument("Refold::stopSetup: Not in SETUP mode." );
    status = FILLING;

    nBins = 1;
    metrics[0] = 1;
    for(unsigned int iA = 0; iA < nAxes;){
      if(axes[iA].GetNbins() == 0) throw std::invalid_argument("Refold::stopSetup: Not all axes have been setup." );
      nBins *= axes[iA].GetNbins();
      if (++iA < nAxes)
        metrics[iA] = nBins;
    }

    if(setupOnlyAxes) return;

    errors = new float[nBins];
    values = new float[nBins];
    for(unsigned int iB = 0; iB < nBins; ++iB){
      errors[iB]  = 0;
      values[iB]  = 0;
    }

    if(storeFullCov){
      covarianceMatrix = new float[nBins*nBins];
      for(unsigned int iB = 0; iB < nBins*nBins; ++iB){
        covarianceMatrix[iB]  = 0;
      }
    }
  }
  void Refold::setError(float in){
    if(status != FILLING) throw std::invalid_argument("Refold::setError: Not in FILLING mode." );
    int bin = getBin(*binCache);
    errors[bin] = in;
    if(storeFullCov) covarianceMatrix[getCovBin(bin,bin)] = in;
  }
  void Refold::setValue(float in){
    if(status != FILLING) throw std::invalid_argument("Refold::setValue: Not in FILLING mode." );
    int bin = getBin(*binCache);
    values[bin] = in;
  }
  void Refold::setError(unsigned int bin1,unsigned int bin2, float in) {
    if(status != FILLING) throw std::invalid_argument("Refold::setError: Not in FILLING mode." );
    if(bin1 >= nBins || bin2 >= nBins) throw std::invalid_argument(TString::Format("Refold::setError: (%u,%u) out of bounds (%u)",bin1,bin2,nBins ).Data());
    if(storeFullCov) covarianceMatrix[getCovBin(bin1,bin2)] = in;
    if(!storeFullCov || bin1 == bin2) errors[bin1] = in;
  }
  void Refold::setValue(unsigned int bin, float in){
    if(status != FILLING) throw std::invalid_argument("Refold::setValue: Not in FILLING mode." );
    if(bin >= nBins) throw std::invalid_argument(TString::Format("Refold::setValue: %u out of bounds (%u)",bin,nBins ).Data());
    values[bin] = in;
  }
  void Refold::setErrors(const std::vector<float>& inErrors){
    if(status != FILLING) throw std::invalid_argument("Refold::setErrors: Not in FILLING mode." );
    if(storeFullCov){
      if(inErrors.size() != nBins*nBins) throw std::invalid_argument(TString::Format("Refold::setErrors: Expect cov. matrix with size %u, you gave %lu",nBins*nBins,inErrors.size()).Data() );
      for(unsigned int iB = 0; iB < nBins*nBins; ++iB){
        covarianceMatrix[iB] = inErrors[iB];
      }
      for(unsigned int iB = 0; iB < nBins; ++iB){
        errors[iB] = covarianceMatrix[getCovBin(iB,iB)];
      }
    } else {
      if(inErrors.size() != nBins) throw std::invalid_argument(TString::Format("Refold::setErrors: Expect errors with size %u, you gave %lu",nBins,inErrors.size()).Data() );
      for(unsigned int iB = 0; iB < nBins; ++iB){
        errors[iB] = inErrors[iB];
      }
    }
  }
  void Refold::setValues(const std::vector<float>& inValues){
    if(status != FILLING) throw std::invalid_argument("Refold::setValues: Not in FILLING mode." );
    if(inValues.size() != nBins) throw std::invalid_argument(TString::Format("Refold::setValues: Expect values with size %u, you gave %lu",nBins,inValues.size()).Data() );
    for(unsigned int iB = 0; iB < nBins; ++iB){
      values[iB] = inValues[iB];
    }
  }

  //helper functions
  unsigned int Refold::translateToBin(const std::vector<unsigned int>& inBins) const {
    if(inBins.size() != nAxes) throw std::invalid_argument(TString::Format("Refold::translateToBin: Expect size %u, you gave %lu",nAxes,inBins.size()).Data() );
    return getBin(inBins);
  }
  unsigned int Refold::translateToCovBin(unsigned int bin1,unsigned int bin2) const{
    if(bin1 >= nBins || bin2 >= nBins) throw std::invalid_argument(TString::Format("Refold::translateToCovBin: (%u,%u) out of bounds (%u)",bin1,bin2,nBins ).Data());
    return getCovBin(bin1,bin2);
  }
  unsigned int Refold::getBin(const std::vector<unsigned int>& inBins) const {
    unsigned int bin = 0;
    for(unsigned int iA = 0; iA < nAxes; ++iA)
      bin += metrics[iA] * (inBins[iA] -1);
    return bin;
  }
  unsigned int Refold::getCovBin(unsigned int bin1,unsigned int bin2) const {
   return bin1*nBins + bin2;
  }
  void Refold::fillBinCache(unsigned int inBin) const {
    for(unsigned int iA = 0; iA < nAxes; ++iA){
      binCache->at(iA) = inBin % axes[iA].GetNbins() +1;
      inBin -= (binCache->at(iA) -1);
      inBin /= axes[iA].GetNbins();
    }
  }



  // Streamer
  void Refold::Streamer(TBuffer& R__b){
    if (R__b.IsReading())
    {
      R__b.ReadVersion();
      TObject::Class()->ReadBuffer(R__b, this);
      R__b >> nAxes;
      R__b >>storeFullCov;

      axes = new TAxis [nAxes];
      R__b.ReadFastArray(axes,TAxis::Class(),nAxes);

      status = SETUP;
      metrics = new unsigned int [nAxes];
      binCache = new std::vector<unsigned int>(nAxes,1u);
      stopSetup();

      R__b.ReadFastArray(values,nBins);
      if(storeFullCov){
        R__b.ReadFastArray(covarianceMatrix,nBins*nBins);
        for(unsigned int iB = 0; iB < nBins; ++iB){
          errors[iB] = covarianceMatrix[getCovBin(iB,iB)];
        }
      } else {
        R__b.ReadFastArray(errors,nBins);
      }

      status = READING;

    } else {
      R__b.WriteVersion(IsA());
      TObject::Class()->WriteBuffer(R__b, this);
      R__b                <<  nAxes;
      R__b <<storeFullCov;
      R__b.WriteFastArray(axes,TAxis::Class(),nAxes);
      R__b.WriteFastArray(values,nBins);
      if(storeFullCov){
        R__b.WriteFastArray(covarianceMatrix,nBins*nBins);
      } else {
        R__b.WriteFastArray(errors,nBins);
      }

    }
  }

} /* namespace QuickRefold */

//ClassImp(QuickRefold::Refold)
