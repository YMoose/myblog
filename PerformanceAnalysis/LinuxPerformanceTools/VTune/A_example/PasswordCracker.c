// Original Author: Kris Kaspersky
// Adapted from Listing 1.17 of Code Optimization: Effective Memory Usage.
// Used in https://hadibrais.wordpress.com/2017/03/15/the-art-of-profiling-using-intel-vtune-amplifier-part-1/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//------------------------------------------------------------------------
// This example illustrates the worst practices of software development!
// It contains many of errors that have a negative effect on performance.
// Profiling will eliminate all of them.
//------------------------------------------------------------------------
// CONFIGURATION
#define ITER 10000000       // Maximum number of iterations
#define MAX_CRYPT_LEN 200 // Maximum length of the ciphertext

// Decrypting ciphertext using the found password
void DeCrypt(char *pswd, char *crypteddata)
{
  int a;
  int p = 0; // Pointer to the current position within the data

  // * * * MAIN DECRYPTION LOOP * * *
  do
  {
    // Decrypting the current character
    crypteddata[p] ^= pswd[p % strlen(pswd)];

    // Decrypting the next character
  } while (++p < strlen(crypteddata));
}

// Calculating the password checksum
int CalculateCRC(char *pswd)
{
  int a;
  int x = -1; // CRC calculation error

  for (a = 0; a < strlen(pswd); a++)
    x += *(int *)(pswd + a);
  return x;
}
// The CRC calculation algorithm is awkward.
// This was done intentionally to demonstrate misalignment.

// Checking the password CRC
int CheckCRC(char *pswd, int validCRC)
{
  if (CalculateCRC(pswd) == validCRC)
    return validCRC;
  // else
  return 0;
}

// Processing the current password
void do_pswd(char *crypteddata, char *pswd, int validCRC, int progress)
{
  char *buff;

  // Displaying the current state on the terminal
  printf("Current pswd : %10s [%d%%]\r", &pswd[0], progress);

  // Checking the password CRC
  if (CheckCRC(pswd, validCRC))
  { // CRC match
    // Copying ciphertext to the temporary buffer
    buff = (char *)malloc(strlen(crypteddata));
    strcpy(buff, crypteddata);

    // Decrypting
    DeCrypt(pswd, buff);

    // Displaying the decryption results
    printf("CRC %8X: try to decrypt: \"%s\"\n",
           CheckCRC(pswd, validCRC), buff);
  }
}

// Password generation procedure
int gen_pswd(char *crypteddata, char *pswd, int max_iter, int validCRC)
{
  int a;
  int p = 0;

  // Generating passwords
  for (a = 0; a < max_iter; a++)
  {

    // Processing the current password
    do_pswd(crypteddata, pswd, validCRC, 100 * a / max_iter);

    // * Main loop of password generation *
    // Using the "counter" algorithm
    while ((++pswd[p]) > 'z')
    {
      pswd[p] = '!';
      p++;
      if (!pswd[p])
      {
        pswd[p] = ' ';
        pswd[p + 1] = 0;
      }
    } // end while (pswd)
    // Returning the pointer to initial position
    p = 0;
  } // end for(a)
  return 0;
}

// Displaying the number, using DOT as a delimiter
void print_dot(float per)
{

// * * * CONFIGURATION * * *
#define N 3        // Separating three positions
#define DOT_SIZE 1 // Size of the DOT delimiter
#define DOT ","    // Delimiter
  int a;
  char buff[666];
  sprintf(buff, "%0.0f", per);
  for (a = strlen(buff) - N; a > 0; a -= N)
  {

    memmove(buff + a + DOT_SIZE, buff + a, 66);
    if (buff[a] == ' ')
      break;
    else
      memcpy(buff + a, DOT, DOT_SIZE);
  }

  // Displaying on the screen
  printf("%s\n", buff);
}

main(int argc, char **argv)
{

  // Variables
  FILE *f;           // For reading the source file (if present)
  char *buff;        // For reading data from the source file
  char *pswd;        // Currently tested password (needed by gen_pswd)
  int validCRC;      // For storing the original password CRC
  unsigned int t;    // For measuring the execution time
  int iter = ITER;   // Maximum number of passwords
  char *crypteddata; // For storing the ciphertext

  // built-in default crypt
  // Those who read this text have discovered a great secret. ;)
  char _DATA_[] = "\x4B\x72\x69\x73\x20\x4B\x61\x73\x70\x65\x72\x73\x6B"
                  "\x79\x20\x44\x65\x6D\x6F\x20\x43\x72\x79\x70\x74\x3A"
                  "\xB9\x50\xE7\x73\x20\x39\x3D\x30\x4B\x42\x53\x3E\x22"
                  "\x27\x32\x53\x56\x49\x3F\x3C\x3D\x2C\x73\x73\x0D\x0A";

  // Title
  printf("= = = VTune profiling demo = = =\n"
         "=================================\n");

  // Help
  if (argc == 2)
  {
    printf("USAGE:\n\tpswd.exe [StartPassword MAX_ITER]\n");
    return 0;
  }
  // Memory allocation
  printf("memory malloc\t\t");
  buff = (char *)malloc(MAX_CRYPT_LEN);
  if (buff)
    printf("+OK\n");
  else
  {
    printf("-ERR\n");
    return -1;
  }

  // Getting the ciphertext for decryption
  printf("get source from\t\tbuilt-in data\n");
  buff = _DATA_;

  // Calculating CRC
  validCRC = *(int *)(strstr(buff, ":") + 1);
  printf("calculate CRC\t\t%X\n", validCRC);
  if (!validCRC)
  {
    printf("-ERR: CRC is invalid\n");
    return -1;
  }

  // Extracting the encrypted data
  crypteddata = strstr(buff, ":") + 5;
  // printf ("cryptodata\t\t%s\n", crypteddata);

  // Allocating memory for the password buffer
  printf("memory malloc\t\t");
  pswd = (char *)malloc(512 * 1024);
  pswd += 62;
  // The consequences of misaligned data, when requesting blocks
  // of a chosen size, are demonstrated. The malloc function
  // always aligns the address by the desired 64 KB.

  memset(pswd, 0, 666); // Initialization

  if (pswd)
    printf("+OK\n");
  else
  {
    printf("-ERR\n");
    return -1;
  }
  // Parsing command line arguments, and getting the
  // initial password and maximum number of iterations
  printf("get arg from\t\t");
  if (argc > 2)
  {
    printf("command line\n");
    if (atoi(argv[2]) > 0)
      iter = atoi(argv[2]);
    strcpy(pswd, argv[1]);
  }
  else
  {
    printf("build-in default\n");
    strcpy(pswd, "!");
  }
  printf("start password\t\t%s\nmax iter\t\t%d\n", pswd, iter);

  // Starting password enumeration
  printf("===\ntry search... wait!\n");
  t = clock();
  gen_pswd(crypteddata, pswd, iter, validCRC);
  t = clock() - t;

  // Output of the number of passwords per second
  printf("                                      \rPassword per sec:\t");
  print_dot(iter / (float)t * CLOCKS_PER_SEC);

  return 0;
}