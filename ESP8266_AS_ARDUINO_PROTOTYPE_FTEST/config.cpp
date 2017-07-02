/**
 * @file   config.cpp
 * @brief  MicSense configuration functions
 *
 * Provides functions related to retrieving and saving MicSense 
 * configuration.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */

#include "config.h"
#include "util.h"
#include <EEPROM.h>
#include <bcrypt.hpp>
#include <mbedtls-esp8266-arduino.h>
#include <esp/hwrand.h>
#include <mbedtls/sha256.h>
#include <mbedtls/gcm.h>

#define GET_RAW_BYTE(var, idx) ((uint8_t *)(var))[idx]
#define SET_RAW_BYTE(var, idx, val) ((uint8_t *)(var))[idx] = val

/* Table and CRC32 function from SNIPPETS C Source Code Archive.
 * Although not available anymore (website is now down), the code used can
 * be found here:
 * https://opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/crc32.c
 */
static uint32_t crc32_tab[] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
  0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
  0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
  0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
  0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
  0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
  0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
  0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
  0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
  0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
  0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
  0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
  0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
  0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
  0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
  0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
  0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
  0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
  0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

uint32_t crc32(const void *buf, size_t size) {
  /* Starting CRC, modified from Apple/BSD's version */
  uint32_t crc = 0xFFFFFFFF;
  const uint8_t *p;

  p = (const uint8_t *) buf;
  crc = crc ^ ~0U;

  while (size--)
    crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

  return crc ^ ~0U;
}

/**
 * Initialize the EEPROM.
 * 
 * Enable internal EEPROM reading and writing. This should only be
 * called once.
 */
void eepromInit() {
  // WARNING:
  //   This table is now deprecated. The calculations for each variable's offset
  //   and size are automatically determined via macros. This table will be
  //   condensed down into field name, field components, and field component sizes
  //   only in a future commit.
  // 
  // EEPROM usage:
  //   mid_thresh (int)
  //     offset 0 bytes
  //       - 4 bytes CRC32
  //       - 4 bytes int
  //     Total = 8 bytes
  //   high_thresh (int)
  //     offset 8 bytes
  //       - 4 bytes CRC32
  //       - 4 bytes int
  //     Total = 8 bytes
  //   ssid (str)
  //     offset 16 bytes
  //       - 4 bytes CRC32
  //       - 32 bytes ASCII
  //     Total = 36 bytes
  //     due to 8 byte chunking, will technically take up 40 bytes
  //   psk (str)
  //     offset 56 bytes
  //       - 4 bytes CRC32
  //       - 64 bytes ASCII
  //     Total = 68 bytes
  //     due to 8 byte chunking, will technically take up 72 bytes
  //   micsense_server (str)
  //     offset 128 bytes
  //       - 4 bytes CRC32
  //       - 64 bytes ASCII
  //     Total = 68 bytes
  //     due to 8 byte chunking, will technically take up 72 bytes
  //   use_https (str)
  //     offset 200 bytes
  //       - 4 bytes CRC32
  //       - 1 byte char "bool" (true/false, FF/00)
  //     Total = 5 bytes
  //   https_fingerprint (str)
  //     offset 208 bytes
  //       - 4 bytes CRC32
  //       - 128 bytes ASCII
  //     Total = 132 bytes
  //     NOTE:
  //       For the fingerprint, the currently accepted one (as of writing) is
  //       a SHA1 fingerprint, which is very small (40-59 bytes, with or without
  //       spacing, respectively). However, it is strongly recommended to upgrade
  //       the hashing algorithm to something stronger, such as SHA256, SHA512, or
  //       the newly created SHA3. The space allocated can support SHA512/SHA3
  //       with no spaces (128 bytes).
  //       
  //       When support is enabled for better hashing, this field is futureproof...
  //       at least until the newer hashes are broken, too!
  //
  // Overall usage: 344 bytes
  // 
  // Future security improvements can include HTTPS certificates, but
  // implementation has not stabilized yet, and the certificates may
  // need to be stored as SPIFFS instead of EEPROM due to size
  // constraints. Therefore, there is no space allocated here.

  EEPROM.begin(768);
}

