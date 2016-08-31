#ifndef __TEST_CLIENT_H_
#define __TEST_CLIENT_H_

//! @file simply give a few nice functions that will allow the test to run smoothly. Simply implement void test_main() to begin the test ;)

//! implement this one to begin your test.
//! don't forget to set your global timeout duration (using setTimeout())
//! you may abort it earlier with terminate();
void test_main();
//! tell when the test should end
void setTimeout(double ms);
//! Quickly interrupt the service.
void terminate();





#endif // __TEST_CLIENT_H_