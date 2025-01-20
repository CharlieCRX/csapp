/* 
 * tsh - A tiny shell program with job control
 * 
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <bits/types/sigset_t.h>
#include <bits/sigaction.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */
#define TODO() do{printf("not implemented!\n"); exit(0);} while(0);
/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);
void changeJobStatus(struct job_t *jobs, pid_t pid, int new_status);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
    char buf[MAXLINE];
    char *argv[MAXARGS];
    strcpy(buf, cmdline);

    // 解析命令，并且忽略无参数命令
    int bg = parseline(buf, argv);
    if (argv[0] == NULL) {
        return;
    }
    // 信号初始化
    int pid;
    sigset_t mask_one,prev_one, mask_all;
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);
    sigfillset(&mask_all);

    // 外部命令处理
    if (!builtin_cmd(argv)) {
        // 创建子进程，运行并加载新程序
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
        if ((pid = fork()) == 0) {
            setpgid(0, 0);
            sigprocmask(SIG_SETMASK, &prev_one, NULL); //解除子进程的信号阻塞
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }
        }

        // job 控制
        sigprocmask(SIG_BLOCK, &mask_all, NULL);  // 阻塞所有信号
        addjob(jobs, pid, (bg ? BG : FG), cmdline);
        sigprocmask(SIG_SETMASK, &prev_one, NULL);

        if(bg) {
            printf("[%d] (%d) %s",pid2jid(pid), pid, cmdline);
        }
        else {
            waitfg(pid);
        }
        
    }


    return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{
    if (strcmp(argv[0], "jobs") == 0) {
        listjobs(jobs);
        return 1;
    } else if (strcmp(argv[0], "fg") == 0) {
        do_bgfg(argv);
        return 1;
    } else if (strcmp(argv[0], "bg") == 0) {
        do_bgfg(argv);
        return 1;
    } else if (strcmp(argv[0], "quit") == 0) {
        exit(0);
    }
    return 0;
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    struct job_t *pjob;
    // 将任务运行(SIGCONT)在后台
    if (strcmp(argv[0], "bg") == 0) {
        if (argv[1] == NULL) {
            printf("bg command requires PID or %%jobid argument\n");
            return;
        }
        // 获取此任务信息
        if (argv[1][0] == '%') {
            char dest[10];
            strcpy(dest, argv[1] + 1);
            int jid = atoi(dest);
            pjob = getjobjid(jobs, jid);
        } else {
            // pid 处理
            char dest[10];
            strcpy(dest, argv[1]);
            int pid = atoi(dest);
            pjob = getjobpid(jobs, pid);
        }

        if (pjob == NULL) {
            printf("(%s): No such process\n", argv[1]);
            return;
        }

        // 修改任务状态
        sigset_t mask, prev;
        sigfillset(&mask);
        sigprocmask(SIG_BLOCK, &mask, &prev);
        kill(-pjob->pid, SIGCONT);
        changeJobStatus(jobs, pjob->pid, BG);
        printf("[%d] (%d) %s", pjob->jid, pjob->pid, pjob->cmdline);
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }
    // 将任务运行在前台
    else {
        if (argv[1] == NULL) {
            printf("fg command requires PID or %%jobid argument\n");
            return;
        }
        // 获取此任务信息
        if (argv[1][0] == '%') {
            char dest[10];
            strcpy(dest, argv[1] + 1);
            int jid = atoi(dest);
            pjob = getjobjid(jobs, jid);
        } else {
            // pid 处理
            char dest[10];
            strcpy(dest, argv[1]);
            int pid = atoi(dest);
            pjob = getjobpid(jobs, pid);
        }

        if (pjob == NULL) {
            printf("(%s): No such process\n", argv[1]);
            return;
        }

        // 修改任务状态
        sigset_t mask, prev;
        sigfillset(&mask);
        sigprocmask(SIG_BLOCK, &mask, &prev);
        kill(-pjob->pid, SIGCONT);
        changeJobStatus(jobs, pjob->pid, FG);
        sigprocmask(SIG_SETMASK, &prev, NULL);

        // 显式地等待前台任务
        waitfg(pjob->pid);
    }
    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    sigset_t mask, prev;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    // 确保父进程在等待前台作业完成时，不会被非前台子进程的退出信号打断
    sigprocmask(SIG_BLOCK, &mask, &prev);
    while(fgpid(jobs) != 0) {
        sigsuspend(&prev);
    }

    if (verbose) {
        printf("waitfg: Process (%d) no longer the fg process\n", pid);
    }
    sigprocmask(SIG_SETMASK, &prev, NULL);
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int olderrno = errno;
    if (verbose) {
        printf("sigchld_handler: entering\n");
    }
    
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    int pid, statusp;
    while ((pid = waitpid(-1, &statusp, WNOHANG | WUNTRACED)) > 0)
    {
        // 删除此终止或暂停的子进程
        struct job_t *pjob =  getjobpid(jobs, pid);
        int jid = pjob->jid;

        if (WIFEXITED(statusp)) {
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            deletejob(jobs, pjob->pid);
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        } else if (WIFSIGNALED(statusp)){   // 信号终止
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            deletejob(jobs, pjob->pid);
            printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(statusp));
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        } else if (WIFSTOPPED(statusp)){
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            // 修改此子进程的作业为停止状态
            changeJobStatus(jobs, pid, ST);
            printf("Job [%d] (%d) stopped by signal %d\n", jid, pid, WSTOPSIG(statusp));
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }

    }
    
    errno = olderrno;
    if (verbose) {
        printf("sigchld_handler: exiting\n");
    }
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    int olderrno = errno;

    if (verbose) {
        printf("sigint_handler: entering\n");
    }
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    // 只处理前台作业
    pid_t fg_pid = fgpid(jobs);
    if (fg_pid == 0) {
        errno = olderrno;
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        return;
    }
    
    kill(-fg_pid, SIGINT);
    sigprocmask(SIG_SETMASK, &prev_all, NULL);
    if (verbose) {
        printf("sigint_handler:Job [%d] (%d) killed\n", pid2jid(fg_pid), fg_pid);
    }

    errno = olderrno;
    if (verbose) {
        printf("sigint_handler: exiting\n");
    }
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    int olderrno = errno;
    if (verbose) {
        printf("sigtstp_handler: entering\n");
    }

    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    // 只处理前台作业
    pid_t fg_pid = fgpid(jobs);
    if (fg_pid == 0) {
        errno = olderrno;
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        return;
    }
    kill(-fg_pid, SIGTSTP);
    if (verbose) {
        printf("sigtstp_handler: Job [%d] (%d) stopped\n", pid2jid(fg_pid), fg_pid);
    }
    errno = olderrno;
    if (verbose) {
        printf("sigtstp_handler: exiting\n");
    }
    
    sigprocmask(SIG_SETMASK, &prev_all, NULL);
    return;
}

void sigquit_handler(int sig) {
    exit(0);// todo
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] (%d) %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
        if (verbose) {
            printf("sigchld_handler: Job [%d] (%d) deleted\n", jobs[i].jid, jobs[i].pid);
        }
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
	}
    }
}

/* changeJobStatus - 修改特定pid作业的状态 */
void changeJobStatus(struct job_t *jobs, pid_t pid, int new_status) {
    struct job_t *pjob = getjobpid(jobs, pid);
    if (pjob == NULL) {
        printf("Job is NULL!\n");
        exit(0);
    }

    pjob->state = new_status;
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}



