Andrew Kwang
Max Dunne
CMPE 13
22 February 2018

==============================
            Lab 6
==============================

Battleboats Partner: Kenneth Mai


I've never done more hardware coding in my life than this assignment.

That's probably a good thing. This was a great experience overall, and
is a great reminder to myself as to why I should go to Friday's section.
All in all, I learned quite a bit about how to work with hardware in
tandem with software. Great stuff, really. Part 3 was really hard and I
didn't get to finish it, but I still think the process was very helpful
to developing my programming skills on the PIC32.

I actually pulled up my old CMPE 12 assignment for the first part. We had
to make lights bounce and manipulate switches, but luckily for me, C is a
higher level programming language. I included a 'direction' flag and easily
implemented part 1 in a much smaller amount of lines. Essentially, part 1
broke down into two if-statements that checked the direction flag, then checked
whether the LED was at the border or not.

Part 2 wasn't too much trickier. I initialized in the same manner as I did in
part 1, but this time created a char array to contain the values of the string
I was to create. After I manipulated some numbers, I sent the results to the
Oled.

Part 3 was difficult enough that I couldn't finish it after a series of chronic
procrastination. ButtonsInit() was easy to implement, and ButtonsCheckEvents() was
implemented by bit AND-ing BUTTON_STATES() and BUTTON_STATE_X. Afterwards, I 
OR-equaled it to returnState, which would store the state of the buttons. Having
completed Buttons.c, I tried to perform part 3 to no avail. I created a seperate
function SetLED(int mask); to handle all the times I would have to toggle the LED
lights, but none of it ended up working. I followed the pseudocode with little success.

For the record, I am extremely grateful that you gave Part 3 the lowest amount of points.
That was a life-saver. Thank you so much. I'd appreciate if future grading standards were
like this.

Regarding criticism towards the lab manuals, I think the Buttons section needs a bit of
a rework. I had no clue what a sample was, and it meant nothing to me in the context of the
paragraph. The section TAs taught the buttons well, but the manual did not make clear
the concept of searching for the transition rather than the consistent stream of 1's or 0's.
I had no clue I had to create a history until the TA's explained it.

All in all, decent lab. I hope a get a better score.