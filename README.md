# QuickRefold

A quick and easy way to interface with RooUnfold:

http://hepunx.rl.ac.uk/~adye/software/unfold/RooUnfold.html

Unfolding is a lot of fun and the RooUnfold team put a lot of work into it. What this package does is make it easy for you to interface their tool and to then store your results in easy to parse ROOT files.

See the examples directory for more info, but the summary is that we have written a wrapper around the unfolding matrix so you can:

1. Store it in a root file easily to share.
2. Analyze the unfolding matrix in the commandline
3. Easily use it in your code with nice user-understandable functions

You of course need RooUnfold to work. In the RooUnfold directory you can find some things to help getting that started (and to install in CMSSW).
