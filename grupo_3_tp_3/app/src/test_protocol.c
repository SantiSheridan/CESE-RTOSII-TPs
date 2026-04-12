/*
 * uart_process.c
 *
 *  Created on: Apr 2, 2026
 *      Author: seb
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "protocol.h"

void print_msg_request(MsgRequest_t *msg)
{
  printf("Request: id: %u, priority: %u\n", msg->id, msg->priority);
}

void on_msg(ProtocolParser_t *p)
{
  MsgRequest_t msg;
  if (0 == msg_request_parser(p->buffer, &msg))
  {
    print_msg_request(&msg);
  }
}

void test_parse_msg(void)
{
  char buffer[32];
  ProtocolParser_t protocol_parser;
  protocol_parser_init(&protocol_parser, buffer, 64);

  char stream[] = "basura{type:1,id:2,priority:3}masbasura{type:1,id:7,priority:8}muchamasbasura{type:1,id:31,";
  for (int i = 0; stream[i] != '\0'; i++)
  {
    protocol_parser_feed(&protocol_parser, stream[i], on_msg);
  }
}

void test_create_and_write_msg(void)
{
  // tick
  {
    char buffer[64] = "";
    MsgTick_t msg;
    int ret;

    ret = msg_tick_create(&msg, 45);
    if (0 != ret) printf("Error msg_tick_create, %d\n", ret);
    ret = msg_tick_write(buffer, &msg);
    if (0 != ret) printf("Error msg_tick_write, %d\n", ret);
    printf("Tick Output Buffer: %s\n", buffer);
  }

  // response
  {
    char buffer[300] = "";
    MsgResponse_t msg;
    int ret;

    ret = msg_response_create(&msg, 24);
    if (0 != ret) printf("Error msg_response_create, %d\n", ret);
    ret = msg_response_write(buffer, &msg);
    if (0 != ret) printf("Error msg_response_write, %d\n", ret);
    printf("Response Output Buffer: %s\n", buffer);
  }
}

int main()
{
  test_parse_msg();
  test_create_and_write_msg();
  return 0;
}
