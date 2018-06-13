SRC_DIR=src
TGT_DIR=target
OBJ_DIR=objs

define make-depend-compile
    @echo "[CC]  $1"
    @mkdir -p $(dir $2)
    @$(CC) -MM -MF $3 -MP -MT $2 $4 $1
    @$(CC) $4 -o $2 -c $1
endef

$(SRCS) = $(wildcard *.c)
$(OBJ) = $(patsubst %.c, $(OBJ_DIR)/%.o,$(SRCS))

all: $(OUT)
-include $(OBJ:.o=.d)

