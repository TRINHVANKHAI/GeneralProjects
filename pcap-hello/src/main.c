/*
 * Copyright (C) mrkhai@live.com all right reserved.
 *
 * Author: TRINH VAN KHAI
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the PG_ORGANIZATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY	THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS-IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>


int main(int argc, char* argv[]) {
	int i;
	char *dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* descr;
	const u_char *packet;
	struct pcap_pkthdr hdr;
	struct ether_header *eptr;
	u_char *ptr;

	dev = pcap_lookupdev(errbuf);

	if (dev==NULL)
	{
		printf ("%s\n", errbuf);
		exit(1);
	}

	printf ("DEV: %s\n", dev);

	descr = pcap_open_live (dev, BUFSIZ, 0, -1, errbuf);

	if (descr == NULL)
	{
		printf ("pcap_open_live: %s\n", errbuf);
		exit(1);

	}

	packet = pcap_next (descr, &hdr);

	if (packet == NULL) {
		printf ("Didn't grab any packet\n");
		exit (1);
	}

	printf ("Grabbed packet of length: %d\n", hdr.len);
	printf ("Received at: [ %s ]\n", ctime((const time_t *)&hdr.ts.tv_sec));
	printf ("Ethernet address length is: %d\n", ETHER_HDR_LEN);
	eptr = (struct ether_header*) packet;

	if (ntohs (eptr->ether_type) == ETHERTYPE_IP)
	{
		printf ("Ethernet type hex: %x dec:%d is an IP packet\n", ntohs(eptr->ether_type), ntohs(eptr->ether_type));

	}
	else if (ntohs (eptr->ether_type) == ETHERTYPE_ARP)
	{
		printf ("Ethernet type hex: %x dec:%d is an ARP packet\n", ntohs(eptr->ether_type), ntohs(eptr->ether_type));
	}
	else {
		printf ("Ethernet type %x not IP", ntohs (eptr->ether_type));
		exit (1);
	}

	ptr = eptr->ether_dhost;
	i = ETHER_ADDR_LEN;

	printf ("Destination Address:   ");

	do {
		printf("%s%x", (i==ETHER_ADDR_LEN) ? " " : " : ", *ptr++);
	} while (--i>0);
	printf("\n");

	ptr = eptr->ether_shost;
	i = ETHER_ADDR_LEN;
	printf ("Source Address:   ");
	do {
		printf ("%s%x", (i==ETHER_ADDR_LEN) ? " " : " : ", *ptr++);
	}while (--i>0);
	printf ("\n");


    return 0;
}
