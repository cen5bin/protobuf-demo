TARGET = run 

CXX = g++
CPPFLAGS = -g
LDFLAGS = -L/usr/local/lib -lprotobuf 
PROROC = protoc

SRC_DIR = ./src
OBJ_DIR = ./obj
$(shell mkdir -p ${OBJ_DIR})

EXT = cpp cc   #extensions of source file
SOURCE = $(foreach d, $(EXT), $(notdir $(wildcard $(SRC_DIR)/*.$(d)))) 
OBJS = $(foreach src, $(basename $(SOURCE)), $(OBJ_DIR)/$(src).o)
PROTO_DIR = ./protos

edit: $(TARGET)

run: $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@
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

-include $(OBJS:.o=.d)

.PHONY: clean all
clean:
	rm -rf $(OBJS) $(OBJ_DIR)/*.d run

all:
	$(PROROC) -I=$(PROTO_DIR) --cpp_out=$(SRC_DIR) $(PROTO_DIR)/*.proto	
	make	