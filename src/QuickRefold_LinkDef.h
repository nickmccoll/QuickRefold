
#include "../interface/Refold.h"
#include "../interface/TObjectContainer.h"
#include "../interface/TF1Container.h"
#include "../interface/MakeRefold.h"

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class QuickRefold::BaseCorrector<float> ;
#pragma link C++ class QuickRefold::BaseCorrector<TH1F> ;
#pragma link C++ class QuickRefold::BaseCorrector<TH1D> ;
#pragma link C++ class QuickRefold::BaseCorrector<TH2F> ;
#pragma link C++ class QuickRefold::BaseCorrector<TH2D> ;
#pragma link C++ class QuickRefold::BaseCorrector<TF1 > ;
#pragma link C++ class QuickRefold::Refold-;
#pragma link C++ class QuickRefold::TObjectContainer<TH1F>- ;
#pragma link C++ class QuickRefold::TObjectContainer<TH1D>- ;
#pragma link C++ class QuickRefold::TObjectContainer<TH2F>- ;
#pragma link C++ class QuickRefold::TObjectContainer<TH2D>- ;
#pragma link C++ class QuickRefold::TF1Container- ;
#pragma link C++ typedef QuickRefold::TH1FContainer        ;
#pragma link C++ typedef QuickRefold::TH1DContainer        ;
#pragma link C++ typedef QuickRefold::TH2FContainer        ;
#pragma link C++ typedef QuickRefold::TH2DContainer        ;
#pragma link C++ class QuickRefold::MakeRefold;
#endif
