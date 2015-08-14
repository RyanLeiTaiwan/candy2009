# Introduction #

之前一直擔心合作程式與版本控制的問題，後來決定使用Google Code + Subversion (svn)來實現。<br>
Google Code提供的平台很不錯，最近也蠻紅的；而svn這類的工具還蠻重要的，最好能把它練會。<br>
<br>
<h1>網路上的基礎文章</h1>

<ol><li><a href='http://blog.ericsk.org/archives/446'>用 Subversion 跟 Google Code 作版本控制 （一）→ 圖文教學</a>
</li><li><a href='http://plog.longwin.com.tw/my_note-programming/2007/07/10/svn_tutorial_2007'>SVN基本指令教學</a>
</li><li><a href='http://blog.linux.org.tw/~jserv/archives/001148.html'>淺談版本控制系統</a>
</li><li><a href='http://svnbook.red-bean.com/nightly/zh/svn-book.html'>Subversion Book(完整中文手冊)</a></li></ol>

<h1>之前在專題板寫的教學文章( 可以對照"基礎文章1."來看 )</h1>

<h2>1.Google Code平台的部份</h2>
目前我開放的分頁是預設的Home, Downloads, Wiki, Issues, Source, Administer。<br>
<h3>Home</h3>
Home除了可以把外觀佈置漂亮一點之外XD，還可以從Updates查看最近的更新。<br>
<h3>Downloads</h3>
Downloads會放完成品這類的東西，應該是project後期才會有完成品，平常翻到的機會不大。<br>
<h3>Wiki</h3>
Wiki的用途除了把project的注意事項貼上去，還可以放各階段的references。<br>
<i>ryanlei:</i> 既然現在都移駕到Web了，應該會放更多東西在這裡XD。<br>
<h3>Issue</h3>
Issue是討論程式或是想法的相關議題，既然現在移駕到Web了，除了敲MSN，有些比較要記下來的issues可以放在這裡。<br>
<h3>Source</h3>
Source就是原始碼，這應該是跟大家最有關的。在這裡的Browse可以瀏覽所有的原始碼，甚至可以切換不同的*修訂版(revision)<b>。</b><br>
我們的project規模不大，用不到tags跟branches的目錄，所以程式碼都會放在*trunk(主幹)<b>這個資料夾，或是trunk底下建的目錄。</b><br>
<ul><li><b>Checkout</b> 是Google Code附的svn checkout語法，這在某電腦第一次從貯藏庫取回時用的到。<br>
</li><li><b>Browse</b> 是瀏覽程式碼，除了語法都會套色外，double click還可以留下評論。<br>
</li><li><b>Changes</b> 則是查看版本間的變化，不管是Browse或Change，都有辦法查看同一檔案在不同版本的差異，顏色也套得很好。可以摘要式、或是整頁展開顯示差異。<br>
<h3>Administer</h3>
Administer是project的管理頁面，大家都是project owner所以只要不要有太超過的舉動，要更動什麼應該都OK。</li></ul>

<h2>2.Subversion的部份</h2>
<h3>安裝</h3>
接來下就是真正版本控制的工作啦。Google Code平台支援的版本控制工具是<a href='http://subversion.tigris.org/'>subversion(svn)</a>，<br>
在Windows上可以去找圖形介面的<a href='http://tortoisesvn.net/'>TortoiseSVN</a>，或是文字模式的<a href='http://www.collab.net/downloads/subversion/'>CollabNet Subversion</a>。；<br>
Unix-like系統上也有圖形介面，不過身為unix-like user通常都會裝文字模式的套件，用終端機來操作。<br>
雖然GUI方便，不過說明手冊或是專業討論通常都是用指令來表達，所以可以取捨一下要用哪一種。<br>
在官網上有針對各種系統的<a href='http://subversion.tigris.org/getting.html#binary-packages'>取得方法</a>：<br>
<br>
<h3>使用</h3>
以下說明都是以指令為主，如果是圖形介面點滑鼠應該更簡單，對照第一行那篇教學操作應該就沒啥問題了。<br>
只列出平常會用到的指令：<br>
<br>
<h4><font color='red'>從無到有 → svn checkout</font></h4>
遠端已經有了一個*貯藏庫(repository)<b>，現在要整個*取回(checkout)</b>到本地。這步只有在自己電腦上第一次，或是在系計中有還原卡時才需要做。<br>
Google Code的 Source->chekcout 也有教，指令是：<br>
<ul><li><i>svn checkout <a href='https://candy2009.googlecode.com/svn/trunk/'>https://candy2009.googlecode.com/svn/trunk/</a> candy2009 --username XXXXXX</i> <br>
整個是一行，做的事是把遠端repository的trunk資料夾放到本地端，本地端取名叫"candy2009"，本地資料夾名稱要改當然可以隨自己高興。<br>
指令後半段代表以XXXXXX的身分登入，要注意的是被問及密碼時，並不是用Google帳號的密碼，而是用Google Code幫你隨機產生的密碼，可以在Google Code畫面最上方的Profile->Settings裡查到自己的Google Code password，如果按了，"Regenerate"重設密碼，則往後用svn的指令也必須改密碼。<br>
另外，也有唯讀開放給其他人的anonymous取得方式，位址就變成是http而不是https，不過跟我們無關。</li></ul>

