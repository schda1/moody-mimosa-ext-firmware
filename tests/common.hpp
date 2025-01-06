/* Resolving conflicts between operator overloading of 
 * Memory leak detection and new/delete from STL containers. 
 * In order to resolve those, the STL macros have to be 
 * included before memory leak detector macros are defined. 
 * This issue is also mentioned in the CppUTest documentation,
 * http://cpputest.github.io/manual.html#memory_leak_detection
 */
#include <functional>
#include <CppUTest/MemoryLeakDetectorNewMacros.h>
#include <mock/stm32g4xx_hal.h>