#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <signal.h>

// t_client is a struct that stores the display_tty of the client
typedef struct s_client
{
	char *display_tty;
	int display_fd;
	int net_socket;
}	t_client;

t_client client;

// new_terminal_macOS creates a new terminal window and returns the tty of the new terminal window
char *new_terminal_macOS()
{
	// Get the list of ttys before opening the terminal window
	// The "ps -o tty,comm | grep zsh" command lists all of the ttys that are running zsh
	FILE* fp = popen("ps -o tty,comm | grep zsh", "r");
	if (fp == NULL)
	{
		perror("Error: popen failed");
		return NULL;
	}
	int num_tasks = 0;
	char ttys[256][256];
	char line[256];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		sscanf(line, "%s", ttys[num_tasks]);
		++num_tasks;
	}
	pclose(fp);

	// Open a new terminal window
	system("open -n -a Terminal");

	// Wait for the terminal window to open
	sleep(1);

	// Get the tty of the terminal window
	fp = popen("ps -o tty,comm | grep zsh", "r");
	if (fp == NULL)
	{
		perror("Error: popen failed");
		return NULL;
	}
	// Name of the tty
	char tty[256];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		sscanf(line, "%s", tty);

		// Check if the tty is not in the list of ttys before opening the terminal window
		int found = 0;
		for (int i = 0; i < num_tasks; i++)
		{
			if (strcmp(ttys[i], tty) == 0)
			{
				found = 1;
				break;
			}
		}
		if (!found)
		{
			// The tty is the terminal window that was opened by the C program
			break;
		}
	}
	pclose(fp);

	char *tty_str = malloc(strlen(tty) + 1);
	strcpy(tty_str, tty);

	return tty_str;
}

char *new_terminal_linux()
{
	FILE* fp = popen("ps a -o tty,comm | grep bash", "r");
	if (fp == NULL)
	{
		perror("Error: popen failed");
		return NULL;
	}
	int num_tasks = 0;
	char ttys[256][256];
	char line[256];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		sscanf(line, "%s", ttys[num_tasks]);
		++num_tasks;
	}
	pclose(fp);

	system("gnome-terminal");

	sleep(1);

	fp = popen("ps a -o tty,comm | grep bash", "r");
	if (fp == NULL)
	{
		perror("Error: popen failed");
		return NULL;
	}
	char tty[256];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		sscanf(line, "%s", tty);
		int found = 0;
		for (int i = 0; i < num_tasks; i++)
		{
			if (strcmp(ttys[i], tty) == 0)
			{
				found = 1;
				break;
			}
		}
		if (!found)
		{
			// The tty is the terminal window that was opened by the C program
			break;
		}
	}
	pclose(fp);

	char *tty_str = malloc(strlen(tty) + 1);
	strcpy(tty_str, tty);

	return tty_str;
}

// create_tty_path creates a path string for the given tty
char *create_tty_path(char* tty)
{
	// Allocate memory for the tty path string and create the string by concatenating "/dev/" and the tty
	char* tty_string = malloc(strlen("/dev/") + strlen(tty) + 1);
	sprintf(tty_string, "/dev/%s", tty);
	// Free the memory allocated for the tty string
	free(tty);
	return tty_string;
}

// display_incoming_message is a function that is executed by a separate thread
// It will display incoming messages from the server on the terminal window
void *display_incoming_message(void *param)
{
	char message[512];
	client.display_fd = open(client.display_tty, O_WRONLY);
	int i;

	write(client.display_fd, "\n", 1);
	// Read server output and send it to the opened terminal
	while (1)
	{
		i = read(client.net_socket, message, 256);
		if (i == -1)
			break ;
		else if (i == 0)
		{
			write(client.display_fd, "Server connection lost\n", 24);
			close(client.net_socket);
			exit(0);
		}
		i = strlen(message);
		message[i] = '\n';
		message[i + 1] = '\0';
		write(client.display_fd, message, strlen(message) + 1);
		if (strcmp(message, "Session ended\n") == 0)
		{
			close(client.net_socket);
			close(client.display_fd);
			exit(0);
		}
	}
	return (NULL);
}

// receiver is a function that is called when the program receives a SIGINT signal (Ctrl+C)
// It sends an "END_SESSION" message to the server and closes the network socket, tty, and stdout
void receiver(int signal)
{
	close(client.net_socket);
	write(client.display_fd, "Session ended\n", 15);
	close(client.display_fd);
	exit(0);
}

int main()
{
	// request is a buffer to store user input
	char request[256];
	// tid is a thread identifier for the display_incoming_message thread
	pthread_t tid;

	// Get system info to open suitable terminal
	struct utsname sys_info;
	uname(&sys_info);

	// Create the network socket
	client.net_socket = socket(AF_INET, SOCK_STREAM, 0);
	// Set the receiver function to be called when the program receives a SIGINT signal
	signal(SIGINT, &receiver);

	// Set up the server address to connect to
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// Connect to the server
	int status = connect(client.net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
	if (status == -1)
		printf("Connection error!\n");
	else
	{
		printf("Connected to the server successfully!\n");
		// Create a new terminal window and store the tty in the client struct
		char *tty;
		// Open Terminal app on macOS
		// if (strcmp(sys_info.sysname, "Darwin") == 0)
		// 	tty = new_terminal_macOS();
		// // Open gnome-terminal on Linux
		// else if (strcmp(sys_info.sysname, "Linux") == 0)
		// 	tty = new_terminal_linux();
		// if (tty == NULL)
		// 	return (1);
		// client.display_tty = create_tty_path(tty);
		// pthread_create(&tid, NULL, &display_incoming_message, &client);
		// Read user input and send it to the server
		// while (1)
		{
			read(0, request, 256);
			int i = 0;
			// Remove newline from the line end
			while (request[i] != '\n')
				++i;
			request[i] = '\0';
			write(client.net_socket, request, 256);
		}
	}
	return (0);
}
