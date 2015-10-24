/*
 * Refold.h
 *
 *  Created on: Jul 29, 2015
 *      Author: nmccoll
 */

#ifndef QUICKREFOLD_INTERFACE_TOBJECTCONTAINER_H_
#define QUICKREFOLD_INTERFACE_TOBJECTCONTAINER_H_

#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH2D.h"

#include "BaseCorrector.h"

namespace QuickRefold {

template<typename data>
class TObjectContainer : public BaseCorrector<data>{
public:
  TObjectContainer(); //required by root, don't use
  TObjectContainer(const char *name, int numAxes);//stadard constructor
  virtual ~TObjectContainer();
  //Functions to fill the object
  void stopSetup(bool setupOnlyAxes = false); //You are done adding axes
public:
  ClassDef(TObjectContainer,1)
};

typedef TObjectContainer<TH1F>   TH1FContainer;
typedef TObjectContainer<TH1D>   TH1DContainer;
typedef TObjectContainer<TH2F>   TH2FContainer;
typedef TObjectContainer<TH2D>   TH2DContainer;
} /* namespace QuickRefold */

#include "../src/TObjectContainer.icc"

#endif /* ANALYSISTOOLS_QUICKREFOLD_INTERFACE_REFOLD_H_ */
