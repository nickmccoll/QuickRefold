/*
 * Refold.cc
 *
 *  Created on: Jul 29, 2015
 *      Author: nmccoll
 */

#include <stdexcept>
#include "../interface/FormulaContainer.h"

namespace QuickRefold {
FormulaContainer::FormulaContainer() : BaseCorrector<TF1>()
 {};

FormulaContainer::FormulaContainer(const char *name, int numAxes) : BaseCorrector<TF1>(numAxes)
{SetName(name);};

FormulaContainer::~FormulaContainer(){
  };

  void FormulaContainer::stopSetup(bool setupOnlyAxes) {
    BaseCorrector<TF1>::stopSetup(setupOnlyAxes);
  }
  // Streamer
  void FormulaContainer::Streamer(TBuffer& R__b){
    if (R__b.IsReading())
    {
      R__b.ReadVersion();
      TObject::Class()->ReadBuffer(R__b, this);
      R__b >> nAxes;

      axes = new TAxis [nAxes];
      R__b.ReadFastArray(axes,TAxis::Class(),nAxes);

      status = SETUP;
      metrics = new unsigned int [nAxes];
      binCache = new std::vector<unsigned int>(nAxes,1u);
      stopSetup();

      R__b.ReadFastArray(values,TF1::Class(),nBins);
      status = READING;

    } else {
      R__b.WriteVersion(IsA());
      TObject::Class()->WriteBuffer(R__b, this);
      R__b                <<  nAxes;
      R__b.WriteFastArray(axes,TAxis::Class(),nAxes);
      R__b.WriteFastArray(values,TF1::Class(),nBins);
    }
  }

} /* namespace QuickRefold */

//ClassImp(QuickRefold::Refold)
