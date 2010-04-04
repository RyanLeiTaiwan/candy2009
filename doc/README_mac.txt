For users:
1. In Terminal, switch to the product directory, e.g.,
  $ cd ~/Downloads/candy2009_mac_xxxxxxxx
2. Copy the 5 .dylib files to /usr/local/lib.
  $ sudo mkdir -p /usr/local/lib
  $ sudo cp *.dylib /usr/local/lib
3. Run the 3 programs respectively with appropriate parameters.
  $ ./detect
  $ ./train
  $ ./gen_neg
