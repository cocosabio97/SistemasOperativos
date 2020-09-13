#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


/*
 *  Prototypes
 */

int open_files();
void close_files();
char check_indicators(void);
void read_stats(char *, unsigned long *);
unsigned int read_line(int, char *);


#define DISK "sda"
#define FILENAME_STATS "/proc/diskstats"
#define STATS_FORMAT "%*d %*d %s %*u %*u %*u %*u %lu %*u %*u %*u %*u %*u %*u"
#define DRIVER_FILE "/dev/leds"

unsigned long write_count = 0;
int stats_file;
int driver_file;

int main(int argc, const char *argv[])
{   
    close_files();
    if(open_files() == 0){
            while(1){
                if(check_indicators() == 0)
                    usleep(1000);
            }     
     } 
    
         
        
    return 0;
}

/**
 * Open disk statistics file.
 */
int open_files()
{
	int result = 0;

	// open file
    stats_file = open(FILENAME_STATS, O_RDONLY);
    driver_file = open(DRIVER_FILE, O_WRONLY);
	if (stats_file < 0) {
		fprintf(stderr, "Error\n");
		result = -1;
	}
    if(driver_file < 0){
       fprintf(stderr, "Error\n");
		result = -1;
    }

	return result;
}

/**
 * Close disk statistics file.
 */
void close_files()
{
	if (stats_file)
		close(stats_file);
    if(driver_file)
        close(driver_file);
}


/**
 * Check all indicators and return number of currently active ones.
 */
char check_indicators(void)
{
	char result = 0;
	int changed = 0;
    unsigned long stat_write_count = 0;
    char device_aux[20];

	read_stats(device_aux, &stat_write_count);

	while (strcmp(device_aux, "null") != 0) {
		if (strcmp(device_aux, DISK) != 0){
		    // compare data
            
            if(stat_write_count != write_count)
		        changed = 1;

		    // store new data
		    write_count = stat_write_count;

		    // turn the light on
		    if (changed) {
			    write(driver_file, "2", 1);
			    usleep(1000);

			    // turn it off
			    write(driver_file, "", 1);
			    usleep(1000);

			    // increase result
			    result++;
		    }
	    }

		// read stats again
		read_stats(device_aux, &stat_write_count);
	}

	return result;
}

/**
 * Get content for disk statistics file.
 */
void read_stats(char *device, unsigned long *write_count)
{
	char buff[200] = {0};
	unsigned int count = read_line(stats_file, buff);

	// go to beginning of the file
	if (count == 0) {
		lseek(stats_file, 0, SEEK_SET);

		// let check indicators know we reached end of file
		strcpy(device, "null");
		write_count = 0;
		return;
	}

	// parse line
	sscanf(buff, STATS_FORMAT, device, write_count);
}

unsigned int read_line(int file, char *line)
{
	int length = 0, read_count = 0;
	char character[2];

	while (1) {
		read_count = read(file, character, 1);

		// end of file
		if (read_count == 0)
			break;

		// new line
		if (character[0] == '\0' || character[0] == '\n')
			break;

		line[length] = character[0];
		length += read_count;
	}

	line[length] = '\0';

	return length;
}


