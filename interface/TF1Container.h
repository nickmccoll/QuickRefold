
#ifndef QUICKREFOLD_INTERFACE_TF1CONTAINER_H_
#define QUICKREFOLD_INTERFACE_TF1CONTAINER_H_

#include "TF1.h"
#include "BaseCorrector.h"

namespace QuickRefold {


class TF1Container : public BaseCorrector<TF1>{
public:
  TF1Container(); //required by root, don't use
  TF1Container(const char *name, int numAxes);//stadard constructor
  virtual ~TF1Container();
  //Functions to fill the object
  void stopSetup(bool setupOnlyAxes = false); //You are done adding axes
  float eval(float x) const {return values[getBin(*binCache)].Eval(x);}
  float eval(float x,float y) const {return values[getBin(*binCache)].Eval(x,y);}
  float eval(float x,float y, float z) const {return values[getBin(*binCache)].Eval(x,y,z);}

public:
  ClassDef(TF1Container,1)
};
} /* namespace QuickRefold */

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_REFOLD_H_ */
