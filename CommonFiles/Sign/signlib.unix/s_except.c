#include "signport.h"
#include <Sign/Sign_lib.h>

#if defined( _WIN32 ) && defined( EXCEPTION_ENABLE )

#include <windows.h>

// EXCEPTION_CONTINUE_SEARCH 
// EXCEPTION_CONTINUE_EXECUTION 
// EXCEPTION_EXECUTE_HANDLER 
// ---
unsigned long process_sign_exception( unsigned long err_code ) {
  switch( err_code ) {
    case EXCEPTION_ACCESS_VIOLATION :       // The thread attempted to read from or write to a virtual address for which it does not have the appropriate access. 
      return EXCEPTION_EXECUTE_HANDLER;
    case EXCEPTION_BREAKPOINT :             // A breakpoint was encountered. 
    case EXCEPTION_DATATYPE_MISALIGNMENT :  // The thread attempted to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries, 32-bit values on 4-byte boundaries, and so on. 
    case EXCEPTION_SINGLE_STEP :            // A trace trap or other single-instruction mechanism signaled that one instruction has been executed. 
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED :  // The thread attempted to access an array element that is out of bounds, and the underlying hardware supports bounds checking. 
    case EXCEPTION_FLT_DENORMAL_OPERAND :   // One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value. 
    case EXCEPTION_FLT_DIVIDE_BY_ZERO :     // The thread attempted to divide a floating-point value by a floating-point divisor of zero. 
    case EXCEPTION_FLT_INEXACT_RESULT :     // The result of a floating-point operation cannot be represented exactly as a decimal fraction. 
    case EXCEPTION_FLT_INVALID_OPERATION :  // This exception represents any floating-point exception not included in this list. 
    case EXCEPTION_FLT_OVERFLOW :           // The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type. 
    case EXCEPTION_FLT_STACK_CHECK :        // The stack overflowed or underflowed as the result of a floating-point operation. 
    case EXCEPTION_FLT_UNDERFLOW :          // The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type. 
    case EXCEPTION_INT_DIVIDE_BY_ZERO :     // The thread attempted to divide an integer value by an integer divisor of zero. 
    case EXCEPTION_INT_OVERFLOW :           // The result of an integer operation caused a carry out of the most significant bit of the result. 
    case EXCEPTION_PRIV_INSTRUCTION :       // The thread attempted to execute an instruction whose operation is not allowed in the current machine mode. 
    case EXCEPTION_NONCONTINUABLE_EXCEPTION : // The thread attempted to continue execution after a noncontinuable exception occurred. 
    default :
      return EXCEPTION_CONTINUE_SEARCH;
  }
}


#endif