<h4><font color='red'>每次工作時的第一步 → svn update(up)</font></h4>
確定本地端保持在與遠端相同的最新版本，<b>每次工作之前一定要先做</b>。<br>
如果是第一次工作，因為之前已經checkout過一定是最新版，所以就不用update了。<br>
<br>
<h4><font color='red'>產生更動 → M:Modification, A:Add, D: Delete</font></h4>

<font color='red'>修改(M)現有檔案 → NOTHING</font><br>
純粹修改檔案不用特別幹麻。=> 通常是這樣<br>
<br>
<font color='red'>新增(A)檔案或目錄至repository -- svn add XXX</font><br>
新建檔案時往往要做這步，將其納入版本控制的範圍。=> 偶爾<br>
<br>
<font color='red'>從repository移除(D)檔案，並刪除本地端 -- svn delete(del) XXX</font><br>
有點詭異的是，之後上傳時會同時把本地端的該檔案刪除，有點危險請小心。<br>
<br>
<font color='red'>從repository移除(D)檔案，並保留本地端 -- svn del --keep-local XXX</font><br>
如果只是單純不想把某檔案放進repository，這樣比較正常，<br>
所以除非確定某檔案在project已經不需要，否則--keep-local比較安全。<br>
<br>
<font color='red'>本地和遠端複製某檔案 -- svn copy(cp) OLD NEW</font><br>
當於一般的複製一份 + svn add NEW<br>
<br>
<font color='red'>本地遠端重新命名某檔案 -- svn move(mv)</font><br>
相當於svn cp OLD NEW + svn delete OLD<br>
<br>
<font color='red'>檢視更動 → svn status(st)</font><br>
會列出本地與遠端檔案的更動，常見的符號有：<br>
<ul><li><b>M:修改 A:新增  D:刪除  ?:未納入管理  C:衝突</b><br>
上傳前_一定_要再做這步來檢查是否無衝突。</li></ul>

<h4><font color='red>解決衝突/其他版本問題</font'></h4>
感覺不太會發生，而且處理不太容易，碰到再說好了0.0。<br>
可能遇到的問題是，修改期間有人已經上傳，做某個更動想反悔，<br>
甚至是想把repository整個還原回某一個修訂版(revision)，可能會用到merge、revert這類的指令。<br>
<br>
<h4><font color='red'>上傳 → svn commit(ci)</font></h4>
如果幸運地在svn status這步沒有發現衝突，那就可以放心*上傳(commit)<b>了。<br>
commit指令下了以後，還需要編輯這次revision的*摘要(summary)</b>。<br>
如果是文字模式的環境，系統會開vi/vim，打完按:wq存檔以後就會進行上傳。<br>
圖形介面的TortoiseSVN也會冒出編輯log的視窗（好像還能選擇哪些要上傳），點OK以後也會開始動作。<br>
<h3><font color='red>每一次工作結束，不管進度如何都請務必commit，以免之後和別人發生程式衝突，也有利組員互相知道目前的進展。</font'>

<h3>結論</h3>
除了第一次需要<font color='red>*Checkout*</font'>，平常的工作流程大概像上面幾段這樣：<br>
<ul><li><h2><font color='red'><b>update → modification/add/delete → stat → resolve conflicts → commit</b>
</font></h2>
結束以後可以接著到Google Code上看一下結果。<br>
或是在步驟之間也可以配合Google Code來查看程式碼的狀況。<br>
如果有svn update過，平時也可以用svn info來查看目前本地端的修訂版。</li></ul>

Subversion的指令還有很多很多，有必要可以上網找，<br>
或是翻基礎文章的<a href='http://svnbook.red-bean.com/nightly/zh/svn-book.html'>Subversion Book</a>，我寫這篇的工作流程就是參考它的XD。