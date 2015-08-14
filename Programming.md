# Introduction #

多人合作程式，難免要訂一些共同遵守的規則。<br>
我先起個頭，如果有看不懂或是要上訴補充的可以留comment再修改。<br>
<br>
<h1>Details</h1>

<ol><li>不論Win/Mac，編碼統一用 <font color='red'><b>UTF-8</b> </font>，以免中文註解到別台電腦上變亂碼。換行符號麻煩配合Mac上的<font color='red'> <b>LF</b> </font>，否則vim的行尾會顯示^M的怪符號。<br>
<ol><li>不要使用Dev-C++，因為不支援UTF-8編碼。純編輯器可以用NotePad++或UltraEdit之類的軟體，而免費IDE可以用Windows、Mac、Linux版本都有的<a href='http://www.codeblocks.org/'>Code::Blocks (簡稱CB)</a>，雖然只有英文版但比Dev-C++還強，而且Dev-C++很久沒有更新了。八卦是，CB開發團隊裡甚至有前Dev-C++的成員:p。<br>
</li><li>舉Code::Blocks為例。如果要修改預設編碼，可以在<font color='green'>Settings → Editor → End-of-Line Mode </font>，改成 <b>UTF-8</b> ，平常編輯時也檢查畫面底下的編碼正不正確。字型想改的話也是在設定的同一頁。<br>
</li></ol></li><li>檔案標頭註明檔名、作者、建立日期、修改日期、摘要。<br>
</li><li>prototype(.h)與implementation(.c)分開，組件測試時使用<font color='purple'>#define DEBUG 1</font>，並用<font color='purple'>#if DEBUG，#endif</font>包著組件中的main。<br>
</li><li>include問題，XXX.c只會include XXX.h；XXX.h可能會include其他的YYY.h。util.h會include所有需要的C library，所以任何include util.h的XXX.h，都不需要include任何C library。也就是說，很多瑣碎的事可以丟給util.'<b>'來做。<br>
</li><li>縮排用tab(編輯器預設是4格)，程式碼請多空格、空行，不要擠在一起，例如：<br>
<pre><code>   a = b + c;<br>
   array[ functionA( functionB( param ) ) ];<br>
   for ( int i = 1; i &lt;= n; i++ ) { ... }<br>
   void func( int a, int b ) { ... }<br>
</code></pre>
</li><li>錯誤訊息用util.'</b>'裡面的error( "function(): message." );。例如：<br>
<pre><code>   error( "m_mul(): Demensions disagree." );<br>
</code></pre>
</li><li>struct的第一字母一律大寫。<br>
</li><li>C語言是row major，盡量遵守否則效能差很多。也就是要跑3D的迴圈時，可以的話盡量照以下來跑，效能才不會差：<br>
<pre><code>   for 第一個維度 (layer) {<br>
       for 第二個維度 (row) {<br>
           for 第三個維度 (col) {<br>
               // ....<br>
           }<br>
       }<br>
   }<br>
</code></pre>