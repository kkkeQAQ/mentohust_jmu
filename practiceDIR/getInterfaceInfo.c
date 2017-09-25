#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

//#define IFNAMSIZ 16
#define DEFAULT_NIC_NAME "bnep0"
#define MAC_SIZE 6
#define NI_MAXHOST 1025 

//#define DEBUG 1
typedef struct nic_struct {
    char nic_name[IFNAMSIZ]; // network interface card name
    unsigned char nic_MAC[MAC_SIZE]; // network interface card MAC address
    struct in_addr ipaddr; // ip address
    struct in_addr netmask; // network mask
    struct in_addr geteway; 
    struct in_addr dns;
}NIC_STRUCT; 

//int get_nif_addres(void);
int get_nif_addres(NIC_STRUCT *);
//int get_adapterName(char *);
int get_adapterName(NIC_STRUCT *); 

int main()
{
//	get_nif_addres();
  static NIC_STRUCT nic_info;
  static char nic_name[IFNAMSIZ];
  memset(&nic_info, 0, sizeof(NIC_STRUCT));
  get_adapterName(&nic_info);
  get_nif_addres(&nic_info);
  printf("network interface card name:  %s\n"
         "                       ipaddr: %s\n",
	   nic_info.nic_name, inet_ntoa(nic_info.ipaddr));
  printf("                       netmask: %s\n"
	 "                       MAC addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
           inet_ntoa(nic_info.netmask),
	   nic_info.nic_MAC[0],nic_info.nic_MAC[1], nic_info.nic_MAC[2], nic_info.nic_MAC[3], 
	   nic_info.nic_MAC[4], nic_info.nic_MAC[5]);


  return 0;
}

