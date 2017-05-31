// Distributed two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include "Complex.h"
#include "InputImage.h"

using namespace std;

void Transform1D(Complex*, int, Complex*);
void InverseTransform1D(Complex*,int,Complex*);

void Transform2D(const char* inputFN) 
{ 
InputImage image(inputFN);  // Create the helper object for reading the image
int w=image.GetWidth();
int hh=image.GetHeight();
int numtasks,rank,tag;
 //read input
Complex* h =image.GetImageData();
 
 MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
 MPI_Comm_rank(MPI_COMM_WORLD,&rank);
Complex* H= new Complex[w*hh];

////////////////////////////////////  All ranks doing 1D transform /////////////////////////////////////////////////
int add;
int start=(hh/numtasks)*rank;  // 16*rank
for(int i=0;i<hh/numtasks;i++)
{		
		add=(w*start)+(i*w);
	//	cout << "offset for i" << i << "is " << off << endl;	
		Transform1D(h+add,w,H+add);
	//cout << "CPU" << rank << " sending 1D transform to CPU 0" << endl; 
}

 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// Rank 0 receiving ////////////////////////////////////////////////////////////////////

if(rank==0)
{
//cout << "entering rank 0 if loop " ;

	
	for (int j=1;j<numtasks;j++)
	{	
		//Complex r_buf[w*numtasks];
		MPI_Status status;
	//	cout << "entering for loop " << endl;
		
		//cout << "receiving from rank " << rank;
		
		
          	MPI_Recv(H+w*j*(hh/numtasks),w*(hh/numtasks)*sizeof(Complex), MPI_CHAR,j,0, MPI_COMM_WORLD, &status);
		

	}						
}

if(rank != 0)   //other CPUs send to CPU 0 
    {
     MPI_Send(H+start*w,hh*(w / numtasks) * sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
     // cout << "Rank " << rank << " Sending 1D DFT rows to CPU 0." << endl;
    }

if(rank == 0)
  {
    image.SaveImageData("MyAfter1D.txt",H, w, hh);
  }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////// Transposing H /////////////////////////////////////////////////////////////
//Complex* H_t= new Complex[w*hh];
//int  off;
int start1=(hh/numtasks)*rank;

if(rank==0)  // rank 0 transposes
{
//Complex temp1;
for(int p=0;p<hh;p++)
{
        for(int q=0;q<w;q++)
        {
                if(p>q)
                {
               Complex temp1=H[p+w*q];
                H[p+w*q]=H[q+w*p];

                H[q+w*p]=temp1;
                }
                //H_t[q*hh+p]= H[p*hh+q];
         }
//image.SaveImageData("TransposedH.txt",H,w,hh);
}


}

if(rank!=0) // all other ranks receive
{
		 MPI_Status status;
                   //cout << "receiving from rank " << rank;
                 MPI_Recv(H+start1*hh,(w/numtasks)*hh*sizeof(Complex), MPI_CHAR,0,0, MPI_COMM_WORLD, &status);
		 cout << "Rank" << status.MPI_SOURCE << "receiving from rank 0  " << endl; 
}


if(rank==0)
{
	 for(int i=1;i<numtasks;i++)
	{
 		MPI_Send(H+(w/numtasks)*i*hh,(hh/numtasks)*hh*sizeof(Complex), MPI_CHAR,i, 0, MPI_COMM_WORLD);
		cout << "Rank 0 sending transposed H to " << i << endl; 
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Complex* H1 = new Complex[w*hh];


int off1;

	for (int x=0;x<(hh/numtasks);x++)
	{
	off1=(start1+x)*hh;
	Transform1D(H+off1,hh,H1+off1);
	}	


if(rank==0)
{
//cout << "entering rank 0 if loop " ;


        for (int j=1;j<numtasks;j++)
                {
                   //Complex r_buf[w*numtasks];
                    MPI_Status status;
                   //cout << "receiving from rank " << rank;
                   MPI_Recv(H1+w*j*(hh/numtasks),w*(hh/numtasks)*sizeof(Complex), MPI_CHAR,j,0, MPI_COMM_WORLD, &status);	
		cout << "Rank 0 receiving from " << j<< endl ;
		}
}

if(rank!=0)
{
	 MPI_Send(H1+start1*w,hh*(w / numtasks) * sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);

     cout << "Rank " << rank << " Sending 1D DFT rows to CPU 0 after computing DFT." << endl;
}





///////////////////////////////////////// Transposing back //////////////////////////
//Complex H_f[w*hh];
if(rank==0)
{
Complex temp2;
for(int p=0;p<hh;p++)
{
        for(int q=0;q<w;q++)
        {     if(p>q)
	      {
              temp2=H1[(p+w*q)];
	       H1[p+w*q]=H1[p*w+q];
	       H1[p*w+q]=temp2; 
		}     
        }
}
    image.SaveImageData("MyAfter2D.txt",H1, w, hh);

}


/////////////////////////////////////////    INVERSE TRANSFORM ////////////////////////////////////////////////////////////

/// H1 is the input to inverse 2D

int StartLoc = (w / numtasks) * rank;

if(rank != 0)
{
MPI_Status status;
MPI_Recv(H1+StartLoc*hh,(w / numtasks) * hh * sizeof(Complex),
         MPI_CHAR, 0 , 0, MPI_COMM_WORLD, &status);
}
if(rank == 0)
 {
for(int n=1; n<numtasks; n++)
 {
  MPI_Send(H1+ w / numtasks * hh * n,w / numtasks * hh * sizeof(Complex),MPI_CHAR, n, 0, MPI_COMM_WORLD);
 cout<< " Rank 0 sending post transpose data to Rank" << rank << endl;
 }
}


int off2;
int start2=(hh/numtasks)*rank;
for(int i=0;i<hh/numtasks;i++)

{
       off2=w*(start2+i);
       InverseTransform1D(H1+off2,w,h+off2);
}
 

if(rank==0)
{
        for (int j=1;j<numtasks;j++)              
	 {

	        MPI_Status status;
                MPI_Recv(h+w*j*(hh/numtasks),w*(hh/numtasks)*sizeof(Complex), MPI_CHAR,j,0, MPI_COMM_WORLD, &status);
	}
}

if(rank != 0)   //other CPUs send to CPU 0
    {
     MPI_Send(h+start2*w,hh*(w / numtasks) * sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
     // cout << "Rank " << rank << " Sending 1D DFT rows to CPU 0." << endl
   }
  
//Rank 0 has h as inverse 1d
////////////////////////////////// Transposing h ////////////////////////////////////////////

int start3=rank*(hh/numtasks);

if(rank==0)  // rank 0 transposes
{

for(int p=0;p<hh;p++)
{
        for(int q=0;q<w;q++)
        {
        	 if(p>q)
                 {
                 	Complex temp2=h[p+w*q];
                        h[p+w*q]=h[q+w*p];
                        h[q+w*p]=temp2;
    		}                                                                                                             //H_t[q*hh+p]= H[p*hh+q];
        }
}
}

if(rank!=0)
{       MPI_Status status;
         MPI_Recv(h+start3*hh,(w/numtasks)*hh*sizeof(Complex), MPI_CHAR,0,0, MPI_COMM_WORLD, &status);
                 cout << "Rank" << status.MPI_SOURCE << "receiving from rank 0  " << endl;


}

if (rank ==0)
{
	for (int i=1;i<numtasks;i++)
	{
		MPI_Send(h+i*(hh/numtasks)*w,(hh/numtasks)*w*sizeof(Complex),MPI_CHAR,i,0,MPI_COMM_WORLD);
	}

}


/*if(rank!=0)
{	MPI_Status status;
	 MPI_Recv(h+start3*hh,(w/numtasks)*hh*sizeof(Complex), MPI_CHAR,0,0, MPI_COMM_WORLD, &status);
                 cout << "Rank" << status.MPI_SOURCE << "receiving from rank 0  ";


}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Complex* H2 = new Complex[w*hh];
//int start1=(hh/numtasks)*rank;
int off3;

        for (int x=0;x<(hh/numtasks);x++)
                {
                        off3=(start3+x)*hh;
                        InverseTransform1D(h+off3,hh,H2+off3);
                }
                                        
if(rank==0)
{
//cout << "entering rank 0 if loop " ;
        for (int j=1;j<numtasks;j++)
             {
             //Complex r_buf[w*numtasks];
             MPI_Status status;
              //cout << "receiving from rank " << rank;
             MPI_Recv(H2+w*j*(hh/numtasks),w*(hh/numtasks)*sizeof(Complex), MPI_CHAR,j,0, MPI_COMM_WORLD, &status);
             cout << "Rank 0 receiving from " << j<< endl ;
             }
}


if(rank!=0)
{
	MPI_Send(H2+start3*w,hh*(w / numtasks) * sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        cout << "Rank " << rank << " Sending 1D DFT rows to CPU 0 after computing DFT." << endl;
}

if(rank==0)
{
Complex temp4;
for(int p=0;p<hh;p++)
{
        for(int q=0;q<w;q++)
        {     if(p>q)
              {
              temp4=H2[(p+w*q)];
               H2[p+w*q]=H2[p*w+q];
               H2[p*w+q]=temp4;
                }
        }
}
    image.SaveImageDataReal("MyAfterInverse.txt",H2, w, hh);

}

  // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  // 3) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  // 4) Obtain a pointer to the Complex 1d array of input data
  // 5) Do the individual 1D transforms on the rows assigned to your CPU
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  // 6a) To send and receive columns, you might need a separate
  //     Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().
  // Create the helper object for reading the image
  // Step (1) in the comments is the line above.
 // Your code here, steps 2-9
 
}

//////////////////////////////////////////////////////////////// 1D Transform ////////////////////////
void Transform1D(Complex* h, int w, Complex* H)
{
 
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  //	Complex* dft[w*numtasks];
   	Complex S(0,0);
	for(int n=0;n<w;n++)
	{
		for(int k=0;k<w;k++)
		{	double c=cos(2*M_PI*n*k/w);
                        double s=sin(2*M_PI*n*k/w);
			Complex W=(0,0);
                        W = Complex(c,-s);

		        S=S+h[k]*W;
		}	
		H[n]=S;
		S=(0,0);
		
	}
	 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void InverseTransform1D(Complex* H,int w,Complex* h)
{
	Complex SS(0,0);
	for (int n=0;n<w;n++)
	{
		for (int k=0;k<w;k++)
		{	
			double cc=cos(2*M_PI*n*k/w);
			double ss=sin(2*M_PI*n*k/w);
			Complex WW=(0,0);
			WW=Complex(cc,ss);
			SS=SS+ H[k]*WW;
		}
		h[n]=SS;
		h[n].real = h[n].real / w;
		h[n].imag = h[n].imag / w;
		SS=(0,0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

   string fn("Tower.txt"); // default file name
   if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
   // MPI initialization here
      int rc;
          rc = MPI_Init(&argc, &argv);
             if(rc != MPI_SUCCESS)
                {
                   printf("Error starting MPI program, Terminating\n");
                      MPI_Abort(MPI_COMM_WORLD, rc);
                         }
                             Transform2D(fn.c_str()); // Perform the transform.
                             // Finalize MPI here
                                MPI_Finalize();
                               }  
                                
  
  



