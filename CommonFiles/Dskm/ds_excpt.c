#include "dskmi.h"

#if defined( DSKM_EXCEPTION_ENABLE )

// from winnt.h
#define STATUS_WAIT_0                    ((unsigned long)0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((unsigned long)0x00000080L)    
#define STATUS_USER_APC                  ((unsigned long)0x000000C0L)    
#define STATUS_TIMEOUT                   ((unsigned long)0x00000102L)    
#define STATUS_PENDING                   ((unsigned long)0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((unsigned long)0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((unsigned long)0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((unsigned long)0x80000002L)    
#define STATUS_BREAKPOINT                ((unsigned long)0x80000003L)    
#define STATUS_SINGLE_STEP               ((unsigned long)0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((unsigned long)0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((unsigned long)0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((unsigned long)0xC0000008L)    
#define STATUS_NO_MEMORY                 ((unsigned long)0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((unsigned long)0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((unsigned long)0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((unsigned long)0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((unsigned long)0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((unsigned long)0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((unsigned long)0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((unsigned long)0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((unsigned long)0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((unsigned long)0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((unsigned long)0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((unsigned long)0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((unsigned long)0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((unsigned long)0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((unsigned long)0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((unsigned long)0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((unsigned long)0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((unsigned long)0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((unsigned long)0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((unsigned long)0xC00002C0L)     

// from winbase.h
#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE


#define EXCEPTION_EXECUTE_HANDLER       1
#define EXCEPTION_CONTINUE_SEARCH       0
#define EXCEPTION_CONTINUE_EXECUTION    -1



// ---
unsigned long DSKM_ProcessException( unsigned long err_code ) {
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


#endif  // defined( EXCEPTION_ENABLE )


