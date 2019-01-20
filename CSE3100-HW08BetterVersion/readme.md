# Notes

- life.c is the plain sequential code

- lifeMT.c uses a static row partitioning as well as POSIX conditions
  to synchronize the workers
  
- lifeMT1.c revisits lifeMT.c but uses a _barrier_. Note that the code
  has an implementation of barrier to make it work on macOS. The
  barrier implementation is from: 
  http://blog.albertarmea.com/post/47089939939/using-pthreadbarrier-on-mac-os-x 
  
- lifeMT2.c is the extra-credit solution. It uses dirty regions to
  break down the work. Regions are "strips". It could still be further
  improved to only mark neighboring regions as dirty when the
  _boundary_ of the region has changed (but we shouldn't care about
  the inside. 
  
  
Enjoy!
