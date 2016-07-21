#if !defined (_UUID_H_)
#define _UUID_H_

namespace PragueRPC {

  struct UUID {
    unsigned int   time_low;
    unsigned short time_mid;
    unsigned short time_hi_and_version;
    unsigned char  clock_seq_hi_and_reserved;
    unsigned char  clock_seq_low;
    char           node [ 6 ];

    bool operator == ( const UUID& anUUID ) const;
    bool operator != ( const UUID& anUUID ) const;
    bool operator < ( const UUID& anUUID ) const;
    bool operator > ( const UUID& anUUID ) const;

  };

/*   bool operator == ( const UUID& aLeftUUID, const UUID& aRightUUID ); */
/*   bool operator != ( const UUID& aLeftUUID, const UUID& aRightUUID ); */
/*   bool operator < ( const UUID& aLeftUUID, const UUID& aRightUUID ); */
/*   bool operator > ( const UUID& aLeftUUID, const UUID& aRightUUID ); */
}
#endif //_UUID_H_
