
#ifndef jackcasey067_FAST_IO_H
#define jackcasey067_FAST_IO_H


namespace Util {
    /* Fast IO */

    /* Should be called before any IO occurs: Sets flags to make IO faster for 
     * competitive programming. Disables synchronization between C++ and C io 
     * streams, and disables the flushing of cout before cin gets an input. 
     * Neither matter in a competive programming context. */
    void set_fast_IO();
}

#endif /* jackcasey067_FAST_IO_H */
