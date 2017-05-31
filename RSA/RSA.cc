

#include <iostream>
#include "RSA_Algorithm.h"

using namespace std;

int main()
{
  // Instantiate the one and only RSA_Algorithm object
  RSA_Algorithm RSA;

  // Step 1: Loop from sz = 32 to 1024 inclusive
  // Step 2: for each size choose 10 different key pairs
  // Step 3: For each key pair choose 10 differnt plaintext
  // Step 4: messages making sure it is smaller than n.
  // Step 5: If not smaller then n then choose another
  // Step 6: For eacm message encrypt it using the public key (n,e).
  // step 7: After encryption, decrypt the ciphertext using the private
  // step 8: key (n,d) and verify it matches the original message.

  // your code here

  //Step 1
  for (size_t sz=32;sz<=1024;sz=sz*2)
{
	for(int j=0;j<10;j++)
	{
	RSA.GenerateRandomKeyPair(sz);
	mpz_class M;
	mpz_class C;
	mpz_class dd;
	for(int k=0;k<10;k++)
	{
		M=RSA.rng.get_z_bits(2*sz);
		
		while(1)
		{
			if(M<RSA.n)
			{break;}
			else
			{
			 M=RSA.rng.get_z_bits(2*sz);
			}
		}
	        RSA.PrintM(M);
		C=RSA.Encrypt( M);
		dd=RSA.Decrypt( C);
	

	}
	}

}
}