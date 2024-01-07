/* 1.Tạo các #include cần thiết */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
/* dành riêng cho AF_INET */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PORT 2205
#define BUFFER_SIZE 1024
#define MAX_EXPRESSION_LENGTH 225

// Cấu trúc dữ liệu stack 
typedef struct {
    int top;
    float items[MAX_EXPRESSION_LENGTH];
} Stack;


// Hàm kiểm tra xem stack có rỗng không
bool isEmpty(Stack* stack) {
    return stack->top == -1;
}

// Hàm đẩy phần tử vào stack
void push(Stack* stack, int item) {
    if (stack->top == MAX_EXPRESSION_LENGTH - 1) {
        fprintf(stderr, "Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    stack->items[++stack->top] = item;
}

// Hàm lấy phần tử từ stack và giảm top
int pop(Stack* stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack->items[stack->top--];
}

// hàm kiểm tra 1 kí tự có là toán tử hay không
bool isOperator(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/';
}

// hàm kiểm tra 1 kí tự có là chữ số hay không
bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

// Hàm kiểm tra tính hợp lệ của biểu thức
bool isValidExpression(char* expression) {
    int numOpeningParentheses = 0;
	int numClosingParentheses = 0;

	int len = strlen(expression);

	for (int i = 0; i < len; i++) {
		// 1. nếu nó có các kí tự khác digits, () và các toán tử thì nó không hợp lệ
		if (!isDigit(expression[i]) && !isOperator(expression[i]) && expression[i] != '(' && expression[i] != ')') {
			return false;
		}

		// 2. nếu là hai toán tử liên tiếp thì không hợp lệ
		if (isOperator(expression[i]) && (i + 1) < len && isOperator(expression[i + 1])) {
			return false;
		}

		// 3. nếu là () thì không hợp lệ, 
		if (expression[i] == '(' && (i + 1) < len && expression[i + 1] == ')') {
			return false;
		}

		// 4. nếu là toán tử rồi đến () thì không hợp lệ
		if (isOperator(expression[i]) && (i + 1) < len && (expression[i + 1] == ')' || expression[i + 1] == '(')) {
			return false;
		} 

		// 5. nếu là (  rồi đến toán tử thì không hợp lệ
		if (expression[i] == '('  && (i + 1) < len && isOperator(expression[i + 1])) {
			return false;
		}

		// 6. nếu là phép chia 0
		if (isOperator(expression[i]) && (i + 1) < len && expression[i + 1] == '0' && expression[i] == '/') {
			return false;
		}

		if (expression[i] == '(') {
			numOpeningParentheses++;
		} else if (expression[i] == ')') {
			if (numOpeningParentheses == numClosingParentheses) {
				return false;
			}
			numClosingParentheses++;
		}
	}

	if (numOpeningParentheses != numClosingParentheses) {
		return false;
	}
	return true;
}

enum Operator {
	PLUS = '+',
	MINUS = '-',
	MULTIPLY = '*',
	DIVIDE = '/'
};

// hàm tính toán 2 số với 1 toán tử
float applyOperator(float a, float b, enum Operator op) {
	switch (op) {
		case PLUS:
			return a + b;
		case MINUS:
			return a - b;
		case MULTIPLY:
			return a * b;
		case DIVIDE:
			return a / b;
		default:
			return 0;
	}
}

// func to check the priority of operator
int priority(enum Operator op) {
	switch (op) {
		case PLUS:
		case MINUS:
			return 1;
		case MULTIPLY:
		case DIVIDE:
			return 2;
		default:
			return 0;
	}
}

// func to remove all spaces in a string
void removeSpaces(char* str) {
	int len = strlen(str);
	int i = 0;
	int j = 0;
	while (i < len) {
		if (str[i] != ' ') {
			str[j++] = str[i];
		}
		i++;
	}
	str[j] = '\0';
}

// func to calculate the expression
float calculate(char *expression) {
	float result = 0;
	int len = strlen(expression);

	// stack to store numbers
	Stack numbers;
	numbers.top = -1;

	// stack to store operators
	Stack operators;
	operators.top = -1;

	for (int i = 0; i < len; i++) {
		// nếu là số thì đẩy vào stack
		if (isDigit(expression[i])) {
			int num = 0;
			while (i < len && isDigit(expression[i])) {
				num = num * 10 + (expression[i] - '0');
				i++;
			}
			i--;
			push(&numbers, num);
		} else if (expression[i] == '(') {
			push(&operators, expression[i]);
		} else if (expression[i] == ')') {
			// nếu là ) thì lấy 2 số và 1 toán tử ra khỏi stack để tính toán
			while (!isEmpty(&operators) && operators.items[operators.top] != '(') {
				float b = pop(&numbers);
				float a = pop(&numbers);
				enum Operator op = (enum Operator) pop(&operators);
				push(&numbers, applyOperator(a, b, op));
			}
			// lấy ( ra khỏi stack
			pop(&operators);
		} else if (isOperator(expression[i])) {
			// nếu là toán tử thì lấy toán tử ra khỏi stack để tính toán
			while (!isEmpty(&operators) && priority((enum Operator) operators.items[operators.top]) >= priority((enum Operator) expression[i])) {
				float b = pop(&numbers);
				float a = pop(&numbers);
				enum Operator op = (enum Operator) pop(&operators);
				push(&numbers, applyOperator(a, b, op));
			}

			// đẩy toán tử vào stack
			push(&operators, expression[i]);
		}
	}

	// tính toán các phần tử còn lại trong stack
	while (!isEmpty(&operators)) {
		float b = pop(&numbers);
		float a = pop(&numbers);
		enum Operator op = (enum Operator) pop(&operators);
		push(&numbers, applyOperator(a, b, op));
	}

	// lấy kết quả ra khỏi stack
	result = pop(&numbers);
	return result;
	
}

void* clientHandler(void* arg) {
    int client_sockfd = *((int*)arg);
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    char expression[BUFFER_SIZE];
    char result_str[BUFFER_SIZE];


    // Get client address
    getpeername(client_sockfd, (struct sockaddr*)&client_address, &client_len);
    printf("\nConnection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    while (1) {

		// clear expression
		memset(expression, 0, BUFFER_SIZE);

        // Read expression from client
        ssize_t bytesRead = read(client_sockfd, expression, BUFFER_SIZE);

        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                printf("\nClient %s:%d closed the connection.\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            } else {
                perror("Reading data from client failed.");
            }
            close(client_sockfd);
            break;
        }

        printf("\nReceived expression from client %s:%d: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), expression);

        removeSpaces(expression);

		// clear result string
		memset(result_str, 0, BUFFER_SIZE);

        if (isValidExpression(expression) == 0) {
            printf("\nInvalid expression received from client %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            sprintf(result_str, "Invalid expression.");
        } else {
            // Calculate the result
            float result = calculate(expression);
            int size = sizeof(result_str);
			printf("Result: %.1f\n", result);
            snprintf(result_str, size, "%.1f", result);
        }

        // Send result to client
        if (write(client_sockfd, result_str, strlen(result_str)) == -1) {
            perror("Writing to client failed.");
            close(client_sockfd);
            break;
        }
    }

    return NULL;
}


int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	
	/* 2. Thực hiện khởi tạo socket mới cho trình chủ */
	server_sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	
	/* 3. Đặt tên và gán địa chỉ kết nối cho socket theo giao thức Internet */
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	server_address.sin_port = htons( PORT );
	server_len = sizeof( server_address );
	
	/* 4. Ràng buộc tên với socket */
	bind( server_sockfd, (struct sockaddr *)&server_address, server_len );
	
	/* 5. Mở hàng đợi nhận kết nối - cho phép đặt hàng vào hàng đợi tối đa 10 kết nối */
	listen( server_sockfd, 10 );

	printf("Server is running on port %d...\n", PORT);
	
	/* 6. Lặp vĩnh viễn để chờ và xử lý kết nối của trình khách */
	while (1) {
        // Chờ và chấp nhận kết nối
        client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);

        // Create a new thread for each client connection
        pthread_t thread;
        if (pthread_create(&thread, NULL, clientHandler, &client_sockfd) != 0) {
            perror("Failed to create thread.");
            close(client_sockfd);
            continue;
        }

        // Detach the thread to clean up resources automatically
        pthread_detach(thread);
    }
	

	// Đóng socket
	close( server_sockfd);
	return 22;
}