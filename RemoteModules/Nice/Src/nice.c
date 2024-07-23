#include "nice.h"
#include <stdint.h>
#include <stdlib.h>

#include "cc1101.h"
#include "cmsis_gcc.h"

static void PWM_Encode(uint8_t *in, uint8_t *out, size_t size_in) {
  for (size_t i = 0; i < size_in; i++) {
    for (int8_t b = 0; b < 8; b++) {
      if (in[i] & (1 << (7 - b))) {
        SET_BIT_ARRAY(out, (i * 8 + b) * 3, (PWM_TRUE & 0b100) >> 2);
        SET_BIT_ARRAY(out, (i * 8 + b) * 3 + 1, (PWM_TRUE & 0b010) >> 1);
        SET_BIT_ARRAY(out, (i * 8 + b) * 3 + 2, (PWM_TRUE & 0b001));
      } else {
        SET_BIT_ARRAY(out, (i * 8 + b) * 3, (PWM_FALSE & 0b100) >> 2);
        SET_BIT_ARRAY(out, (i * 8 + b) * 3 + 1, (PWM_FALSE & 0b010) >> 1);
        SET_BIT_ARRAY(out, (i * 8 + b) * 3 + 2, (PWM_FALSE & 0b001));
      }
    }
  }
}

static void Nice_ReformatPacket(uint8_t *pwm_data) {
  uint8_t tmp;
  uint8_t tmp2;

  // Adding manual 1 bit preamble
  tmp = pwm_data[0] & 1;
  pwm_data[0] = (pwm_data[0] >> 1) | (1 << 7);
  for (uint8_t i = 1; i < NICE_PACKETSIZE; i++) {
    tmp2 = pwm_data[i] & 1;
    pwm_data[i] = (pwm_data[i] >> 1) | (tmp << 7);
    tmp = tmp2;
  }

  // Triming useless bits
  pwm_data[4] &= (0xFF << 2);
}

/* Packet must be  5 bytes long */
static void Nice_MakePacket(niceModule *this, uint8_t *packet) {
  uint8_t data[2] = {
      (uint8_t)(this->id >> 2),
      (uint8_t)(((this->id & 0b11) << 6) | ((this->channel & 0b11) << 4))};
  uint8_t data_encode[2 * PWM_SIZE] = {0};

  PWM_Encode(data, data_encode, 2);
  Nice_ReformatPacket(data_encode);

  for (size_t i = 0; i < NICE_PACKETSIZE; i++)
    packet[i] = data_encode[i];
}

static void *Nice_AutoGeneratePacket(remoteModule *super, uint32_t field,
                                     uint8_t *packet, size_t size) {
  CHECK_OBJ NULL;
  if (size < NICE_PACKETSIZE)
    return NULL;

  niceModule *this = (niceModule *)(super->this);

  if (field == ID) {
    this->id = (this->id + 1) % NICE_MAX_ID;
  } else if (field == CHANNEL) {
    this->channel = (this->channel + 1) % NICE_MAX_CHANNEL;
  }

  Nice_MakePacket(this, packet);
  return packet;
}

static void *Nice_GeneratePacket(remoteModule *super, uint32_t *packet_fields,
                                 size_t packet_fields_size, uint8_t *packet,
                                 size_t size) {
  CHECK_OBJ NULL;
  if ((packet_fields_size > 2) || (size < NICE_PACKETSIZE))
    return NULL;

  niceModule *this = (niceModule *)(super->this);

  this->id = (uint16_t)(packet_fields[0]);
  this->channel = (uint8_t)(packet_fields[1]);

  Nice_MakePacket(this, packet);
  return packet;
}

static void Nice_Destructor(remoteModule *super) {
  CHECK_OBJ;

  S_FREE(super->this);
}

void Nice_Init(remoteModule *super, tranceivers tranceiver) {
  niceModule *this = malloc(sizeof(niceModule));

  this->tranceiver = tranceiver;
  this->id = 0;
  this->channel = 0;
  super->this = this;
  super->Destructor = &Nice_Destructor;
  super->GetRfSettings = &Nice_GetRfSettings;
  super->AutoGeneratePacket = &Nice_AutoGeneratePacket;
  super->GeneratePacket = &Nice_GeneratePacket;
}
