We found that we needed to do a few things to get RooUnfold to work in CMSSW. First, you can use the `Buildfile.xml` located in this directory.

You will also need to add some includes to `src/RooUnfold_LinkDef.h`:

```
#include "RooUnfold.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldErrors.h"
#include "RooUnfoldParms.h"
#include "RooUnfoldInvert.h"
#include "RooUnfoldTUnfold.h"
#include "TSVDUnfold_local.h"
```
And in a few source files we had to include a TBuffer.h include: `#include "TBuffer.h"`

This may not be necessary if you use an upgraded version of the code. This version is based off of revision 352:  

https://gitlab.cern.ch/RooUnfold/RooUnfold/commit/c4469a1f9729a0646b509e25b2adc306b0623b19
