#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#define FPORT 9000

int NB_Ligne(){
char c;
int nb_line = 0;
FILE* file ;

file = fopen("Factures.txt","r") ;
while ((c = getc(file)) != EOF){
    if (c == '\n')
        ++nb_line;
}
return nb_line;
}

void main()
{
    struct FactureP
    {	int   IDPs[100];
        char  *NameP[100];
        double montantP[100];
        double SUMP;
	    int NP;
    } factureP;

    FILE* file ;
    char MSJSYS2[1024]; 
    char ligne[100];
    int i,FileSocket;
    unsigned int serviceAddrL;
    int nb;
    nb=NB_Ligne()+1;
    
    struct sockaddr_in FilesysAddr, serviceAddr; 
    
    // Create a UDP Socket 
    FileSocket = socket(AF_INET, SOCK_DGRAM, 0);     
    printf("[+]FileSocket Socket Created Sucessfully  %d.\n", FileSocket);

    memset(&FilesysAddr, '\0', sizeof(FilesysAddr));
    FilesysAddr.sin_family = AF_INET;
    FilesysAddr.sin_port = htons(FPORT);
    FilesysAddr.sin_addr.s_addr =  htonl (INADDR_ANY);

    memset(&serviceAddr, '\0', sizeof(serviceAddr));
    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_port = htons(FPORT);
    serviceAddr.sin_addr.s_addr = htonl (INADDR_ANY) ;
    
   
    // bind server address to socket descriptor 
    bind(FileSocket, (struct sockaddr*)&FilesysAddr, sizeof(FilesysAddr));
    printf("[+]Bind to Port number %d.\n", 9000); 
    
    while(1){
      
     //receive the datagram 
     //len = sizeof(serviceAddr);
     //receive message from server
     if (recvfrom(FileSocket, MSJSYS2, sizeof(MSJSYS2),0, (struct sockaddr*)&serviceAddr,&serviceAddrL) != -1){ 
    
        printf("[+]%s arrived from proxy : \n", MSJSYS2);
    
        /* Ouverture du fichier (en lecture texte) : */
        file = fopen("Factures.txt","r") ;
        if (file==NULL)
        {
            puts("Pb d'ouverture du fichier essai.dat !");
            exit(0);
        }
        printf("[+]Open File established successfully\n");
      
        factureP.SUMP=0;;
        factureP.NP=0;
        i=0;
        do{   
            fscanf(file,"%s",ligne);
            //printf("%s\n",ligne);
            factureP.IDPs[i] = atoi(ligne);
            fscanf(file,"%s",ligne);
            //printf("%s\n",ligne);
            factureP.NameP[i] = ligne;
            fscanf(file,"%s",ligne);
            //printf("%s\n",ligne);
            factureP.montantP[i] = atof(ligne);
            factureP.SUMP += factureP.montantP[i];
            i = i + 1;
            } while (i<nb);

        factureP.NP = i;

    /*
     printf("\n\n-------- # Facture Parapharmaceutiques # -----------\n\n");
			printf(" Client | Montant \n");
			printf(" ------   -------\n");
			for (int i=0; i<factureP.NP; i++){
				printf(" ------   -------\n");
				printf("   %d    |  %.2f\n",factureP.IDPs[i],factureP.montantP[i]);
			}
			printf("[+]Somme des factures Parapharmaceutiques : %.2f\n", factureP.SUMP);
    */


    
    // send the response 
        sendto(FileSocket,&factureP, sizeof(factureP), 0,(struct sockaddr*)&serviceAddr, sizeof(serviceAddr));
        printf("[+]Facture Parapharmaceutiques is sent to Proxy ..\n"); 
        fclose(file);
        printf("[+]Closing the connection.\n");
     }
     else { printf(" ---- ! Nothing Arrived from proxy ! ------ \n");}     
        
    
    //shutdown(ServiceSocket, SHUT_RDWR);
     close(ServiceSocket);
    }
   
}

//gcc System2.c -o System2



/*fscanf(file,"%s",ligne);

             //puts(ligne);
                if (strcmp(ligne,buffer) == 0){
                    fgets(ligne,255,(FILE*)file);
                    printf("%s\n",ligne);
                    // send the response 
                    sendto(FileSocket,ligne, MAXLINE, 0,(struct sockaddr*)&serviceAddr, sizeof(serviceAddr));
                    printf("[+]Facture is sent to Proxy ..\n"); 
                    break;
                }
                fgets(ligne,255,(FILE*)file);
                fgets(ligne,255,(FILE*)file); 
                i++;
                */