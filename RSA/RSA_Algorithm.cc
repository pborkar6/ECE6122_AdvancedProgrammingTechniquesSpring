

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "RSA_Algorithm.h"

using namespace std;

mpz_class p,q;
// Implement the RSA_Algorithm methods here

// Constructor
RSA_Algorithm::RSA_Algorithm()
  : rng(gmp_randinit_default)
{
  // get a random seed for the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random, exiting" << endl;
      exit(0);
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;
  rng.seed(drValue);
// No need to init n, d, or e.
}

// void GenerateRandomKeyPair(size_t sz);

void RSA_Algorithm::GenerateRandomKeyPair(size_t sz)
{
	mpz_class p;

	mpz_class q;
	int pp,qq;

	p=rng.get_z_bits(sz);
	pp=mpz_probab_prime_p(p.get_mpz_t(), 100);
	
	while(1)
	{
		if(pp!=1 )
		{
			p=rng.get_z_bits(sz);
			pp=mpz_probab_prime_p(p.get_mpz_t(), 100);
				
		}
		if(pp==1 ) 
			break;

	}
	q=rng.get_z_bits(sz);
	qq=mpz_probab_prime_p(q.get_mpz_t(), 100);
	while(1)
	{
		if(qq!=1)
		{
			q=rng.get_z_bits(sz);
			qq=mpz_probab_prime_p(q.get_mpz_t(), 100);
	
		}
		if( qq==1) 
			break;

	}

	mpz_mul(n.get_mpz_t(),p.get_mpz_t(),q.get_mpz_t()) ;
	mpz_class phi;
	mpz_class temp1,temp2;
	mpz_class one;
	 mpz_set_ui(one.get_mpz_t(),1);
	mpz_sub(temp1.get_mpz_t(),p.get_mpz_t(),one.get_mpz_t());
	mpz_sub(temp2.get_mpz_t(),q.get_mpz_t(),one.get_mpz_t());
	mpz_mul(phi.get_mpz_t(),temp1.get_mpz_t(),temp2.get_mpz_t()) ;
	
	mpz_class rop;
	while(1)
	{
		
		d=rng.get_z_bits(sz*2);
		mpz_gcd(rop.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
		if(d<phi && rop==1)
		break;
	}
		
		int f= mpz_invert (e.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
	PrintNDE(); 


		
  
}

mpz_class RSA_Algorithm::Encrypt(mpz_class M)
{
	mpz_class cipher;

	mpz_powm( cipher.get_mpz_t(),  M.get_mpz_t(), e.get_mpz_t(),  n.get_mpz_t());
	PrintC(cipher);
	return cipher;
}

mpz_class RSA_Algorithm::Decrypt(mpz_class C)
{
	mpz_class rec;

	mpz_powm (rec.get_mpz_t(),  C.get_mpz_t(),  d.get_mpz_t(),  n.get_mpz_t());
	return rec;
}


mpz_class RSA_Algorithm::RhoFactor(mpz_class N)
{
   
    mpz_class x = 2;
    mpz_class y = 2;
    mpz_class d = 1;

    while(d == 1)
    {
      mpz_class g_y; 
      mpz_class abs_val;
      x = ((x * x) + 1) % N;
      g_y = ((y * y) + 1) % N;
      y = ((g_y * g_y) + 1) % N;

      if(y > x)
        abs_val = y - x;
      else
        abs_val = x - y;

      
      mpz_gcd( d.get_mpz_t() , abs_val.get_mpz_t() , N.get_mpz_t());

    }

    return d;
}


// Fill in the remainder of the RSA_Algorithm methods
// void computePrime(mpz_class& x, size_t);
// --> function to select two distinct prime numbers

void RSA_Algorithm::computePrime(mpz_class& x, size_t sz)
{
  bool flag = false;

  while(flag!=true)
  {
    x = rng.get_z_bits(sz);
    if(mpz_probab_prime_p(x.get_mpz_t(),100) == 0) // 100 iterations
      flag = false;
    else
      flag = true;
  }
}

void RSA_Algorithm::PrintND()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << endl;
}

void RSA_Algorithm::PrintNE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " e " << e << endl;
}

void RSA_Algorithm::PrintNDE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << " e " << e << endl;
}

void RSA_Algorithm::PrintM(mpz_class M)
{ // Do not change this, right format for the grading script
  cout << "M " << M << endl;
}

void RSA_Algorithm::PrintC(mpz_class C)
{ // Do not change this, right format for the grading script
  cout << "C " << C << endl;
}
