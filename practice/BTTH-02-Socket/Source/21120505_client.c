/* 1. Tạo các #include cần thiết để gọi hàm socket */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
/* dành riêng cho AF_INET */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


#define PORT 2205
#define BUFFER_SIZE 1024

int main()
{
	int sockfd; /* số mô tả socket – socket handle */
	int len;
	struct sockaddr_in address; /* structure sockaddr_in, chứa các thông tin về socket AF_INET */
	int result;
	char ch = 'A';
	
	/* 2. Tạo socket cho trình khách. Lưu lại số mô tả socket */
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	
	/* 3. Đặt tên và gán địa chỉ kết nối cho socket theo giao thức Internet */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	address.sin_port = htons( PORT );
	len = sizeof( address );
	
	/* 4. Thực hiện kết nối */
	result = connect( sockfd, (struct sockaddr*)&address, len );
	if ( result == -1 ) {
		perror( "Oops: client1 problem" );
		return 1;
	}

	printf( "Connected to server on port %d\n", PORT );
	
	/* 5. Sau khi socket kết nối, chúng ta có thể đọc ghi dữ liệu của socket tương tự đọc ghi trên file */


	while (1) {
		char user_input[BUFFER_SIZE];

		// clear user input
		memset(user_input, 0, BUFFER_SIZE);

		// get user input
		printf("Enter expression (e.g. 1 + 2) or 'q' to quit: ");
		fgets(user_input, BUFFER_SIZE, stdin);

		// remove the newline character
		if (user_input[strlen(user_input) - 1] == '\n') {
			user_input[strlen(user_input) - 1] = '\0';
		}

		// check if user wants to quit
		if (strcmp(user_input, "q") == 0) {
			break;
		}

		printf("Sending expression: %s\n", user_input);

		// send user input to server
		write(sockfd, user_input, strlen(user_input));

		char result_str[BUFFER_SIZE];

		// clear result string
		memset(result_str, 0, BUFFER_SIZE);

		// read result from server
		read(sockfd, result_str, BUFFER_SIZE);

		// print result
		printf("Result: %s\n", result_str);
	}



	close( sockfd );
	printf("Connection closed.\n");

	return 22;
}