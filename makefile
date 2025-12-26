SRCDIR		= srcs
SRCS		= $(shell find $(SRCDIR) -name '*.cpp')

OBJDIR		= objs
OBJS		= $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(SRCS)))
OBJDIRS		= $(sort $(dir $(OBJS)))

MAINCPP		= main.cpp

CWD			:= $(shell pwd)
FOLDER		:= $(notdir $(CWD))
INCLUDE_DIR	= includes
HEADER_DIR	= headers
HEADERS		:= $(shell find $(HEADER_DIR) -name '*.hpp')
HEADERS_INC	= $(addprefix -I,$(sort $(dir $(HEADERS))) $(INCLUDE_DIR))

IFLAGS		:= -I. $(HEADERS_INC)

CC			= c++
CFLAGS		= 
#-Wall -Wextra -Werror 
# -fsanitize=address -g3
AR			= ar -rcs
RM			= rm -rf
UP			= \033[1A
FLUSH		= \033[2K

NAME		= library
ARGS		= 

$(NAME): $(OBJDIRS) $(OBJS) $(MAINCPP)
	$(CC) $(CFLAGS) $(OBJS) $(MAINCPP) $(IFLAGS) $(LFLAGS) -o $(NAME)

all: $(NAME)

$(OBJDIRS):
	@mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIRS)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
	@echo "$(UP)$(FLUSH)$(UP)"

clean:
	@$(RM) $(OBJS)

fclean:	clean
	# make -C $(LIBFT_DIR) fclean
	@$(RM) $(NAME)
	@$(RM) $(OBJDIRS)

run:
	./$(NAME)

re: fclean $(NAME)

push:
	@read -p "Commit name: " commit_name; make fclean;	\
	cd $(CWD); git add .; git commit -m "$$commit_name"; git push;	\
	
.PHONY: all clean fclean re push