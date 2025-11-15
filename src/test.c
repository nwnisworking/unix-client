#include "test.h"

int hasFlag(uint8_t status, uint8_t flag){
  return (status & flag) == flag;
}

int debugMessage(Message* msg){
  printf("Status: 0x%02X, Length: %u, Data: '%.*s'\n", msg->status, ntohs(msg->length), ntohs(msg->length), msg->data);
  
  printf("Flags set:");

  if(hasFlag(msg->status, RES_BIT)) printf(" RES_BIT");
  else printf(" REQ_BIT");

  if(hasFlag(msg->status, ERR_BIT)) printf(" ERR_BIT");
  else printf(" OK_BIT");
  
  if(hasFlag(msg->status, NAME)) printf(" NAME");
  if(hasFlag(msg->status, PASSWORD)) printf(" PASSWORD");
  if(hasFlag(msg->status, AUTH)) printf(" AUTH");
  if(hasFlag(msg->status, DATA)) printf(" DATA");
  if(hasFlag(msg->status, CLOSE)) printf(" CLOSE");

  printf("\n");

  return 1;
}
