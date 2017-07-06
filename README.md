# comp9319-17s1-Search-BWT-Encoded-File
C++ code for Assignment 2 of COMP9319-Web Data Compression &amp; Search



Specification:


Your task in this assignment is to create a search program that implements BWT backward search,  which can efficiently search a BWT encoded record file. The original file (before BWT) format is:     

[recordID1]Text1[recordID2]Text2[recordID3]Text3... ...    

where [recordID1], [recordID2], [recordID3], etc. are     integer values that are used as unique identifiers;    and Text1, Text2, Text3, etc. are text values,  which include any visible ASCII alphabets (i.e., any character with ASCII value from 32 to 126), tab (ASCII 9)  and newline (ASCII 10 and 13). 
For simplicity, there will be no open or close square bracket in the text values. 


Your C/C++ program, called  bwtsearch, accepts the path to a BWT encoded file; the path to an index file;  and one to three quoted query strings (i.e., search terms) as commandline input arguments. Each search term can be up to 256 characters.  Using the given query strings, it will perform backward search on the given BWT encoded file, and output all the records that contain ALL input query strings (i.e., a boolean AND search) to the standard output.   


To make the assignment easier, we assume that: (1) the search is case sensitive; (2) the output records do not need to be sorted; (3) duplicates are allowed if multiple matches are found in a record. Your output shall include the identifiers as well as the text values, one line  (ending with a '\n') for each match. 


If there are newlines in a text value, output them as is, and then ending the record with a '\n'.   If there are multiple matches in one record,  that record will be output multiple number of times (same number as the number of matches in that record, see an example below).   


Your solution is allowed to write out one external index file that is no larger than the size of the given, input BWT file. You only need to generate the index file when it does not exist yet. 




EXAMPLE:


        Suppose the original file (dummy.txt) before BWT is:  
        [3]Computers in industry[25]Data compression[33]Integration[40]Big data indexing
        (Note that you will not be given the original file.  You will only be provided with the BWT encoded file.)  

        Given the command: 
        %wagner> bwtsearch ~MyAccount/XYZ/dummy.bwt dummy.idx "in"

        The output should be: 
        [3]Computers in industry
        [3]Computers in industry
        [40]Big data indexing
        [40]Big data indexing
        
        Note that the order of the records can be different and the output is still correct, e.g., 
        [40]Big data indexing
        [3]Computers in industry
        [3]Computers in industry
        [40]Big data indexing


        Another example: 
        %wagner> bwtsearch ~MyAccount/XYZ/dummy.bwt dummy.idx "in "
        The output should be: 
        [3]Computers in industry

        And the last example: 
        %wagner> bwtsearch ~MyAccount/XYZ/dummy.bwt dummy.idx " in" "ata"
        The output should be: 
        [40]Big data indexing



Use the make command to compile solution.  A makefile is provided. The solution will be compiled and run on a typical CSE Linux machine.

Performance:

Your solution will be marked based on space and runtime performance. Your soluton will not  be tested against any BWT encoded files that are larger than 160MB.    Runtime memory is assumed to be always less than 10MB. Runtime memory consumption will be measured by valgrind massif with the option  --pages-as-heap=yes, i.e., all the memory used by your program will be measured.  Any solution that violates this memory requirement will receive zero points for that query test. Any solution that runs for more than 120 seconds on a machine with similar specification  as wagner for the first query on a given BWT file will be killed, and will receive zero points for the queries for that BWT file.   After that any solution that runs for more than 30 seconds for any one of the subsequent queries on that BWT file will be killed. 
