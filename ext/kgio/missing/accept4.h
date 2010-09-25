#ifndef HAVE_ACCEPT4
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#  include <sys/types.h>
#  include <sys/socket.h>
#  ifndef SOCK_CLOEXEC
#    if (FD_CLOEXEC == O_NONBLOCK)
#      define SOCK_CLOEXEC 1
#      define SOCK_NONBLOCK 2
#    else
#      define SOCK_CLOEXEC FD_CLOEXEC
#      define SOCK_NONBLOCK O_NONBLOCK
#    endif
#  endif

/* accept4() is currently a Linux-only goodie */
static int
accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	int fd = accept(sockfd, addr, addrlen);

	if (fd >= 0) {
		if ((flags & SOCK_CLOEXEC) == SOCK_CLOEXEC)
			(void)fcntl(fd, F_SETFD, FD_CLOEXEC);

		/*
		 * Some systems inherit O_NONBLOCK across accept().
		 * We also expect our users to use MSG_DONTWAIT under
		 * Linux, so fcntl() is completely unnecessary
		 * in most cases...
		 */
		if ((flags & SOCK_NONBLOCK) == SOCK_NONBLOCK) {
			int fl = fcntl(fd, F_GETFL);

			if ((fl & O_NONBLOCK) == 0)
				(void)fcntl(fd, F_SETFL, fl | O_NONBLOCK);
		}

		/*
		 * nothing we can do about fcntl() errors in this wrapper
		 * function, let the user (Ruby) code figure it out
		 */
		errno = 0;
	}
	return fd;
}
#endif /* !HAVE_ACCEPT4 */
