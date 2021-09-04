CC = gcc
CFLAGS = -g
SDIR = sources
ODIR = objects
SRC = pipex.c fork.c error.c initialize.c here_doc.c myfuncs.c
NAME = pipex
MYLIB = mylib/libmylib.a

$(NAME): objdir $(foreach file,$(SRC:.c=.o),$(ODIR)/$(file)) $(MYLIB)
	$(CC) -o $@ $(foreach src,$(SRC:.c=.o),$(ODIR)/$(src)) $(MYLIB)
$(ODIR)/%.o: $(SDIR)/%.c
	cd $(ODIR) && $(CC) $(CFLAGS) -c ../$<
$(MYLIB):
	$(MAKE) --directory=mylib

.PHONY: clean re all fclean bonus objdir
objdir:
	-mkdir objects
all:
	make $(NAME)
clean:
	rm -f $(ODIR)/*.o
fclean:
	make clean
	rm -f $(NAME)
re:
	make clean
	make all
bonus:
	make $(NAME)