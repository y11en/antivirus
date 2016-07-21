#include <string.h>
#include "uuid.h"

namespace PragueRPC {

//   bool operator == ( const UUID& aLeftUUID, const UUID& aRightUUID )
//   {
//     if ( ( aLeftUUID.time_low == aRightUUID.time_low ) &&
//          ( aLeftUUID.time_mid == aRightUUID.time_mid ) &&
//          ( aLeftUUID.time_hi_and_version == aRightUUID.time_hi_and_version ) &&
//          ( aLeftUUID.clock_seq_hi_and_reserved == aRightUUID.clock_seq_hi_and_reserved ) &&
//          ( aLeftUUID.clock_seq_low == aRightUUID.clock_seq_low ) &&
//          ( aLeftUUID.node [ 0 ] == aRightUUID.node [ 0 ] ) &&
//          ( aLeftUUID.node [ 1 ] == aRightUUID.node [ 1 ] ) &&
//          ( aLeftUUID.node [ 2 ] == aRightUUID.node [ 2 ] ) &&
//          ( aLeftUUID.node [ 3 ] == aRightUUID.node [ 3 ] ) &&
//          ( aLeftUUID.node [ 4 ] == aRightUUID.node [ 4 ] ) &&
//          ( aLeftUUID.node [ 5 ] == aRightUUID.node [ 5 ] ) )
//       return true;
//     return false;
//   }
  
//   bool operator != ( const UUID& aLeftUUID, const UUID& aRightUUID )
//   {
//     return !( aLeftUUID == aRightUUID );
//   }
  
//   bool operator < ( const UUID& aLeftUUID, const UUID& aRightUUID )
//   {
//     if ( ( aLeftUUID.time_low < aRightUUID.time_low ) &&
//          ( aLeftUUID.time_mid < aRightUUID.time_mid ) &&
//          ( aLeftUUID.time_hi_and_version < aRightUUID.time_hi_and_version ) &&
//          ( aLeftUUID.clock_seq_hi_and_reserved < aRightUUID.clock_seq_hi_and_reserved ) &&
//          ( aLeftUUID.clock_seq_low < aRightUUID.clock_seq_low ) &&
//          ( aLeftUUID.node [ 0 ] < aRightUUID.node [ 0 ] ) &&
//          ( aLeftUUID.node [ 1 ] < aRightUUID.node [ 1 ] ) &&
//          ( aLeftUUID.node [ 2 ] < aRightUUID.node [ 2 ] ) &&
//          ( aLeftUUID.node [ 3 ] < aRightUUID.node [ 3 ] ) &&
//          ( aLeftUUID.node [ 4 ] < aRightUUID.node [ 4 ] ) &&
//          ( aLeftUUID.node [ 5 ] < aRightUUID.node [ 5 ] ) )
//       return true;
//     return false;
//   }
  
//   bool operator > ( const UUID& aLeftUUID, const UUID& aRightUUID )
//   {
//     return ( aRightUUID < aLeftUUID );
//   }

  bool UUID::operator == ( const UUID& anUUID ) const
  {
    return memcmp ( this, &anUUID, sizeof ( UUID ) ) == 0;
  }
  
  bool UUID::operator != ( const UUID& anUUID ) const
  {
    return memcmp ( this, &anUUID, sizeof ( UUID ) ) != 0;
  }
  
  bool UUID::operator < ( const UUID& anUUID ) const
  {
    return memcmp ( this, &anUUID, sizeof ( UUID ) ) < 0;    
  }
  
  bool UUID::operator > ( const UUID& anUUID ) const
  {
    return memcmp ( this, &anUUID, sizeof ( UUID ) ) > 0;
  }
}
