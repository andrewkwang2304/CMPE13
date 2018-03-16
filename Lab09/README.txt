Andrew Kwang
15 March 2018
CMPE 13

================================
            Lab 09
================================

Lab Partner: Kenneth Mai
Collaborators:

Field.c: Andrew Kwang
Protocol.c: Kenneth Mai

The most important part of this lab was working with 2 PIC32s to talk with
each other over the UART. There was also working with random numbers, something
which I've had little experience with. Working with another person on a long
CE assignment also gave me perspective on how these assignments should be done.
A lot of hardship and struggling came out of this assignment, but I think it
was worth it.

I started by reading the entire lab manual, front to back, making sure I didn't
miss anything important. It was still challenging trying to figure out how to
start, but my partner and I decided to divide and conquer. My partner would
handle Protocol.c and I would handle Field.c.

Field.c was easy until I had to check for boundaries as well as figuring out
why there were only 3 or 8 boats on the field at a given time. Ultimately, I
realized I wasn't clearing the board in FieldInit(), and that fixed the 3-8
boats on the Field. For checking boundaries, I ended up creating a seperate
helper function to handle the direction.

It was complicated, trying to explain Field to my partner, and vice versa with
Protocol. Then came implementing ArtificialAgent.c. I did the first half of the
state machine in AgentRun(), and my partner did the latter half. Ultimately, it
was a bit messy, but we completed the work.

The point distribution is fair, and I learned a lot from this lab. It's a very
valuable lab.

