TARGET = libhdfs.so

CXX = g++
CPPFLAGS = -g \
		   -I/usr/local/include

LDFLAGS = -L/usr/local/lib -lprotobuf -luuid
PROROC = protoc

SRC_DIR = ./src
OBJ_DIR = ./obj
$(shell mkdir -p ${OBJ_DIR})

EXT = cpp cc   #extensions of source file
SOURCE = $(foreach d, $(EXT), $(notdir $(wildcard $(SRC_DIR)/*.$(d)))) 
OBJS = $(foreach src, $(basename $(SOURCE)), $(OBJ_DIR)/$(src).o)
PROTO_DIR = ./protos

edit: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -fPIC -shared $^ $(LDFLAGS) -o $@
	cp $@ test/
	@echo "done"

$(OBJ_DIR)/%.d : $(SRC_DIR)/%.cpp
	@rm -f $@
	@echo -n $(OBJ_DIR)/ > $@; \
	$(CXX) -MM $(CPPFLAGS) $< >> $@; \
	echo "\t" $(CXX) -c $(CPPFLAGS) $< -o $(@:.d=.o) $(LDFLAG) >> $@

$(OBJ_DIR)/%.d : $(SRC_DIR)/%.cc
	@rm -f $@
	@echo -n $(OBJ_DIR)/ > $@; \
	$(CXX) -MM $(CPPFLAGS) $< >> $@; \
	echo "\t" $(CXX) -c $(CPPFLAGS) $< -o $(@:.d=.o) $(LDFLAG) >> $@



ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

.PHONY: clean all
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d run

cleanall:
	make clean
	rm -f $(SRC_DIR)/*.pb.*

all:
	$(PROROC) -I=$(PROTO_DIR) --cpp_out=$(SRC_DIR) $(PROTO_DIR)/*.proto	
	make	
