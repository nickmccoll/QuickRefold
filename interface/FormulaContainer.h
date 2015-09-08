/*
 * Refold.h
 *
 *  Created on: Jul 29, 2015
 *      Author: nmccoll
 */

#ifndef QUICKREFOLD_INTERFACE_FORMULACONTAINER_H_
#define QUICKREFOLD_INTERFACE_FORMULACONTAINER_H_

#include "BaseCorrector.h"
#include "TF1.h"

namespace QuickRefold {

class FormulaContainer : public BaseCorrector<TF1>{
public:
  FormulaContainer(); //required by root, don't use
  FormulaContainer(const char *name, int numAxes);//stadard constructor
  virtual ~FormulaContainer();
  //Functions to fill the object
  void stopSetup(bool setupOnlyAxes = false); //You are done adding axes
public:
  ClassDef(FormulaContainer,1)
};

} /* namespace QuickRefold */

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_REFOLD_H_ */
