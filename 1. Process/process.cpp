#include "process.hpp"

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

std::vector<char*> arg_list(const std::string &name, const std::vector <std::string>& args) {
    std::vector<char*> cstyle_args;
    cstyle_args.reserve(args.size() + 1);

    cstyle_args.push_back(const_cast<char*>(name.c_str()));
    for (auto &arg : args)
        cstyle_args.push_back(const_cast<char*>(arg.c_str()));
    cstyle_args.push_back(nullptr);
    
    return cstyle_args;
}

std::string exec_name(const std::string& full_path) {
    std::size_t pos = full_path.rfind('/');
    if (pos == std::string::npos)
        return full_path;
    return full_path.substr(pos);
}





// Process implementation

Process::Process(const std::string& path, const std::vector <std::string>& args) {
    int fd1[2];
    int fd2[2];

    int ret;

    ret = pipe2(fd1, 0);
    if (ret == -1) {
#ifdef DEBUG
        std::cerr << "Pipe1 initialization failed" << std::endl;
#endif
        return;
    }

    ret = pipe2(fd2, 0);
    if (ret == -1) {
#ifdef DEBUG
        std::cerr << "Pipe2 initialization failed" << std::endl;
#endif
        ::close(fd1[0]);
        ::close(fd1[1]);
        return;
    }

    int verif_fd[2];
    ret = pipe2(verif_fd, O_CLOEXEC);
    if (ret == -1) {
        std::cerr << "Exec verification pipe initialization failed" << std::endl;
        ::close(fd1[0]);
        ::close(fd1[1]);
        ::close(fd2[0]);
        ::close(fd2[1]);
        return;
    }

#ifdef DEBUG
    std::cerr << path.c_str() << " " << Process::exec_name(path).c_str() << std::endl;
#endif 
    _pid = fork();

    if (CHILD(_pid)) {
#ifdef DEBUG
        std::cerr << "Forked off successfully" << std::endl;
#endif

        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);

        ::close(fd1[0]);
        ::close(fd1[1]);
        ::close(fd2[0]);
        ::close(fd2[1]);

        execvp(path.c_str(), arg_list(exec_name(path), args).data());

#ifdef DEBUG
        std::cerr << "EXECVP failed" << std::endl;
#endif

        exit(-1);
    } else if (POSIX_ERROR(_pid)) {
        ::close(fd1[0]);
        ::close(fd1[1]);
        ::close(fd2[0]);
        ::close(fd2[1]);
        ::close(verif_fd[0]);
        ::close(verif_fd[1]);

#ifdef DEBUG
        std::cerr << "Forking off failed" << std::endl;
#endif

        return;
    } else {
        ::close(verif_fd[1]);  // Attaching this action to logical part of forking off
    }

    char tmp;
    ssize_t size = ::read(verif_fd[0], &tmp, sizeof(tmp));
    ::close(verif_fd[0]);

    int status;
    pid_t pid_result = waitpid(_pid, &status, WNOHANG);
#ifdef DEBUG
    std::cerr << "PID: " << pid_result << std::endl;
    std::cerr << "Status: " << status << std::endl;
#endif

    bool _noexec = (pid_result != 0);
    if (_noexec)
        throw(std::runtime_error("Execution of programm " + path + " failed"));

    fd_in = fd2[0];
    fd_out = fd1[1];
    //::close(fd2[1]);
    //::close(fd1[0]);
}

Process::~Process() {
    if (!CHILD(_pid)) {
        int status;

        ::close(fd_in);
        ::close(fd_out);

        waitpid(_pid, &status, 0);
#ifdef DEBUG
        std::cerr << WEXITSTATUS(status) << std::endl;
#endif
    }
}

void Process::close() {
    if (!CHILD(_pid)) {
        ::close(fd_in);
        ::close(fd_out);
        kill(_pid, SIGINT);
    } else {
#ifdef DEBUG
        std::cerr << "Trying to kill softly from child" << std::endl;
#endif
        exit(1);
    }
}

void Process::closeStdin() {
#ifdef DEBUG
        std::cerr << "Trying to close Stdin from parent" << std::endl;
#endif
    if (!CHILD(_pid))
        ::close(fd_out);
}

size_t Process::read(void* data, size_t len) {
    size_t ret_val = ::read(fd_in, data, len);

    if (POSIX_ERROR(ret_val))
        throw std::runtime_error("Reading data failed");
#ifdef DEBUG
    else if (!ret_val)
        std::cerr << "Trying to read from a closed pipe" << std::endl;
    else
        std::cerr << "Read " << ret_val << " bytes" << std::endl;
#endif

    return ret_val;
}

void Process::readExact(void* data, size_t len) {
    char *sized_data = static_cast<char *>(data);

    size_t offset = 0;
    while (offset < len) {
        size_t bytes_read = read(sized_data + offset, len);
#ifdef DEBUG
        std::cerr << "Bytes read: " << bytes_read << std::endl;
#endif
        offset += bytes_read;
    }
}

size_t Process::write(const void* data, size_t len) {
    size_t ret_val = ::write(fd_out, data, len);

    if (POSIX_ERROR(ret_val))
        throw std::runtime_error("Writing failed");

#ifdef DEBUG
    std::cerr << "Wrote " << ret_val << " bytes" << std::endl;
#endif

    return ret_val;
}

void Process::writeExact(const void* data, size_t len) {
    size_t offset = 0;
    const char *sized_data = static_cast<const char *>(data);
    while (offset < len) {
        size_t bytes_written = write(sized_data + offset, len);
#ifdef DEBUG
        std::cerr << "Bytes written: " << bytes_written << std::endl;
#endif
        offset += bytes_written;
    }
}

int Process::return_status() const {
    int status;
    waitpid(_pid, &status, WNOHANG);
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else
        return 0;
}