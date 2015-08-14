Project owner: Ryan Lei (雷禹恆, ryanlei750328)

# Spring 2010: Pattern Recognition - Final Project #
## Title: Using Visual Words for Image Classification ##
Reference: Sivic, "_Video Google: A Text Retrieval Approach to Object Matching in Videos_"**, ICCV 2003.**

### Demo Video (2010/06/17) ###
<a href='http://www.youtube.com/watch?feature=player_embedded&v=eZwEexnynnQ' target='_blank'><img src='http://img.youtube.com/vi/eZwEexnynnQ/0.jpg' width='640' height=480 /></a>

### Dataset ###
Subset of Caltech 101 with my modifications, consisting of 4 object classes:
  * airplanes (Airplane): 800 (720 + 80) images
  * Faces\_easy (Face): 435 (391 + 44) images
  * Motorbikes (Motorbike): 797 (717 + 80) images
  * BACKGROUND\_Google (None): 368 (331 + 37) images
The dataset is available for download at [Google code](http://candy2009.googlecode.com/files/PR10_dataset_20100616.7z) or [OZ FTP server](http://oz.nthu.edu.tw/~u9562171/research/PR_dataset_20100616.7z).

# 2009年大學部專題：本計畫獲98學年度國科會「大專生參與專題研究計畫」核定通過 #
# Also used for Fall 2009: Statistical Learning Theory - Final Project #

## Title: Visual-Aided Outdoor Guidance for the Blind (戶外視覺輔助系統) ##

摘要：<br>
本計畫的目的是透過電腦視覺及影像處理相關技術，協助校園內的視障師生，使他們在日常活動及行走時，更能辨別道路上的建築、標線、障礙物，協助他們建立方向感，及應付可能遇到的一些突發狀況。計畫後期專注在*車輛偵測<b>。</b><br>

關鍵字：視覺輔助，機器學習方法，物件偵測。<br>

Keywords: computer vision, machine learning, pattern recognition,  car detection, visual aid, image processing.<br>
<br>
Primary goal: Car detection.<br>
<br>
Primary reference paper: Chen and Chen, <b>"Fast human detection using a novel boosted cascading structure with meta stages"</b>, in <i>IEEE Transactions on Image Processing</i>, 2008.<br>
<br>
<br>
<h2>Demo Video (2010/03/30)</h2>
<a href='http://www.youtube.com/watch?feature=player_embedded&v=kaWwFFVFguo' target='_blank'><img src='http://img.youtube.com/vi/kaWwFFVFguo/0.jpg' width='640' height=480 /></a><br>
<br>
<h1>Dataset</h1>
This project uses the <b>"NTHU campus data set"</b> of car/non-car images taken by myself. It consists of:<br>
<br>
Training data: 300 positive images and 9000 negative images.<br>
Testing data: 42 positive images and 355 negative images.<br>
The 9335 negative samples are generated from 199 negative images using my "gen_neg" program.<br>
<br>
The dataset archive is available at the <a href='http://oz.nthu.edu.tw/~u9562171/proj/NTHU_data_20100330.zip'>OZ ftp server</a> (fast speed) or at the <a href='http://code.google.com/p/candy2009/downloads/list'>Downloads</a> page (slow speed).<br>


<h1>Tested Programming Environment (Not necessarily the System Requirements)</h1>
<h2>Mac</h2>
Mac executables are available at the <a href='http://code.google.com/p/candy2009/downloads/list'>Downloads</a> page.<br>

<h3>New library since <a href='https://code.google.com/p/candy2009/source/detail?r=176'>r176</a> (QTKit + Cocoa, 64-bit)</h3>
Library: OpenCV 2.1.0 <a href='https://code.google.com/p/candy2009/source/detail?r=2968'>revision 2968</a>, C interface, built using the CMake build system.<br>
IDE: Xcode 3.2.1<br>
OS: Mac OS X Snow Leopard 10.6.3<br>
Notice:<br>
<ol><li>Follow method 2 of the <a href='http://opencv.willowgarage.com/wiki/Mac_OS_X_OpenCV_Port'>OpenCV Wiki</a>.<br>
</li><li>Need to add the 5 .dylib in /usr/local/lib as a Group to the Xcode project: libcxcore, libcvaux, libcv, libhighgui, libml.<br>
</li><li>Header Search Path: /usr/local/include/opencv<br>
</li><li>On Snow leopard, the project should be built as 64-bit (x86_64).</li></ol>

<h3>Old library before <a href='https://code.google.com/p/candy2009/source/detail?r=176'>r176</a> (Quicktime + Carbon, 32-bit)</h3>
Library: OpenCV 2.0.0 <a href='https://code.google.com/p/candy2009/source/detail?r=2492'>revision 2492</a>, C interface, built as an Xcode Framework.<br>
IDE: Xcode 3.2.1<br>
OS: Mac OS X Snow Leopard 10.6.3.<br>
Notice:<br>
<ol><li>Follow method 3 of the <a href='http://opencv.willowgarage.com/wiki/Mac_OS_X_OpenCV_Port'>OpenCV Wiki</a>, or download from the <a href='http://candy2009.googlecode.com/files/OpenCV.framework.zip'>site</a> (deprecated).<br>
</li><li>Put OpenCV.framework in /Library/Frameworks/, and add it to the Xcode project (Add -> Existing Frameworks).<br>
</li><li>Framework Search Path: /Library/Frameworks/OpenCV.framework/<br>
</li><li>Header Search Path: /Library/Frameworks/OpenCV.framework/Headers/<br>
</li><li>On Snow leopard, the project should be built as 32-bit (i386).<br></li></ol>


<h3>Windows</h3>
Windows executables are available at the <a href='http://code.google.com/p/candy2009/downloads/list'>Downloads</a> page.<br>

Library: OpenCV 2.1.0 <a href='https://code.google.com/p/candy2009/source/detail?r=2978'>revision 2978</a>, C interface, built using the CMake GUI, generating a MinGW Makefile.<br>
IDE: Code::Blocks nightly build, 2010-02-27 (<a href='https://code.google.com/p/candy2009/source/detail?r=6181'>revision 6181</a>)<br>
OS: Windows 7 (32-bit)<br>
Notice:<br>
<ol><li>Use TDM/MinGW instead of the official MinGW. The former has gcc 4.4.1 while the latter has gcc 3.4.x, which fails to build OpenCV.<br>
</li><li>Search Directories -> Compiler: C:\OpenCV\include\opencv, ..\..\include.<br>
</li><li>Linker settings -> Link libraries: Select the 5 lib210.dll.a (cxcore, cvaux, cv, highgui, ml) files. Not sure about the ffmpeg one.<br>
</li><li>Add to $PATH C:\OpenCV\bin so that the DLL files are accessible at runtime.<br>
</li><li>Due to the difference between the OpenCV installed paths, the CB project file for Windows cannot be directly used on Linux.</li></ol>


<h3>Linux</h3>
Not yet.