CC = gcc
CFLAGS = -Wall -g

SRCS = main.c scheduler.c scenario.c queue.c io.c scheduler_utils.c
OBJS = $(SRCS:.c=.o)

TARGET = scheduler

# 기본 빌드 규칙
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# .c -> .o 규칙
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
