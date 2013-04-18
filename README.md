#svi - a stream editor

#### NOTE: It has come to my attention that some people (a lot, actually) are submitting this code for an assignment in a class. I can understand that, as my code is awesome, and I hope that it can serve you as an example, but DO NOT use my code to submit it as your own. It is not your own. It is mine. And you will probably get a zero and be considered a cheater. Please keep that in mind.

##What is svi
It modifies input according to a list of edit commands that are given in a file specified as the command line argument. The edited standard input is output to standard output. Each line of input, and output after editing, will be maximally 256 characters long.<br>
<em>Similar to the `sed` command found on UNIX machines</em>

##Commands
Each edit command consists of an optional line range specification, an edit operation specifier, and data required by the edit operation.

###Three types of line range specifications
<ul>
	<li><strong>/&lt;text&gt;/</strong>-This format specifies all lines that contain the &lt;text&gt;. The &lt;text&gt; can be maximally 80 characters.</li>
	<li><strong>&lt;1st line number&gt;,&lt;last line number&gt;/</strong>-This format specifies all lines in the range &lt;1st line number&gt; to &lt;last line number&gt;, inclusive. Lines in the input file are considered to be numbered from 1, and this numbering is not changed in the editing process.</li>
	<li><strong>No line range specification specifies all lines in the file.</strong></li>
</ul>

###There are five edit operations
<strong>A&lt;text&gt;</strong>
<blockquote>
Appends the &lt;text&gt; at the end of the line. For example:
Ahello jim 
appends hello jim to all lines in the file. The &lt;text&gt; can be maximally 80 characters.
</blockquote>
<strong>I&lt;text&gt;</strong>
<blockquote>
Inserts the &lt;text&gt; at the start of the line. For example:
/blah/Ineedle noddle noo 
inserts needle noddle noo at the start of all lines that contain blah. The &lt;text&gt; can be maximally 80 characters.
</blockquote>
<strong>O&lt;text&gt;</strong>
<blockquote>
Inserts the &lt;text&gt; on a new line before the current line. For example:
1,1/OThe Title 
Inserts a new line before the first line in the file, containing the text The Title. The &lt;text&gt; can be maximally 80 characters. The new line is not put through the editing process.
</blockquote>
<strong>d</strong>
<blockquote>
Deletes the line from the file. For example:
3,6/d 
deletes lines 3 to 6 inclusive.
</blockquote>
<strong>s/&lt;old text&gt;/&lt;new text&gt;/</strong>
<blockquote>
Replaces the first occurence of &lt;old text&gt;, in the line, with &lt;new text&gt;. For example:
/filename/s/.pas/.c/ 
replaces the first occurrence of .pas with .c, in all lines containing filename. The &lt;old text&gt; and &lt;new text&gt; can be maximally 80 characters.
</blockquote>

##Install
svi uses autotools, you only have to run
<pre>make</pre>

##Running svi
####Here's an example run
#####Sample commands file `commands.txt`
<pre>
/Never done/I---------------------------------------
1,3/IPrepended to 1,2 and 3 :
1,1/OThis must appear as the first line
A : Appended to all
/line for substitution/s/This is one/This has been substituted on a/
9,10/d
/deleted/IThis should not appear
/Never done/I---------------------------------------
</pre>

#####Sample Input file `example.txt`
<pre>
1 To have text prepended
2 To have text prepended
3 To have text prepended
4 Unmodified except appended text
5 Unmodified except appended text
6 Unmodified except appended text
7 This is one line for substitution
8 This is one line for substitution
9 This is to be deleted
10 This is to be deleted
11 The last line of the file
</pre>

#####Run
<pre>./svi commands.txt &lt; example.txt</pre>

#####Sample Output
<pre>
This must appear as the first line
Prepended to 1,2 and 3 :1 To have text prepended : Appended to all
Prepended to 1,2 and 3 :2 To have text prepended : Appended to all
Prepended to 1,2 and 3 :3 To have text prepended : Appended to all
4 Unmodified except appended text : Appended to all
5 Unmodified except appended text : Appended to all
6 Unmodified except appended text : Appended to all
7 This has been substituted on a line for substitution : Appended to all
8 This has been substituted on a line for substitution : Appended to all
11 The last line of the file : Appended to all
</pre>
