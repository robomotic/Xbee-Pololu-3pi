	#include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <termios.h>
        #include <stdio.h>

        /* baudrate settings are defined in <asm/termbits.h>, which is
        included by <termios.h> */
        #define BAUDRATE B9600            
        /* change this definition for the correct port */
        #define MODEMDEVICE "/dev/ttyUSB1"
        #define _POSIX_SOURCE 1 /* POSIX compliant source */

        #define FALSE 0
        #define TRUE 1

        volatile int STOP=FALSE; 

	FILE *output;
	char In1, Key;
	int Data_Bits = 8;              // Number of data bits
	int Stop_Bits = 1;              // Number of stop bits
	int Parity = 0;                 // Parity as follows:
                  // 00 = NONE, 01 = Odd, 02 = Even, 03 = Mark, 04 = Space

        main()
        {
          int fd,c, res,i;
	  char buf[4];
	  char message[90];
          struct termios oldtio,newtio;
	  output = fopen("datalog.txt", "w");     //open the terminal screen

        /* 
          Open modem device for reading and writing and not as controlling tty
          because we don't want to get killed if linenoise sends CTRL-C.
        */
         fd = open(MODEMDEVICE,O_RDWR | O_NOCTTY | O_NDELAY);
         if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
        
        	  
	  if (!output)
	  {
	  fprintf(stderr, "Unable log data\n");
	  exit(1);
	  }
   
         tcgetattr(fd,&oldtio); /* save current serial port settings */
         bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
        
        /* 
          BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters
        */
         newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
         
        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          otherwise make device raw (no other input processing)
        */
         newtio.c_iflag = IGNPAR | ICRNL;
         
        /*
         Raw output.
        */
         newtio.c_oflag = 0;
         
        /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to calling program
        */
         newtio.c_lflag = ICANON;
         
        /* 
          initialize all control characters 
          default values can be found in /usr/include/termios.h, and are given
          in the comments, but we don't need them here
        */
         newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
         newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
         newtio.c_cc[VERASE]   = 0;     /* del */
         newtio.c_cc[VKILL]    = 0;     /* @ */
         newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
         newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
         newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
         newtio.c_cc[VSWTC]    = 0;     /* '\0' */
         newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
         newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
         newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
         newtio.c_cc[VEOL]     = 0;     /* '\0' */
         newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
         newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
         newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
         newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
         newtio.c_cc[VEOL2]    = 0;     /* '\0' */
        
        /* 
          now clean the modem line and activate the settings for the port
        */
         tcflush(fd, TCIFLUSH);
         tcsetattr(fd,TCSANOW,&newtio);
        
        /*
          terminal settings done, now handle input
          In this example, inputting a 'z' at the beginning of a line will 
          exit the program.
        */
         while (STOP==FALSE) {     /* loop until we have a terminating condition */
         /* the robot is sending 4 bytes in the buffer */
            res = read(fd,buf,4); 
	    printf("Res %d \n",res);
	    if (res>0)
            {
	       printf("Buffer full\n");
               for (i=0; i<res; i++)  //for all chars in string
               {
                  In1 = buf[i];
			//decimal
                        sprintf(message,"%d ",In1);

                        fputs(message,output);
			if(i<res-2)
			fputc(',',output);
			else if(i==res-1)
			fputc('\n',output);
                        break;
                 
               }  //end of for all chars in string
	    }
               
         }
         /* close the log file */
         fclose(output);
         /* restore the old port settings */
         tcsetattr(fd,TCSANOW,&oldtio);
        }
