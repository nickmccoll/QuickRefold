#include "../interface/TF1Container.h"

#include <stdexcept>


namespace QuickRefold {


TF1Container::TF1Container() : BaseCorrector<TF1>()
{};


TF1Container::TF1Container(const char *name, int numAxes) : BaseCorrector<TF1>(numAxes)
{BaseCorrector<TF1>::SetName(name);};


TF1Container::~TF1Container(){
};


void TF1Container::stopSetup(bool setupOnlyAxes) {
  BaseCorrector<TF1>::stopSetup(setupOnlyAxes);
}

// Streamer
void TF1Container::Streamer(TBuffer& R__b){
  if (R__b.IsReading())
  {
    R__b.ReadVersion();
    TObject::Class()->ReadBuffer(R__b, this);
    R__b >> BaseCorrector<TF1>::nAxes;

    BaseCorrector<TF1>::axes = new TAxis [BaseCorrector<TF1>::nAxes];
    R__b.ReadFastArray(BaseCorrector<TF1>::axes,TAxis::Class(),BaseCorrector<TF1>::nAxes);

    BaseCorrector<TF1>::status = BaseCorrector<TF1>::SETUP;
    BaseCorrector<TF1>::metrics = new unsigned int [BaseCorrector<TF1>::nAxes];
    BaseCorrector<TF1>::binCache = new std::vector<unsigned int>(BaseCorrector<TF1>::nAxes,1u);
    stopSetup();

    for(unsigned int iB =0; iB < BaseCorrector<TF1>::nBins; ++iB){
      TString name;
      TString formula;
      R__b >> name;
      R__b >> formula;
      TF1 * tempF = new TF1(name,formula);
      values[iB] = (*tempF);
      delete tempF;
    }

    BaseCorrector<TF1>::status = BaseCorrector<TF1>::READING;

  } else {
    R__b.WriteVersion(IsA());
    TObject::Class()->WriteBuffer(R__b, this);
    R__b                <<  BaseCorrector<TF1>::nAxes;
    R__b.WriteFastArray(BaseCorrector<TF1>::axes,TAxis::Class(),BaseCorrector<TF1>::nAxes);

    for(unsigned int iB =0; iB < BaseCorrector<TF1>::nBins; ++iB){
      const TF1& func = values[iB];
      R__b                <<  TString(func.GetName());
      R__b                <<  TString(func.GetExpFormula("p"));
    }
  }
}

} /* namespace QuickRefold */