int get_adapterName(NIC_STRUCT *nic_info)
{
  struct ifaddrs *ifaddr, *ifa;
  int family, s, n;
  char hbuf[NI_MAXHOST]; 
  size_t available_adapter_num = 0, choosed_adapter = 0;
  char **available_adapter_name = NULL, **more_available_adapter_names = NULL;

  

  if (getifaddrs(&ifaddr) == -1)
  {
    printf("%s", strerror(errno));
  }

  /* Walk through linked list, maintaining head pointer so we 
   * can free list later
   */

  for(ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
  {
    if (ifa->ifa_addr == NULL)
      continue;
    family = ifa->ifa_addr->sa_family;
    if(ifa->ifa_flags & IFF_LOOPBACK)
	    continue;
//    if ( family == AF_INET || family == AF_INET6 )
    if( family == AF_INET ) // only use ipv4
    {
      s = getnameinfo(ifa->ifa_addr,
		 (family == AF_INET) ? sizeof(struct sockaddr_in) :
	                               sizeof(struct sockaddr_in6),
	          hbuf, NI_MAXHOST,
                  NULL, 0, NI_NUMERICHOST);		  
      if(s != 0)
      {
       // printf("getnameinfo() failed: %s\n", gai_strerror(s));
	//exit(EXIT_FAILURE);
	continue;
      } else {
	if(strlen(ifa->ifa_name) >= IFNAMSIZ) // avoid adapter name exceed sizeof( ifr.ifr_name )
	  continue;
	available_adapter_num++;
	/*
	 * (re)allocating adapters name array for user to chose
	 */
	if(available_adapter_name == NULL)
	{
	  available_adapter_name = (char**)calloc(1, sizeof(char*));
	  if(available_adapter_name == NULL)
	  {
             printf("Error (re)allocating adapters name memory (char **)");
	     exit(EXIT_FAILURE);
	  }
          available_adapter_name[(int)available_adapter_num - 1] = (char*)calloc(IFNAMSIZ, sizeof(char));
	  if(available_adapter_name[(int)available_adapter_num - 1] == NULL)
	  {
	     printf("Error (re)allocating adapters name memory (available_adapter_name[0])");
	     exit(EXIT_FAILURE);
	  }	
	} else {
	 more_available_adapter_names = (char**)realloc(available_adapter_name, available_adapter_num * sizeof(char*));
	 if (more_available_adapter_names != NULL)
	 {
	   available_adapter_name = more_available_adapter_names;
	   available_adapter_name[(int)available_adapter_num - 1] = (char*)calloc(IFNAMSIZ, sizeof(char));
	   if(available_adapter_name[(int)available_adapter_num - 1] == NULL)
	   {
	     printf("Error (re)allocating adapters name memory (available_adapter_name[%zu])", available_adapter_num - 1);
	     exit(EXIT_FAILURE);
	   }

	 } else {
	   free (available_adapter_name);
	   printf("Error reallocating adapters name memory");
	   exit(EXIT_FAILURE);
	 }
        }
	memcpy(available_adapter_name[available_adapter_num - 1], ifa->ifa_name, sizeof(ifa->ifa_name));
	printf("[ %zu ] %s  <%s>\n", available_adapter_num,  ifa->ifa_name, hbuf);

      }
    } else {
      continue;
    }
  }
  if(available_adapter_num == 0)
  {
    freeifaddrs(ifaddr);
    ifaddr = NULL;
    printf("No available network adapter\n");
    
    return -1; // failed to get adapter
  } else {
    printf("please enter 1 ~ %zu to choose a network adapter:  ", available_adapter_num);
    scanf("%zu", &choosed_adapter);
    choosed_adapter %= available_adapter_num; // avoid illegal input
    abs( choosed_adapter );
    if(choosed_adapter != 0)
    {
      choosed_adapter -= 1;
    } else {
      choosed_adapter = abs( available_adapter_num ) - 1;
    }
//    printf("%zu\n", choosed_adapter);
 //   printf("%s\n", available_adapter_name[choosed_adapter]);
    memcpy(nic_info->nic_name, available_adapter_name[choosed_adapter], sizeof(nic_info->nic_name));
  }

  return 0;
}

int get_nif_addres(NIC_STRUCT *nic_info)
{
  /*
   * Get the MAC address of an Ethernet interface in C using SIOCGIFHWADDR
   */
  struct ifreq ifr; // ifreq structure contains network interface infos
//  char if_name[IFNAMSIZ] = DEFAULT_NIC_NAME; // network interface name
  char if_name[IFNAMSIZ] = "";
  memcpy(if_name, nic_info->nic_name, sizeof(if_name));
  unsigned char host_MAC[MAC_SIZE] = ""; // Ethernet MAC address
  size_t if_name_len = strlen( if_name ); // ifr_name is a fixed-length buffer, be care about it

  int fd = socket(AF_UNIX, SOCK_DGRAM, 0); // function ioctl() need a socket descriptor as variable
  if(fd == -1)
  {
    printf("%s", strerror(errno));
   // printf("create socket failed\n");
    
   // return -1;
  }
  if( if_name_len < sizeof( ifr.ifr_name ) )
  {
    memcpy( ifr.ifr_name, if_name, if_name_len );  
    ifr.ifr_name[if_name_len] = 0;
  } else {
    printf("interface name is too long");
  }
#if defined(DEBUG)
  printf("Network Interface Name: %s\n", ifr.ifr_name);
#endif

#ifdef SIOCGIFHWADDR
  // get the host( Ethernet interface )  MAC address in C language
  if ( ioctl( fd, SIOCGIFHWADDR, &ifr ) == -1 ) // once have ifreq structure and socket descriptor then we can invoke ioctl()
  {
    int temp_errno = errno;
    close( fd );
    printf("%s", strerror(temp_errno));
    //printf(" get host MAC address procedure error\n");
   // return -1; 
  }
  close( fd );
   
  if ( ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER ) // make sure the type of the network interface we got is a Ethernet interface
  {
    printf("not an Ethernet interface");
  }
  memcpy( host_MAC, ifr.ifr_hwaddr.sa_data, 6);
#endif
  memcpy(nic_info->nic_MAC, ifr.ifr_hwaddr.sa_data, MAC_SIZE);
#if defined(DEBUG)
  printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    host_MAC[0], host_MAC[1], host_MAC[2], host_MAC[3], host_MAC[4], host_MAC[5]);
#endif
  /*
   * Get the IP address of a network interface in C using SIOCGIFADDR
   */
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in* ipaddr;
  if (fd == -1)
  {
    printf("%s", strerror(errno));
  } 
  if (ioctl(fd, SIOCGIFADDR, &ifr) == -1)
  {
    int temp_errno = errno;
    close(fd);
    printf("%s", strerror(temp_errno));
  }
  close(fd);
  ipaddr  = (struct sockaddr_in*)&ifr.ifr_addr;
  //nic_info->ipaddr = ipaddr->sin_addr.s_addr;
  memcpy(&nic_info->ipaddr, &ipaddr->sin_addr, sizeof(struct in_addr));
#if defined(DEBUG)
  printf("IP address: %s\n", inet_ntoa(ipaddr->sin_addr)); // ntoa : network to ASCII
#endif
  
  /*
   * Get the MASK address of a network interface in C using SIOCGIFNETMASK
   */
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in* netmask;
  if(fd == -1)
  {
    printf("%s", strerror(errno));
  }
  if(ioctl(fd, SIOCGIFNETMASK, &ifr) == -1)
  {
    int temp_errno = errno;
    close(fd);
    printf("%s", strerror(temp_errno));
  }
  close(fd);
  netmask = (struct sockaddr_in*)&ifr.ifr_addr;
  //nic_info->netmask = netmask->sin_addr.s_addr;
  memcpy(&nic_info->netmask, &netmask->sin_addr, sizeof(struct in_addr));
#if defined(DEBUG)
  printf("Network Mask: %s\n", inet_ntoa(netmask->sin_addr));
#endif
  
  return 0;
}
