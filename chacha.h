#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// rotate bits left (32-bit number)
static inline uint32_t rotl(uint32_t chunk, uint8_t bits) {
  return (chunk << bits) | (chunk >> (32 - bits));
}

// little endian print function
void printMatPtr(uint32_t *m) {
  unsigned char *n = (unsigned char *)m;
  for (int i = 0; i < 64; i++) {
    printf("%c", n[i]);
  }
}

static void splitMat(uint32_t *input, uint32_t *output) {
  for (int i = 0; i < 16; i++) {
    output[i * 4] = (input[i] & 0x000000ff);
    output[i * 4 + 1] = (input[i] & 0x0000ff00) >> 8;
    output[i * 4 + 2] = (input[i] & 0x00ff0000) >> 16;
    output[i * 4 + 3] = (input[i] & 0xff000000) >> 24;
  }
}
static void str2hex(char *input, uint32_t *output) {
  int inputLength = strlen(input);
  for (int i = 0; i < inputLength; i++) {
    output[i] = (uint32_t)input[i];
  }
}
// quarter round of the ChaCha Cipher
static void quarterround(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
  *a += *b;
  *d ^= *a;
  *d = rotl(*d, 16);
  *c += *d;
  *b ^= *c;
  *b = rotl(*b, 12);
  *a += *b;
  *d ^= *a;
  *d = rotl(*d, 8);
  *c += *d;
  *b ^= *c;
  *b = rotl(*b, 7);
}

// main ChaCha Cipher
static uint32_t *chachaCipherMain(uint32_t *key, uint32_t counter,
                                  uint32_t *nonce) {
  uint32_t inputWords[16];
  uint32_t temp[16];
  static uint32_t output[16];
  // fill in the constants
  inputWords[0] = 0x61707865;
  inputWords[1] = 0x3320646e;
  inputWords[2] = 0x79622d32;
  inputWords[3] = 0x6b206574;

  // fill in key
  inputWords[4] = *key;
  inputWords[5] = *(key + 1);
  inputWords[6] = *(key + 2);
  inputWords[7] = *(key + 3);
  inputWords[8] = *(key + 4);
  inputWords[9] = *(key + 5);
  inputWords[10] = *(key + 6);
  inputWords[11] = *(key + 7);

  // fill in seed and counter
  inputWords[12] = counter;
  inputWords[13] = *nonce;
  inputWords[14] = *(nonce + 1);
  inputWords[15] = *(nonce + 2);

  for (int j = 0; j < 16; j++) {
    temp[j] = inputWords[j];
  }
  for (int round = 0; round < 10; round++) {
    quarterround(&inputWords[0], &inputWords[4], &inputWords[8],
                 &inputWords[12]);
    quarterround(&inputWords[1], &inputWords[5], &inputWords[9],
                 &inputWords[13]);
    quarterround(&inputWords[2], &inputWords[6], &inputWords[10],
                 &inputWords[14]);
    quarterround(&inputWords[3], &inputWords[7], &inputWords[11],
                 &inputWords[15]);
    quarterround(&inputWords[0], &inputWords[5], &inputWords[10],
                 &inputWords[15]);
    quarterround(&inputWords[1], &inputWords[6], &inputWords[11],
                 &inputWords[12]);
    quarterround(&inputWords[2], &inputWords[7], &inputWords[8],
                 &inputWords[13]);
    quarterround(&inputWords[3], &inputWords[4], &inputWords[9],
                 &inputWords[14]);
  }
  for (int k = 0; k < 16; k++) {
    output[k] = temp[k] + inputWords[k];
  }
  return output;
}

// encryption based on ChaCha
// Takes a a key that is chosen by user, a counter which will be incremented, a
// seed which is chosen by the user, a message, and the length of the mesage in
// bytes

static uint32_t *encrypt(char *message, int messageLength, uint32_t *key,
                         uint32_t counter, uint32_t *nonce,
                         uint32_t *encryptedMessage) {
  // this means that the message length is divisible by 64 and we do not have to
  // do any padding, start encryption
  int numBlocks = messageLength / 64;

  // define all necessary arrays and vars that we will need
  char message_block[64];
  uint32_t message_block_hex[64];
  uint32_t tempRes[64];
  uint32_t *key_stream_block_temp; // 16 element array that will need broken up
                                   // into a 64 element
  uint32_t key_stream_block[64];

  // for each block:
  for (int i = 0; i < numBlocks; i++) {
    printf("Block #%d\n", i + 1);
    // generate the key block
    key_stream_block_temp = chachaCipherMain(key, counter + i, nonce);

    // copy correct chars to new message block
    memcpy(message_block, message + (i * 64), 64);

    // add null terminator
    message_block[64] = '\0';

    // convert to hex
    str2hex(message_block, message_block_hex);

    // convert the key stream into 64 bytes instead 16- 2 word groups
    splitMat(key_stream_block_temp, key_stream_block);
    // XOR with the key stream and add to the encrypted message
    for (int j = 0; j < 64; j++) {
      printf("message_block_hex: %x ^ key hex: %x  = ", message_block_hex[j],
             key_stream_block[j]);
      tempRes[j] = message_block_hex[j] ^ key_stream_block[j];
      printf("%x\n", tempRes[j]);
      encryptedMessage[i * 64 + j] = tempRes[j];
    }
  }
  return encryptedMessage;
}
static uint32_t *chacha_next(uint32_t *key, uint32_t counter, uint32_t *nonce,
                             char *seed, uint32_t *encryptedMessage,
                             int seedLength) {

  double remainder =
      seedLength %
      64; // remainder is zero with test message, we will need two blocks

  if (remainder != 0) {
    // pad message

    // calculate next multiple of 64

    int idx = seedLength;
    while (idx % 64 != 0) {
      idx++;
    }
    char padded_seed[idx + 1]; // idx mod 64 should be 0 here
    // copy the original message into the new message and terminate with \0
    memcpy(padded_seed, seed, seedLength);
    char *ptr = &padded_seed[seedLength];
    memset(ptr, '\0', idx - seedLength);
    padded_seed[idx] = '\0';
    // same as below
    seedLength = idx;

    encrypt(padded_seed, seedLength, key, counter, nonce, encryptedMessage);
    return encryptedMessage;
  } else {

    encrypt(seed, seedLength, key, counter, nonce, encryptedMessage);
    return encryptedMessage;
  }
}