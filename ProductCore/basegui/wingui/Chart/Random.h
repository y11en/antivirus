#if !defined(RNDGEN_H__ECEA7740)
#define RNDGEN_H__ECEA7740

/*################# Random Number Generator ##################################
Based on a portable subtractive pseudorandom number generator 
described in "The Art of Programming" Knuth 1981 */

class RndGen 
{
	int     n;  
	float   *oldrand;
	void    advance_random();

public:  
	RndGen() { oldrand = 0; }
	~RndGen();
	float   randm();        
	void    init_random(const float randomseed = 0.1f);
};

#endif //RNDGEN_H__ECEA7740