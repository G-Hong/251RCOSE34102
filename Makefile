CC = gcc
CFLAGS = -Wall -g

# 소스 파일 목록
SRCS = main.c scheduler.c scheduler_utils.c scenario.c queue.c io_handler.c io_event.c io_log.c

OBJS = $(SRCS:.c=.o)

TARGET = scheduler

# 기본 빌드 규칙
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# .c -> .o 개별 빌드 규칙
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean 명령
clean:
	rm -f $(TARGET) *.o
