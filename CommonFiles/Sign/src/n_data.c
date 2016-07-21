#include <Sign/Sign_lib.h>

#if defined( N_PLAT_NLM )
  
  char	szKaspUser[] = "Kaspersky Lab";              
  char	szKaspUserSN[] = "Kaspersky Lab SK 000"; 

  ROTATE_FUNCTION_POINTER RotatePtr = 0;

  void InitRotatePtr(ROTATE_FUNCTION_POINTER aPtr) {
    RotatePtr = aPtr;
  }

#endif