/**
 * Checksum and write data to the EEPROM.
 * 
 * Write data to the EEPROM. This function will compute the CRC32 of
 * the given data, and write it as the first 4 bytes after the given
 * offset. Once done, the data will be written right after the
 * checksum.
 * 
 * @param [in] offset  offset of the EEPROM to write to
 * @param [in] data    data to write to the EEPROM
 * @param [in] size    size of the data that will be written to the EEPROM;
 *                     does NOT include the CRC32
 * @see eepromRead()
 */
void eepromWrite(const int offset, const void *data, size_t size) {
  size_t pos = 0;
  uint32_t crc32_chksum = crc32(data, size);

  SerialPrintStr("[eepromWrite] Writing EEPROM offset ");
  Serial.print(offset);
  SerialPrintStr(", size ");
  Serial.println(size);
  
  /* Write CRC32 bytes */
  EEPROM.write(offset, GET_RAW_BYTE(&crc32_chksum, 0));
  EEPROM.write(offset + 1, GET_RAW_BYTE(&crc32_chksum, 1));
  EEPROM.write(offset + 2, GET_RAW_BYTE(&crc32_chksum, 2));
  EEPROM.write(offset + 3, GET_RAW_BYTE(&crc32_chksum, 3));

  SerialPrintStr("[eepromWrite] CRC32 computed: ");
  Serial.println(crc32_chksum, HEX);

  /* Write actual data bytes */
  for (pos = 0; pos < size; pos++) {
    EEPROM.write(offset + 4 + pos, GET_RAW_BYTE(data, pos));
    SerialPrintStr("[eepromWrite] Byte ");
    Serial.print(pos);
    SerialPrintStr(" returns ");
    Serial.println(GET_RAW_BYTE(data, pos), HEX);

    if (EEPROM.read(offset + 4 + pos) != GET_RAW_BYTE(data, pos)) {
      SerialPrintStr("[eepromWrite] Mismatched byte @ ");
      Serial.print(pos);
      SerialPrintStr(" | Correct byte is ");
      Serial.print(GET_RAW_BYTE(data, pos), HEX);
      SerialPrintStr(" | EEPROM byte is ");
      Serial.println(EEPROM.read(offset + 4 + pos), HEX);
    }
  }
}

/**
 * Read and verify data from the EEPROM.
 * 
 * Read data from the EEPROM. This function will read the entire data chunk
 * specified (including the CRC32). Once read, the data will be verified with
 * the first 4 bytes of the data chunk, aka the CRC32 checksum computed when
 * the data was written.
 * 
 * If the data is verified, the data will be written to the pointer provided.
 * If the checksum verification fails, the pointer will instead be written with
 * zeroes.
 * 
 * @param [in]  offset  offset of the EEPROM to read from
 * @param [out] data    data to read out from the EEPROM; should contain enough
 *                      memory to store the amount of data specified in the size
 *                      argument
 * @param [in]  size    size of the data that will be read from the EEPROM;
 *                      does NOT include the CRC32
 * @see eepromRead()
 */
void eepromRead(const int offset, void *data, size_t size) {
  size_t pos = 0;
  uint32_t crc32_chksum_ref = 0, crc32_chksum_computed = 0;

  SerialPrintStr("[eepromRead] Reading EEPROM offset ");
  Serial.print(offset);
  SerialPrintStr(", size ");
  Serial.println(size);
  
  /* Write CRC32 bytes */
  SET_RAW_BYTE(&crc32_chksum_ref, 0, EEPROM.read(offset));
  SET_RAW_BYTE(&crc32_chksum_ref, 1, EEPROM.read(offset + 1));
  SET_RAW_BYTE(&crc32_chksum_ref, 2, EEPROM.read(offset + 2));
  SET_RAW_BYTE(&crc32_chksum_ref, 3, EEPROM.read(offset + 3));

  /* Read data bytes */
  for (pos = 0; pos < size; pos++) {
    SET_RAW_BYTE(data, pos, EEPROM.read(offset + 4 + pos));
    SerialPrintStr("[eepromRead] Byte ");
    Serial.print(pos);
    SerialPrintStr(" returns ");
    Serial.println(EEPROM.read(offset + 4 + pos), HEX);
  }

  /* Verify checksum */
  crc32_chksum_computed = crc32(data, size);
  
  if (crc32_chksum_computed != crc32_chksum_ref) {
    /* Zero out memory */
    SerialPrintStr("[eepromRead] CRC32 match failed - computed CRC32 ");
    Serial.print(crc32_chksum_computed, HEX);
    SerialPrintStr(" does not match reference (stored) CRC32 ");
    Serial.println(crc32_chksum_ref, HEX);
    
    memset(data, 0, size);
  }
}

