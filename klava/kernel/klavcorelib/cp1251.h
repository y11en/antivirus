// cp1251.h
//
//

#ifndef cp1251_h_INCLUDED
#define cp1251_h_INCLUDED

#define CP1251_NO_MAPPING 0

// map: CP1251->UNICODE, returns 0 if no mapping
unsigned int cp1251_map (unsigned char c);

// map: UNICODE->CP1251, returns 0 if no mapping
unsigned int cp1251_rmap (unsigned int uc);


#endif // cp1251_h_INCLUDED

