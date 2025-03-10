NAME = minishell

HEADER = ./inc/

SRCS = minishell.c execute.c parse_spec_symbs.c pathfinder.c builtin_commands.c\
	   env_utils.c builtin_env.c builtin_export.c builtin_cd.c\
	   exit_shell.c preparser.c utilsforparser.c exec_piped.c\
	   parse_redirect.c parse_args_arr.c deal_redir.c signal.c\
	   free_mem.c exec_extern.c exec_builtin.c builtin_echo.c del_empty_args.c

OBJSDIR = ./objs
SRCSDIR = ./srcs
HDRDIR = ./inc
LIBFTDIR = ./libft

LIBFT = $(LIBFTDIR)libft.a

CC = gcc


RLFLAGS = -lreadline -L/Users/$(USER)/.brew/Cellar/readline/8.1/lib\
		  -I/Users/$(USER)/.brew/Cellar/readline/8.1/include
FLAGS = -Wall -Werror -Wextra -g

OBJS = $(addprefix $(OBJSDIR)/,$(SRCS:.c=.o))

all: lib $(NAME)

$(OBJSDIR)/%.o: $(SRCSDIR)/%.c $(HEADER)
	@$(CC) $(FLAGS) -I $(HDRDIR) -I $(LIBFTDIR) -c $< -o $@

lib:
	@make bonus -C $(LIBFTDIR)

$(NAME): $(OBJSDIR) $(OBJS)
	$(CC) -L$(LIBFTDIR) -lft $(OBJS) $(RLFLAGS) -o $(NAME)

$(OBJSDIR):
	@mkdir -p $(OBJSDIR)

clean:
	rm -f $(OBJS)
	rm -rf $(OBJSDIR)
	make clean -C $(LIBFTDIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBFTDIR)

re: fclean all

.PHONY: all clean fclean re
