Amit Mondal #804746916
CS35L HW 6

I multithreaded the tracing code by taking the code in the doubly-nested loop
that loops through each pixel and renders then outputs it and putting it in a
function called trace_ray. I then created the appropriate number of threads that
divided the image into columns of pixels and rendered each one separately.
Furthermore, because I couldn't directly output from the threads because they
might not be executing in the right order, I created a 3-D array to hold the
output (two dimensions for the pixel x/y, and the third dimension holds the RGB
float values), which is printed out after the threads are joined. I did run into
one major issue when I first rewrote main.c. My first strategy to divide the
image into columns was to go sequentially. For example, if there were four
threads, I would number each thread, and use that thread number to take the
appropriate quarter of the image in one big block. In that scenario, the first
thread would try to render a column of the image (n/4) pixels wide.
Unfortunately, this method was somewhat buggy and would routinely miss columns
because of some bad calculation. I decided on a much simpler method of simply
starting at the thread number and incrementing by the number of threads. This
way, I could be sure that I never miss any threads, unlike the block-processing
method used earlier. The results of the multithreaded version are just as
improved as expected. With each doubling of threads, the time gets halved, going
from 44s to 22s to 11s, and ending at 5s with 8 threads.