/* TEMPORARY - needed to resolve an issue with linking on wrapper.c for bcrypt.
 * Should hopefully be fixed when a newer ESP8266 Arduino SDK comes out.
 * (Written while using v2.3.0 - delete this function if an error regarding this 
 * function becoming a duplicate occurs.)
 */
void *memchr(const void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char*)s;
    while( n-- )
        if( *p != (unsigned char)c )
            p++;
        else
            return p;
    return 0;
}

/**
 * AES256 + bcrypt PW deriv
 * Storage format:
 *   [CRC32] [64 byte salt] [16 byte tag] [12 byte IV] [Encrypted data (decoded chunks of 16)]
 */
void eepromCryptWrite(const int offset, const void *data, size_t size) {
  char bcrypt_salt[BCRYPT_HASHSIZE], bcrypt_hash[BCRYPT_HASHSIZE];
  unsigned char sha256_hash[32];
  unsigned char iv[12];
  unsigned char tag[16];

  unsigned char enc_data[size];
  unsigned char eeprom_data[BCRYPT_HASHSIZE + 16 + 12 + size];

  int result;
  
  mbedtls_gcm_context gcm;

  /* Generate salt for bcrypt */
  SerialPrintStrLn("[eepromCryptWrite] Generating salt for bcrypt...");
  Bcrypt::gensalt(1, bcrypt_salt);

  /* Build key to use for AES hash */
  SerialPrintStrLn("[eepromCryptWrite] Building AES key, part 1 (bcrypt)...");
  
  // NOTE: bcrypt temporarily disabled while we're debugging an issue with
  // hashpw() crashing the ESP8266... for now, the hash is assumed to be a
  // zero array.
  // ** THIS IS INSECURE - DO NOT USE IN PRODUCTION **
  
  //Bcrypt::hashpw(PASSWD, bcrypt_salt, bcrypt_hash);
  memset(bcrypt_hash, 0, BCRYPT_HASHSIZE);
  
  SerialPrintStrLn("[eepromCryptWrite] Building AES key, part 2 (sha256)...");
  mbedtls_sha256((const unsigned char *)bcrypt_hash, BCRYPT_HASHSIZE, sha256_hash, 0);
  Serial.println(sha256_hash[0], HEX);
  Serial.println(sha256_hash[1], HEX);
  Serial.println(sha256_hash[2], HEX);

  /* Generate IV */
  SerialPrintStrLn("[eepromCryptWrite] Generating IV for AES...");
  hwrand_fill(iv, 12);
  
  /* GCM */
  SerialPrintStrLn("[eepromCryptWrite] Encrypting and signing with AES GCM...");
  mbedtls_gcm_init(&gcm);
  mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, sha256_hash, 256);
  result = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, size, iv, 12, NULL, 0, (const unsigned char *) data, (unsigned char *) enc_data, 16, tag);
  mbedtls_gcm_free(&gcm);

  if (result != 0) {
    SerialPrintStrLn("[eepromCryptWrite] Could not encrypt data!");
    SerialPrintStr("[eepromCryptWrite] Error code (hex): ");
    Serial.println(result, HEX);
    return;
  }

  SerialPrintStrLn("Tag hex data (first/last 3 bytes):");
  Serial.println(tag[0], HEX);
  Serial.println(tag[1], HEX);
  Serial.println(tag[2], HEX);
  Serial.println(tag[13], HEX);
  Serial.println(tag[14], HEX);
  Serial.println(tag[15], HEX);

  /* Save everything! */
  SerialPrintStrLn("[eepromCryptWrite] Building final encrypted data...");
  memcpy(eeprom_data, bcrypt_salt, BCRYPT_HASHSIZE);
  memcpy(eeprom_data+BCRYPT_HASHSIZE, tag, 16);
  memcpy(eeprom_data+BCRYPT_HASHSIZE+16, iv, 12);
  memcpy(eeprom_data+BCRYPT_HASHSIZE+16+12, enc_data, size);

  SerialPrintStrLn("[eepromCryptWrite] Writing final encrypted data to EEPROM...");
  eepromWrite(offset, eeprom_data, size+BCRYPT_HASHSIZE+16+12);
}

