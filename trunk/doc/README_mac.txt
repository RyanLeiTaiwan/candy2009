For users:
1. In Terminal, copy the 5 .dylib files to /usr/local/lib.
  $ sudo mkdir -p /usr/local/lib
  $ sudo cp *.dylib /usr/local/lib
2. Run the 3 programs respectively with appropriate parameters.
  $ ./detect
  $ ./train
  $ ./gen_neg
