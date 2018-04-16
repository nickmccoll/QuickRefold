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
#include "TBuffer.h"
#include "../interface/Refold.h"

namespace QuickRefold {
Refold::Refold() : BaseCorrector<float>(), storeFullCov(false), covarianceMatrix(0),errors(0)
 {};

Refold::Refold(const char *name, int numAxes, bool storeCovMatrix) : BaseCorrector<float>(numAxes), storeFullCov(storeCovMatrix), covarianceMatrix(0), errors(0)
{SetName(name);};

Refold::~Refold(){
    delete [] errors;
    delete [] covarianceMatrix;
  };

  //Functions to access values and errors
  float Refold::getError(const float& input) const {
    if(nAxes != 1) throw std::invalid_argument("This version of Refold::getError assumes only one axis");
    return errors[findAxisBin(0,input)];
  }
  float Refold::getError(const float& input0, const float& input1) const {
    if(nAxes != 2) throw std::invalid_argument("This version of Refold::getError assumes two axes");
    binCache->at(0) = findAxisBin(0,input0);
    binCache->at(1) = findAxisBin(1,input1);
    return errors[getBin(*binCache)];
  }
  float Refold::getError() const {
    return errors[getBin(*binCache)];
  }
  float Refold::getError(const std::vector<unsigned int>& bins ) const{
    if(bins.size() != nAxes) throw std::invalid_argument(TString::Format("Refold::getError: tried to access %lu axes but the structure contains %u",bins.size(),nAxes).Data());
    return errors[getBin(bins)];
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

  void Refold::stopSetup(bool setupOnlyAxes) {
    BaseCorrector<float>::stopSetup(setupOnlyAxes);
    if(setupOnlyAxes) return;

    errors = new float[nBins];
    for(unsigned int iB = 0; iB < nBins; ++iB){
      errors[iB]  = 0;
    }

    for(unsigned int iB = 0; iB < nBins; ++iB){
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
  void Refold::setError(unsigned int bin1,unsigned int bin2, float in) {
    if(status != FILLING) throw std::invalid_argument("Refold::setError: Not in FILLING mode." );
    if(bin1 >= nBins || bin2 >= nBins) throw std::invalid_argument(TString::Format("Refold::setError: (%u,%u) out of bounds (%u)",bin1,bin2,nBins ).Data());
    if(storeFullCov) covarianceMatrix[getCovBin(bin1,bin2)] = in;
    if(!storeFullCov || bin1 == bin2) errors[bin1] = in;
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
  unsigned int Refold::translateToCovBin(unsigned int bin1,unsigned int bin2) const{
    if(bin1 >= nBins || bin2 >= nBins) throw std::invalid_argument(TString::Format("Refold::translateToCovBin: (%u,%u) out of bounds (%u)",bin1,bin2,nBins ).Data());
    return getCovBin(bin1,bin2);
  }
  unsigned int Refold::getCovBin(unsigned int bin1,unsigned int bin2) const {
   return bin1*nBins + bin2;
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