int isAllZero(void *data, size_t size) {
  unsigned char sum = 0;
  size_t i;
  for (i = 0; i < size; ++i) {
    sum |= ((unsigned char *)data)[i];
  }
  
  /* Return 1 if all zero, 0 if not */
  return (sum ? 0 : 1);
}

void eepromCryptRead(const int offset, void *data, size_t size) {
  char bcrypt_salt[BCRYPT_HASHSIZE], bcrypt_hash[BCRYPT_HASHSIZE];
  unsigned char sha256_hash[32];
  unsigned char iv[12];
  unsigned char tag[16];
  int result;

  unsigned char enc_data[BCRYPT_HASHSIZE + 16 + 12 + size];

  /* Read from EEPROM */
  SerialPrintStrLn("[eepromCryptRead] Reading encrypted data from EEPROM...");
  eepromRead(offset, enc_data, size+BCRYPT_HASHSIZE+16+12);

  /* Make sure we didn't get back a zeroed-out response */
  if (isAllZero(enc_data, size+BCRYPT_HASHSIZE+16+12)) {
    /* Zero out memory */
    SerialPrintStrLn("[eepromCryptRead] CRC32 failed when reading from EEPROM, returning.");
    SerialPrintStrLn("(memset to zero)");
    Serial.println(size);
    memset(data, 0, size);
    SerialPrintStrLn("(return)");
    return;
  }
  
  /* Read everything! */
  SerialPrintStrLn("[eepromCryptRead] Extracting encrypted data...");
  memcpy(bcrypt_salt, enc_data, BCRYPT_HASHSIZE);
  memcpy(tag, enc_data+BCRYPT_HASHSIZE, 16);
  memcpy(iv, enc_data+BCRYPT_HASHSIZE+16, 12);
  memcpy(enc_data, enc_data+BCRYPT_HASHSIZE+16+12, size);
  // model:
  // copy 5 bytes from pos 12 to pos 0, so 0-4
  // set idx 5 from pos 5-11 to zero
  // memset(enc_data + 5, 0, 12 - 5);
  memset(enc_data+size, 0, (BCRYPT_HASHSIZE+16+12) - size);

  SerialPrintStrLn("Tag hex data (first/last 3 bytes):");
  Serial.println(tag[0], HEX);
  Serial.println(tag[1], HEX);
  Serial.println(tag[2], HEX);
  Serial.println(tag[13], HEX);
  Serial.println(tag[14], HEX);
  Serial.println(tag[15], HEX);

  mbedtls_gcm_context gcm;

  /* Build key to use for AES hash */
  SerialPrintStrLn("[eepromCryptRead] Building AES key, part 1 (bcrypt)...");
  
  // NOTE: bcrypt temporarily disabled while we're debugging an issue with
  // hashpw() crashing the ESP8266... for now, the hash is assumed to be a
  // zero array.
  // ** THIS IS INSECURE - DO NOT USE IN PRODUCTION **
  
  //Bcrypt::hashpw(PASSWD, bcrypt_salt, bcrypt_hash);
  memset(bcrypt_hash, 0, BCRYPT_HASHSIZE);
  
  SerialPrintStrLn("[eepromCryptRead] Building AES key, part 2 (sha256)...");
  mbedtls_sha256((const unsigned char *)bcrypt_hash, BCRYPT_HASHSIZE, sha256_hash, 0);
  Serial.println(sha256_hash[0], HEX);
  Serial.println(sha256_hash[1], HEX);
  Serial.println(sha256_hash[2], HEX);
  
  /* GCM */
  SerialPrintStrLn("[eepromCryptRead] Validating and decrypting with AES GCM...");
  mbedtls_gcm_init(&gcm);
  mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, sha256_hash, 256);
  result = mbedtls_gcm_auth_decrypt(&gcm, size, iv, 12, NULL, 0, tag, 16, (const unsigned char *) enc_data, (unsigned char *) data);
  mbedtls_gcm_free(&gcm);

  if (result != 0) {
    /* Zero out memory */
    SerialPrintStrLn("[eepromCryptRead] Could not decrypt data!");
    SerialPrintStr("[eepromCryptRead] Error code (hex): ");
    Serial.println(result, HEX);

    if (result == MBEDTLS_ERR_GCM_AUTH_FAILED) SerialPrintStrLn("[eepromCryptRead] Decoded error: MBEDTLS_ERR_GCM_AUTH_FAILED");
    if (result == MBEDTLS_ERR_GCM_BAD_INPUT) SerialPrintStrLn("[eepromCryptRead] Decoded error: MBEDTLS_ERR_GCM_BAD_INPUT");
    
    memset(data, 0, size);
  }
}


