Andrew Kwang
CMPE 13
7 Februaray 2018

=======
Lab 4
=======

This is a solo project by yours truly. No collaboration was involved
because I have no friends in this class.

I thought the best way to approach this lab was by starting with the
Stack class, naturally. This wasn't too difficult thanks to the lovely
class which is CS 12B. I finished the Stack class and got to the real
meat of this assignment--rpm.c. Here, I thought the best approach was
to seperate a section into two sections--digits and operands. Two fairly
sizeable if statements seperate the two sections, both which detect if the
key entered is either an operand or a digit. Unfortunately, this code is 
a massive mess. I hope I'll still get some points.

The things that went wrong was firstly, I had no clue where the error was
coming from thanks to a lovely serial port error. After being told it was a
segmentation fault, I still couldn't find the source of the error.

The result of this lab was something that has an unworkable main and a
workable Stack function. Sigh. Maybe I should really work a lot earlier next
time. The lab manual regarding the order of pushing and popping depending on whether
you receive and operand or a digit was really helpful. Trying to figure out the
serial port error was extremely difficult.