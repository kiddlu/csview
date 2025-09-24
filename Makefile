# Makefile for csview C implementation

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -D_GNU_SOURCE
LDFLAGS = -lcsv -lunistring
TARGET = csview
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Test with example CSV
test: $(TARGET)
	echo "Year,Make,Model,Description,Price" > test.csv
	echo "1997,Ford,E350,\"ac, abs, moon\",3000.00" >> test.csv
	echo "1999,Chevy,\"Venture \"\"Extended Edition\"\"\",\"\",4900.00" >> test.csv
	echo "1999,Chevy,\"Venture \"\"Extended Edition, Large\"\"\",\"\",5000.00" >> test.csv
	echo "1996,Jeep,Grand Cherokee,\"MUST SELL! air, moon roof\",4799.00" >> test.csv
	./$(TARGET) test.csv
	rm -f test.csv

# Test with CJK characters
test-cjk: $(TARGET)
	echo "姓名,城市,符号" > test-cjk.csv
	echo "李磊(Jack),四川省成都市,💍" >> test-cjk.csv
	echo "张三,北京市,🎉" >> test-cjk.csv
	echo "王五,上海市,🌟" >> test-cjk.csv
	./$(TARGET) test-cjk.csv
	rm -f test-cjk.csv