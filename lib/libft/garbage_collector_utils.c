#include "libft.h"

t_list	**getgarbage(void)
{
	static t_list	*collector;

	return (&collector);
}