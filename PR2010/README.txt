* Project hosting at http://code.google.com/p/search
  Email: ryanlei750328@gmail.com
  Update: 2011/10/04

* System requirements:
  1. MATLAB or GNU/Octave (not guaranteed). Tested on MATLAB R2009b for Mac. 
  2. libSVM main package (for grid.py) and libSVM MATLAB interface. Included
  in the source code is version 2.91 for both packages. Check the README there
  for prerequisites, such as python and gnuplot. For Unix users, check the 
  README there for compilation. By default, both libSVM directories are
  one level above train.m and predict.m. If you use other versions of libSVM
  or change their paths, you need to modify some lines in train.m.

* How to put MY dataset:
  Of course, you can choose to download my dataset from Google Code and extract
  it. Make sure the subdirectories are at the same path as train.m, predict.m.

* How to put YOUR dataset [This is important]:
  [Note] The repository does NOT provide visual words extraction tools due to
  confidential reasons. Please prepare your own.

  If you want to use your own dataset, make sure you have AT LEAST the
  following folders and files placed at the same path as train.m and predict.m:
  Suppose the class name for 1 is ONE, for 2 is TWO, for 3 is THREE, etc.
  1. A training set folder (let's call it dataset_train)
	 |--- class 1 visual words features (text file) -> VW_train_ONE
	 |--- class 2 visual words features (text file) -> VW_train_TWO
	 |--- class 3 visual words features (text file) -> VW_train_THREE
	 |--- ... etc.
  2. A testing set folder (let's call it dataset_test)
	 |--- class 1 visual words features (text file) -> VW_test_ONE
	 |--- class 2 visual words features (text file) -> VW_test_TWO
	 |--- class 3 visual words features (text file) -> VW_test_THREE
	 |--- ... etc.
     |--- [class 1 folder containing images] -> ONE
	 |--- [class 2 folder containing images] -> TWO
	 |--- [class 2 folder containing images] -> THREE
	 |--- [... etc.]
	 |--- [class 1 image file name list] -> list_test_ONE
	 |--- [class 2 image file name list] -> list_test_TWO
	 |--- [class 3 image file name list] -> list_test_THREE
	 |--- [... etc.]

  After "->" is how you should name these folders or files.
  Those marked with [] are only necessary for the demo mode command:
    predict(true).

  Finally, in train.m, modify:
  1. class_name = {'ONE';'TWO';'THREE';...};
  2. train_directory = 'dataset_train';
  3. test_directory = 'dataset_test';

  [Note] 
    1. The VW features should be a DATA_SIZE x DIMENSION matrix, with format
	compatible with MATLAB's load() function.
	2. The file name lists contain image file names line by line in the same
	order as the VW features. These files should be compatible with MATLAB's
	importdata() function. (Octave doesn't seem to have it?)

* Function parameters for train.m:
  1. write_file (logical): meaning "DON'T TRAIN, but write to file in libSVM
  format". Default: false.
  2. var_percentage_in: percentage of total variance retained in PCA.
  Range: [0,100]. Default: 100 (no PCA).
  var_percentage_in = 0 means directly using the previous transformation A
  without recomputing PCA [Use this with caution].
  3. C: parameter C (cost) in libSVM. Default: 1.
  4. gamma: parameter gamma in libSVM. Default: 1 / dimensionality.

* Function parameters for predict.m:
  1. ShowEach (logical): Show the image and classification result one by one.
  Default: false.

* How to run the program:
  1. Quick start (No SVM parameter selection. No PCA):
  >> train
  >> predict  OR  predict(true)

  2. Attempt 2 & 4 (With SVM parameter selection. No PCA):
  >> train(true)
     Now use the OS terminal (cmd on Windows) to switch to the tools/ directory
	 of libSVM main package, e.g., libsvm-2.91/tools/. And over there:
   $ python grid.py VW_train
  (VW_train should already appear there. And this requires gnuplot)
     This takes a LONG time. If grid.py eventually returns a line like:
       32.0 0.0001220703125 93.2839
     Then 32.0 means C, and 0.0001220703125 means gamma.
     Now go back to MATLAB and put the C and gamma in:
  >> train(false,100,C,gamma)
  >> predict  OR  predict(true)
  
  3. Attempt 3 (With SVM parameter selection. With p percentage of total
  variance retained in PCA):
  >> train(true,p);
     Use the terminal to switch to the tools/ directory of
	 libSVM main package. Then:
   $ python grid.py VW_train
     Memorize the C and gamma that grid.py returned.
     Go back to MATLAB, and put the C and gamma in:
  >> train(false,0,C,gamma)
  >> predict  OR  predict(true)

