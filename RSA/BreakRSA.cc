

#include <iostream>

#include "RSA_Algorithm.h"

using namespace std;

int main(int argc, char** argv)
{ // Arguments are as follows:
  //argv[1] = n;
  //argv[2] = e;  // n and e are the public key
  //argv[3] = first 6 characters of encrypted message
  //argv[4] = next 6 characters .. up to argv[12] are the lsat 6 characters
  // The number of arguments will always be exacty 12, and each argument past the
  // public key contain 6 ascii characters of the encrypted message.
  // Each of the 32bit values in the argv[] array are right justified in the
  // low order 48 bits of each unsigned long.  The upper 16 bits are always
  // zero, which insures the each unsigned long is < n (64 bits) and therefore
  // the RSA encryption will work.

  // Below is an example of the BreakRSA and command line arguments:

// ./BreakRSA  2966772883822367927 2642027824495698257  817537070500556663 1328829247235192134
// 1451942276855579785 2150743175814047358 72488230455769594 1989174916172335943 962538406513796755
// 1069665942590443121 72678741742252898 1379869649761557209

//   The corect output from the above is:
//   HelloTest  riley CekwkABRIZFlqmWTanyXLogFgBUENvzwHpEHRCZIKRZ
//
//   The broken (computed) private key for the above is 4105243553



  // Our one and only RSA_Algorithm object
  RSA_Algorithm rsa;

  mpz_class p1, q1, phi1;
  mpz_class e1,d1;

  // First "break" the keys by factoring n and computing d
  // Set the keys in the rsa object afer calculating d
  rsa.n = mpz_class(argv[1]);
//  cout<<"n: "<<rsa.n<<endl;
  rsa.e = mpz_class(argv[2]);
  e1 = rsa.e;
  //rsa.PrintNE();

  p1 = rsa.RhoFactor(rsa.n);
  q1 = rsa.n/p1;

  //cout<<"P1: "<<p1<<endl;
  //cout<<"Q1: "<<q1<<endl;

  phi1 = (p1- 1) * (q1 - 1); // New value of phi1
  //cout<<"phi1 "<<phi1<<endl;

  mpz_invert(d1.get_mpz_t() , e1.get_mpz_t() , phi1.get_mpz_t());

  // Set rsa.d to the calculated private key d
  rsa.d = d1;
  //rsa.PrintNDE();

  for (int i = 3; i < 13; ++i)
    { // Decrypt each set of 6 characters
      char ch;
      unsigned long ul1,ul2;
      mpz_class c(argv[i]);
      mpz_class m = rsa.Decrypt(c);
      //  use the get_ui() method in mpz_class to get the lower 48 bits of the m
      unsigned long ul = m.get_ui();

      int dec = 48;

      for(int j = 0; j < 7; j++)
      {
        ul1 = ul >> dec;
        ul2 = ul1 & 0xFF;
        ch = (char)ul2;
        //cout<<"print: "<<ch<<endl;
        cout<<ch;
        dec = dec - 8;
      }
      // Now print the 6 ascii values in variable ul.  As stated above the 6 characters
      // are in the low order 48 bits of ui.
      //cout<<"ul for: "<<i<<"--- "<<ul<<endl;
    }

  cout << endl;
}
