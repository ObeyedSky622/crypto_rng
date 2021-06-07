#include "chacha.h"
#include <stdint.h>
#include <stdio.h>

int main() {

  uint32_t key[8];
  uint32_t counter;
  uint32_t nonce[3];
  key[0] = 0x03020100;
  key[1] = 0x07060504;
  key[2] = 0x0b0a0908;
  key[3] = 0x0f0e0d0c;
  key[4] = 0x13121110;
  key[5] = 0x17161514;
  key[6] = 0x1b1a1918;
  key[7] = 0x1f1e1d1c;

  nonce[0] = 0x00000000;
  nonce[1] = 0x4a000000;
  nonce[2] = 0x00000000;
  counter = 0x00000001;

  char seed[] = "Ladies and Gentlemen of the class of '99: If I could offer "
                "you only one tip for the "
                "future,sunscreen would be it";
  int encryptLength = strlen(seed);
  uint32_t encryptedMessage[encryptLength];
  uint32_t *rn =
      chacha_next(key, counter, nonce, seed, encryptedMessage, encryptLength);
  printMatPtr(rn);
  return 0;
}