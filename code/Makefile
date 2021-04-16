CC	= gcc
AS	= as
CFLAGS	= -g -O -Wall
ASFLAGS	= -g
LDFLAGS	= 
RM	= rm

CSRCS1	= etc.c heap.c huffman.c range.c runlength.c lempelziv.c compress.c
CSRCS2	= etc.c heap.c huffman.c range.c runlength.c lempelziv.c decompress.c
#ASRCS	= linkedlist.s
TARGET1	= compress 
TARGET2 = decompress
OBJECTS1	= $(CSRCS1:.c=.o) $(ASRCS:.s=.o)
OBJECTS2	= $(CSRCS2:.c=.o) $(ASRCS:.s=.o)

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJECTS1)
	$(CC) $(LDFLAGS) $(OBJECTS1) -o $@

$(TARGET2): $(OBJECTS2)
	$(CC) $(LDFLAGS) $(OBJECTS2) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.s.o:
	$(AS) $(ASFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJECTS) $(TARGET1) $(TARGET2)