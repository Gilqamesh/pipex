CC = gcc
CFLAGS =
SDIR = sources
ODIR = objects
SRC = pipex.c fork.c error.c initialize.c here_doc.c
NAME = pipex
MYLIB = mylib/libmylib.a

$(NAME): $(SRC:.c=.o) $(MYLIB)
	$(CC) -o $@ $(foreach src,$(SRC:.c=.o),$(ODIR)/$(src)) $(MYLIB)
%.o: $(SDIR)/%.c
	cd $(ODIR) && $(CC) $(CFLAGS) -c ../$<
$(MYLIB):
	$(MAKE) --directory=mylib

.PHONY: clean re all fclean bonus
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