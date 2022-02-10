#
# Makefile
#

#　最終ファイル
TARGET = MYDS.exe

#　ソースファイル(*.c)の一覧
SRCS = MYDS.c 
# prog2.c 
# 	prog3.c

#　オブジェクトファイル(*.o)の一覧	
OBJS = ${SRCS:.c=.o}

#　ヘッダファイルの知覧(今は何もなし)
HEADERS = 

#　コンパイラ・リンカの指定	
CC = i686-pc-cygwin-gcc
CCFLAGS = -Wall -I/usr/include/opengl
LD = i686-pc-cygwin-gcc
LDFLAGS = 
LIBS = -lm -lglpng -lglut32 -lglu32 -lopengl32 myicon.o

#　OBJSからTAEGWTを作る方法	
$(TARGET) : $(OBJS) 
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET) $(LIBS)

#　*.c から*.oを作る方法
.c.o :
	$(CC) $(CCFLAGS) -c $<

#　*.o はHEADERSとMakefileに依存(これらが書き換わったときにも*.oを再構築)
$(OBJS) : $(HEADERS) Makefile

#　make clean としたときに実行されるコマンド
clean :
	rm -f $(TARGET) $(OBJS) core *^