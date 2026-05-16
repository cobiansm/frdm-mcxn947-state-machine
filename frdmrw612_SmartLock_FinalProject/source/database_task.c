/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

//phpmyadmin user:root pwd:udooer

#include "lwip/opt.h"
#include "event_groups.h"
#include "stdio.h"

//Test from the web browser
//	http://192.168.1.2:1031/datalog.php?frdm_id=FRMD-Profe&sensor=acc&data=123123
//	http://192.168.1.2:1031/nfcauth.php?tagid=4474c7a1e4e81
//	http://192.168.1.2:1031/nfcreg.php?tagid=4474c7a1e4e81&name=Luis&lastname=Garabito&access=Mortal
//	http://192.168.1.2:1031/nfcreg.php?tagid=0638040324&name=Luis&lastname=Garabito&access=Mortal
//Test from the web browser

//TODO DSOAE extern tcpip Events group
extern EventGroupHandle_t tcpipEvent_group;
//TODO DSOAE extern the Queue Handler for the database queue
extern QueueHandle_t database_queue;
//TODO DSOAE extern the Queue Handler for the servo queue
extern QueueHandle_t servo_queue;

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
/*-----------------------------------------------------------------------------------*/
void database_task(void *pvParameters)
{
	struct netconn *conn;
	err_t err;
	int result;
	char* charptr;
	ip4_addr_t ipaddr;
	char HTTPrequest[100] = {0};
	struct netbuf *buf;
	void *data;
	u16_t len;
	char tagID[10];
	EventBits_t tcpipBits;

	//TODO DSOAE Wait until TCPIP stack is up and running
	tcpipBits = xEventGroupWaitBits(tcpipEvent_group,     /* The event group handle. */
										 0x01,            /* The bit pattern the event group is waiting for. */
										 pdFALSE,         /* 0x1 will be cleared automatically. */
										 pdFALSE,         /* Don't wait for both bits, either bit unblock task. */
										 portMAX_DELAY);

	PRINTF("Database Task Started.\n\r");
	//TODO DSOAE Wait for new messages on database queue using xQueueReceive
	while (xQueueReceive(database_queue, &tagID, portMAX_DELAY) == pdTRUE)
	//while (0)
	{
		PRINTF("Received a tagID to Authenticate: %s\n\r", tagID);
		/* Create a new connection identifier. */
		/* Bind connection to well known port number 1031. */
		conn = netconn_new(NETCONN_TCP);
		netconn_bind(conn, IP_ADDR_ANY, 7);

		LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

		IP4_ADDR(&ipaddr, 192,168,1,2);
		netconn_connect(conn, &ipaddr, 1031);

		//PRINTF("Authenticate user\n\r");
		//authenticate user
		sprintf(HTTPrequest, "GET /nfcauth.php?tagid=%s HTTP/1.0\r\n\r\n", tagID);
		//PRINTF("HTTPrequest to database: %s\n\r", HTTPrequest);
		err = netconn_write(conn, HTTPrequest, strlen(HTTPrequest), NETCONN_COPY);
		while ((err = netconn_recv(conn, &buf)) == ERR_OK)
		{
			do {
				netbuf_data(buf, &data, &len);
				//PRINTF("Received: %s\n", data);
			} while (netbuf_next(buf) >= 0);
			//PRINTF("Received: %s\n", data);
			charptr = strstr((const char *)data, "tag_id: ");
			if (charptr)
			{
				PRINTF("User does exists.\n\r");
				//TODO DSOAE send a message to the servo task to open the door
				char servo_cmd = 'o';
				xQueueSend(servo_queue, &servo_cmd, 0);
			}
			else
			{
				PRINTF("User does NOT exists.\n\r");
				PRINTF("Register new user\n\r");
				//register new user
				//sprintf(HTTPrequest, "GET /nfcreg.php?tagid=4474c7a1e4e81&name=Luis&lastname=Garabito&access=Mortal HTTP/1.0\r\n\r\n");
				sprintf(HTTPrequest, "GET /nfcreg.php?tagid=%s&name=Nuevo&lastname=Usuario&access=Mortal HTTP/1.0\r\n\r\n", tagID);
				err = netconn_write(conn, HTTPrequest, strlen(HTTPrequest), NETCONN_COPY);
				while ((err = netconn_recv(conn, &buf)) == ERR_OK)
				{
					do {
						netbuf_data(buf, &data, &len);
						PRINTF("Received: %s\n", data);
					} while (netbuf_next(buf) >= 0);
					//PRINTF("Received: %s\n", data);
					result = strncmp("HTTP/1.1 200 OK", data, 15);
					if (result == 0)
					{
						PRINTF("User Registered\n");
					}
					netbuf_delete(buf);
				}


			}
			netbuf_delete(buf);
		}


		//PRINTF("Sensor data Logged\n\r");
		//datalog sensor
	//	sprintf(HTTPrequest, "GET /datalog.php?frdm_id=Iteso&sensor=LightSensor&data=%d HTTP/1.0\r\n\r\n", tagID);
	//	err = netconn_write(conn, HTTPrequest, strlen(HTTPrequest), NETCONN_COPY);
	//	while ((err = netconn_recv(conn, &buf)) == ERR_OK)
	//	{
	//		do {
	//			netbuf_data(buf, &data, &len);
	//			PRINTF("Received: %s\n", data);
	//		} while (netbuf_next(buf) >= 0);
	//		//PRINTF("Received: %s\n", data);
	//		result = strncmp("HTTP/1.1 200 OK", data, 15);
	//		if (result == 0)
	//		{
	//			PRINTF("Data logged\n");
	//		}
	//		netbuf_delete(buf);
	//	}

	//	PRINTF("Register new user\n\r");
	//	//register new user
	//	sprintf(HTTPrequest, "GET /nfcreg.php?tagid=4474c7a1e4e81&name=Luis&lastname=Garabito&access=Mortal HTTP/1.0\r\n\r\n");
	//	err = netconn_write(conn, HTTPrequest, strlen(HTTPrequest), NETCONN_COPY);
	//	while ((err = netconn_recv(conn, &buf)) == ERR_OK)
	//	{
	//		do {
	//			netbuf_data(buf, &data, &len);
	//			PRINTF("Received: %s\n", data);
	//		} while (netbuf_next(buf) >= 0);
	//		//PRINTF("Received: %s\n", data);
	//		result = strncmp("HTTP/1.1 200 OK", data, 15);
	//		if (result == 0)
	//		{
	//			PRINTF("User Registered\n");
	//		}
	//		netbuf_delete(buf);
	//	}

	//	PRINTF("Authenticate user\n\r");
	//	//authenticate user
	//	sprintf(HTTPrequest, "GET /nfcauth.php?tagid=4474c7a1e4e81 HTTP/1.0\r\n\r\n");
	//	err = netconn_write(conn, HTTPrequest, strlen(HTTPrequest), NETCONN_COPY);
	//	PRINTF("Authenticate user1\n\r");
	//	while ((err = netconn_recv(conn, &buf)) == ERR_OK)
	//	{
	//		PRINTF("Authenticate user2\n\r");
	//		do {
	//			netbuf_data(buf, &data, &len);
	//			PRINTF("Received: %s\n", data);
	//		} while (netbuf_next(buf) >= 0);
	//		//PRINTF("Received: %s\n", data);
	//		result = strstr(data, "4474c7a1e4e81");
	//		if (result == 0)
	//		{
	//			PRINTF("User exists\n");
	//		}
	//		netbuf_delete(buf);
	//	}
		netconn_close(conn);
		netconn_delete(conn);
	}

	vTaskSuspend(NULL);

}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