void loadLocalThresholds() {
  int mid_thresh_tmp = 0;
  int high_thresh_tmp = 0;
  
  eepromRead(EEPROM_MID_THRESH_OFFSET, &mid_thresh_tmp, SUB_CRC(EEPROM_MID_THRESH_SIZE));
  eepromRead(EEPROM_HIGH_THRESH_OFFSET, &high_thresh_tmp, SUB_CRC(EEPROM_HIGH_THRESH_SIZE));
  
  if ((mid_thresh_tmp > 0) && (mid_thresh_tmp < 1024)) {
    mid_thresh = mid_thresh_tmp;
    SerialPrintStrLn("[loadLocalThresholds] Loaded saved mid thresh!");
  } else {
    SerialPrintStr("[loadLocalThresholds] Not loading invalid saved mid thresh: ");
    Serial.println(mid_thresh_tmp);
  }

  if ((high_thresh_tmp > 0) && (high_thresh_tmp < 1024)) {
    high_thresh = high_thresh_tmp;
    SerialPrintStrLn("[loadLocalThresholds] Loaded saved high thresh!");
  } else {
    SerialPrintStr("[loadLocalThresholds] Not loading invalid saved high thresh: ");
    Serial.println(high_thresh_tmp);
  }

  SerialPrintStr("[loadLocalThresholds] Current mid thresh: ");
  Serial.println(mid_thresh);
  SerialPrintStr("[loadLocalThresholds] Current high thresh: ");
  Serial.println(high_thresh);
}

void saveLocalThresholds() {
  SerialPrintStrLn("[saveLocalThresholds] Saving thresholds!");

  SerialPrintStrLn("[saveLocalThresholds]   Saving mid thresh...");
  eepromWrite(EEPROM_MID_THRESH_OFFSET, &mid_thresh, EEPROM_MID_THRESH_SIZE - EEPROM_DATA_CHUNK_CRC_SIZE);
  
  SerialPrintStrLn("[saveLocalThresholds]   Saving high thresh...");
  eepromWrite(EEPROM_HIGH_THRESH_OFFSET, &high_thresh, EEPROM_HIGH_THRESH_SIZE - EEPROM_DATA_CHUNK_CRC_SIZE);

  // Commit changes!
  EEPROM.commit();
}

void loadLocalWiFiCredentials() {
  char ssid_tmp[33];
  char psk_tmp[65];

  SerialPrintStrLn("SUB_ENC(SUB_CRC(EEPROM_WIFI_SSID_SIZE))");
  Serial.println(SUB_ENC(SUB_CRC(EEPROM_WIFI_SSID_SIZE)));
  eepromCryptRead(EEPROM_WIFI_SSID_OFFSET, ssid_tmp, SUB_ENC(SUB_CRC(EEPROM_WIFI_SSID_SIZE)));

  SerialPrintStrLn("SUB_ENC(SUB_CRC(EEPROM_WIFI_PSK_SIZE))");
  Serial.println(SUB_ENC(SUB_CRC(EEPROM_WIFI_PSK_SIZE)));
  eepromCryptRead(EEPROM_WIFI_PSK_OFFSET, psk_tmp, SUB_ENC(SUB_CRC(EEPROM_WIFI_PSK_SIZE)));

  SerialPrintStr("ssid_tmp = ");
  Serial.println(ssid_tmp);

  SerialPrintStr("psk_tmp = ");
  Serial.println(psk_tmp);
  
  Serial.println("We're doneeeeee");
}

void saveLocalWiFiCredentials(const char ssid[33], const char psk[65]) {
  eepromCryptWrite(EEPROM_WIFI_SSID_OFFSET, (void *) ssid, SUB_ENC(SUB_CRC(EEPROM_WIFI_SSID_SIZE)));
  eepromCryptWrite(EEPROM_WIFI_PSK_OFFSET, psk, SUB_ENC(SUB_CRC(EEPROM_WIFI_PSK_SIZE)));
  
  Serial.println("Saved");
}
